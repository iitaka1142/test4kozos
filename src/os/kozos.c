#include "../misc/misra2012deviation.h"
#include "../misc/defines.h"
#include "../misc/intr.h"
#include "../misc/interrupt.h"
#include "../misc/lib.h"
#include "syscall.h"
#include "memory.h"
#include "kozos.h"

#define THREAD_NUM 6
#define PRIORITY_NUM 16
#define THREAD_NAME_MAX_SIZE 15

/** Thread context */
typedef struct _kz_context {
  stack_ptr_t sp; ///< stack pointer
} kz_context;

/// Task Control Block(TCB)
typedef struct _kz_thread kz_thread;
struct _kz_thread {
  kz_thread * next; ///< use for queue
  
  char name [THREAD_NAME_MAX_SIZE + 1];
  int priority; ///< priority of thread
  char * stack; ///< pointer to stack area of thread
  union {
    struct {
      Bool ready : 1;
    } bit;

    uint32 array;
  } flag; ///< some flags

  /** parameters for startup(thread_init()) */
  struct {
    kz_func_t func;
    int argc;
    char ** argv;
  } init;
  
  /** buffer for system call */
  struct {
    kz_syscall_type_t type;
    kz_syscall_param_t * param;
  } syscall;
  
  kz_context context;
  
} __attribute__((aligned(64)));
/**<
   アセンブラでは以下のようにしてアドレスを計算する。
   1. インデックス X 要素サイズ
      <= ここで32bit X 32bit = 64bitの命令を呼び出そうとしてリンクエラーになる
         ※  最終的に32bit(アドレス値のサイズ)で結果が欲しいので、最初に関連する数値を32bitにしている
   2. オフセット + (1.の結果)

   1.の計算でシフト演算を使用するようにすれば、問題は起こらない。
   つまり、要素のサイズを2の冪乗にして以下のような計算に置き換える

   1'. インデックスをlog_2(要素サイズ(bit))左へビットシフト

   構造体のサイズは56byteなので、これを64byteの境界へ揃える
*/

/// ready queue of thread
static struct {
  kz_thread * head; ///< head item of queue
  kz_thread * tail; ///< last item of queue
} readyque[PRIORITY_NUM];


typedef struct _msg_buf kz_msgbuf;
struct _msg_buf {
  kz_msgbuf * next;
  
  kz_thread * sender;
  struct {
    unsigned int size;
    char * data_ptr;
  } param;
};
  
typedef struct _msg_box kz_msgbox;
struct _msg_box{
  kz_thread * reciever;
  kz_msgbuf * head;
  kz_msgbuf * tail;
} __attribute__((aligned(16)));
/**<
   kz_msgboxは、4byte(uint32) X 3 = 12byte なので、各要素を16byteの境界へ揃えれば良い
 */

static kz_thread * current; ///< current thread
static kz_thread threads[THREAD_NUM]; ///< Task Control Block(TCB)
static kz_handler_t handlers[SOFTVEC_TYPE_NUM]; ///< interrupt handler
static kz_msgbox msg_boxes[MSGBOX_ID_NUM];

#if 0
/// for debug
void dump_que(void)
{
  kz_thread * this;
  for( int i = 0; i < PRIORITY_NUM; i++ ) {
    putxval(i,2); puts(": ");
    this = readyque[i].head;
    while(this != NULL) {
      puts(this->name); puts(" ");
      this = this->next;
    }
    puts("\n");
  }
}
#endif

void dispatch(kz_context * context) __attribute__((noreturn));
/**<
   @see startup.s
*/

static void thread_intr(softvec_type_t type, stack_ptr_t sp) __attribute__((noreturn));
/**<
    interrupt handler
*/

/** exit thread */
static void thread_end( void )
{
  kz_exit();
}

/// start up thread
static void thread_init( kz_thread * thread )
{
  /* call main function of thread */
  thread->init.func(thread->init.argc, thread->init.argv);
  thread_end();
}
 
/// get 1 thread from ready queue
/// !!!side effect!!! queue will lost its top element
static kz_thread * get( int priority )
{
  if( priority < 0 || PRIORITY_NUM <= priority ) {
    return NULL;
  }

  kz_thread * result = readyque[priority].head;

  if(result == NULL){
    // if queue is empty, return NULL
    // (do not have to do anything)
  }
  else if(result->flag.bit.ready == false){
    // thread at top of queue hasn't been in ready state yet
  }
  else{
    // set next item
    readyque[priority].head = readyque[priority].head->next;
    
    // set ready flag as false
    result->flag.bit.ready = false;

  }
  
  if(result->next == NULL){
    // queue has only 1 element and become empty
    readyque[priority].tail = NULL;
  }
  
  return result;
}

/// put 1 thread to ready queue
/// (ensure) queue is not empty otherwise exit with Error
static int put( kz_thread * thread )
{
  if(thread == NULL) {
    return -1;
  }
  else if(thread->flag.bit.ready == true) {
    // if thread has already been in ready state
    // don't do anything
  }
  else if(readyque[thread->priority].head == NULL) {
    // queue is empty
    readyque[thread->priority].head = thread;
  }
  else {
    // append given thread to ready queue
    readyque[thread->priority].tail->next = thread;
  }

  // reset a next element of tail
  thread->next = NULL;
  
  readyque[thread->priority].tail = thread;
  thread->flag.bit.ready = true;
  
  return EXIT_SUCCESS;
}

/// create TCB
static inline kz_thread * create_TCB( void )
{
  kz_thread * thread = NULL;
  
  // search for vacant TCB
  for(int i = 0; (thread == NULL) || (i < THREAD_NUM); i++){
    if(threads[i].init.func == NULL) {
      thread = &(threads[i]);
      memset(thread, 0, sizeof(*thread));
    }
  }
  
  return thread;
}

static inline char * get_stack_area(stack_ptr_t stack_size)
{
  extern char userstack;
  static char * thread_stack = &userstack;

  memset(thread_stack, 0, stack_size);
  thread_stack += stack_size;
  
  return thread_stack;
}

/// stack push
#define push(stack,value) ({ *(--(stack)) = (value); EXIT_SUCCESS; })

/// stack pop
#define pop(stack) (*((stack)++))


/// system call
static kz_thread_id_t thread_run(String name, kz_func_t func, int priority, int argc, char * argv[], stack_ptr_t stack_size)
{
  kz_thread * thread = NULL;
  uint32 * sp = NULL;
  
  // get vacant TCB and clear it
  thread = create_TCB();
  if (thread == NULL) {
    // did not find
    return -1;
  }

  // init thread parameters
  strcpy(thread->name, name);
  thread->next = NULL;
  thread->priority = priority;
  thread->flag.array = 0;
  thread->init.func = func;
  thread->init.argc = argc;
  thread->init.argv = argv;

  // get stack area
  thread->stack = get_stack_area(stack_size);
  
  // initialize stack area
  sp = (uint32 *)thread->stack;
  (void)push(sp, (uint32)thread_end);

  // set program counter
  // in case of 0 priority, this thread isn't interrupted
  //  | CCR | PC | => | 0xc0 | &thread_init |
  // 31    23    0   31     23              0
  (void)push(sp, (uint32)thread_init | ((uint32)( (priority>0) ? 0 : 0xc0) << 24) );
  (void)push(sp, 0); // ER6
  (void)push(sp, 0); // ER5
  (void)push(sp, 0); // ER4
  (void)push(sp, 0); // ER3
  (void)push(sp, 0); // ER2
  (void)push(sp, 0); // ER1

  // arguments of thread_init (ER0)
  (void)push(sp, (uint32)thread);

  // set context of thread
  thread->context.sp = (uint32)sp;

  // put thread to ready queue since that thread use system call
  put(current);
  
  // put new thread to ready queue
  put(thread);

  return (kz_thread_id_t)thread;
}

/// exit the thread
/// @param thread thread that exit now
static int thread_exit(kz_thread * thread)
{
  puts(thread->name); puts(" EXIT.\n");

  /// clear TCB
  memset(thread, 0, sizeof(*thread));
  
  return EXIT_SUCCESS;
}
  
/**
   give anothrer thread execution

   @return EXIT_SUCCESS
*/
static int thread_wait(void)
{
  put(current);

  return EXIT_SUCCESS;
}

/**
   sleep thread
   
   @return EXIT_SUCCESS
*/
static int thread_sleep(void)
{
  return EXIT_SUCCESS;
}

/**
   wakeup another thread

   @param id thread id of wake up thread
   @return EXIT_SUCCESS
*/
static int thread_wakeup(kz_thread_id_t id)
{
  put(current); // current thread is rescheduled
  
  put((kz_thread *)id);

  return EXIT_SUCCESS;
}

/**
   get thread id of current thread

   @param thread thread want to know its id
   @return thread id
*/
static kz_thread_id_t thread_getid(kz_thread * thread)
{
  put(thread);

  return (kz_thread_id_t)thread;
}

/**
   change current thread priority

   @param priority new priority
   @return old priority
*/
static int thread_chpri(int priority)
{
  int old = current->priority;

  if( 0 <= priority && priority < PRIORITY_NUM ){
    current->priority = priority;
  }
  put(current);

  return old;
}

/**
   allocate memory area

   @param size size of memory area
   @ret head address of allocated area
 */
static void * thread_kmalloc(unsigned int size)
{
  put(current);
  
  return kzmem_alloc(size);
}

/**
   free allocated memory area

   @param head address of area
   @ret just return EXIT_SUCCESS
 */
static int thread_kmfree(void * p)
{
  int ret;
  ret = kzmem_free(p);

  put(current);

  return ret;
}

static int set_reciever(kz_msgbox * msg_box, kz_thread * thread)
{
  if( msg_box->reciever != NULL ) {
    kz_sysdown("too many reciever is waiting\n");
  }
  else {
    msg_box->reciever = thread;
  }
  
  return EXIT_SUCCESS;
}

static Bool wait_msg_of(const kz_msgbox * msg_box)
{
  return (Bool)(msg_box->head);
}

/// !!! side effect !!! msg_box loss its top item
static kz_msgbuf * deque(kz_msgbox * msg_box)
{
  kz_msgbuf * head = msg_box->head;

  msg_box->head = msg_box->head->next;
  if( msg_box->head == NULL ) {
    msg_box->tail = NULL;
  }

  head->next = NULL;

  return head;
}

static kz_msgbox * enque(kz_msgbox * msg_box, kz_msgbuf * msg_buf)
{
  if( msg_box->tail == NULL ) {
    msg_box->head = msg_buf;
  }
  else {
    msg_box->head->next = msg_buf;
  }
  msg_box->tail = msg_buf;

  return msg_box;
}

/// @return sender id
static kz_thread_id_t recvmsg(kz_msgbox * msg_box)
{
  kz_msgbuf * msg_buf = deque(msg_box);
  kz_syscall_param_t * param = msg_box->reciever->syscall.param;

  if( param->recv.size_ptr != NULL ) {
    *(param->recv.size_ptr) = msg_buf->param.size;
  }
  if( param->recv.data_ptr_ptr != NULL ) {
    *(param->recv.data_ptr_ptr) = msg_buf->param.data_ptr;
  }
  param->recv.ret = (kz_thread_id_t)msg_buf->sender;

  msg_box->reciever = NULL;
  kzmem_free(msg_buf);

  return param->recv.ret;
}

static void sendmsg(kz_msgbox * msg_box, kz_thread * thread, unsigned int size, char * data)
{
  kz_msgbuf * msg_buf;
  msg_buf = (kz_msgbuf *)kzmem_alloc(sizeof(kz_msgbuf));
  if( msg_buf == NULL ) {
    kz_sysdown("no buffer for send\n");
  }

  msg_buf->next = NULL;
  msg_buf->sender = thread;
  msg_buf->param.size = size;
  msg_buf->param.data_ptr = data;

  msg_box = enque(msg_box, msg_buf);
  
  return;
}

/**
   プロセス間通信(送信)
 */
static unsigned int thread_send(kz_msgbox_id_t id, unsigned int size, char * data)
{
  kz_msgbox * msg_box = &(msg_boxes[id]);

  put(current);
  sendmsg(msg_box, current, size, data);

  if(msg_box->reciever != NULL) {
    put(msg_box->reciever);
    recvmsg(msg_box);
  }

  return size;
}

/**
   プロセス間通信(受信)
 */
static kz_thread_id_t thread_recv(kz_msgbox_id_t id, unsigned int * size, char ** data)
{
  kz_msgbox * msg_box = &(msg_boxes[id]);
  kz_thread_id_t result;

  set_reciever(msg_box, current);

  if ( wait_msg_of(msg_box) ) {
    result = recvmsg(msg_box);
    put(current);
  }
  else {
    // change to sleep state to wait for message
    result = (kz_thread_id_t)NULL;
  }

  return result;
}

/// set interrupt handler
static int thread_setintr(softvec_type_t type, kz_handler_t handler)
{
  if( type < 0 || SOFTVEC_TYPE_NUM <= type ) {
    return -1;
  } 

  softvec_setintr(type, thread_intr);

  handlers[type] = handler;
  put(current);

  return EXIT_SUCCESS;
}

static void call_functions(kz_syscall_type_t type, kz_syscall_param_t * param)
{
  switch(type){
  case KZ_SYSCALL_TYPE_RUN:
    param->run.ret = thread_run(param->run.name, param->run.func, param->run.priority, param->run.argc, param->run.argv, param->run.stack_size);
    break;
  case KZ_SYSCALL_TYPE_EXIT:
    // don't read return value because TCB is erased
    (void)thread_exit(current);
    break;
  case KZ_SYSCALL_TYPE_WAIT:
    param->wait.ret = thread_wait();
    break;
  case KZ_SYSCALL_TYPE_SLEEP:
    param->sleep.ret = thread_sleep();
    break;
  case KZ_SYSCALL_TYPE_WAKEUP:
    param->wakeup.ret = thread_wakeup(param->wakeup.id);
    break;
  case KZ_SYSCALL_TYPE_GETID:
    param->getid.ret = thread_getid(current);
    break;
  case KZ_SYSCALL_TYPE_CHPRI:
    param->chpri.ret = thread_chpri(param->chpri.priority);
    break;
  case KZ_SYSCALL_TYPE_KMALLOC:
    param->kmalloc.ret = thread_kmalloc(param->kmalloc.size);
    break;
  case KZ_SYSCALL_TYPE_KMFREE:
    param->kmfree.ret = thread_kmfree(param->kmfree.p);
    break;
  case KZ_SYSCALL_TYPE_SEND:
    param->send.ret = thread_send(param->send.id, param->send.size, param->send.data_ptr);
    break;
  case KZ_SYSCALL_TYPE_RECV:
    param->recv.ret = thread_recv(param->recv.id, param->recv.size_ptr, param->recv.data_ptr_ptr);
    break;
  case KZ_SYSCALL_TYPE_SETINTR:
    param->setintr.ret = thread_setintr(param->setintr.type, param->setintr.handler);
  default:
    break;
  }
  
  return;
}

/// システムコールの処理
///
/// システムコールを呼び出したスレッドを一旦キューから外す
static void syscall_proc(kz_syscall_type_t type, kz_syscall_param_t * param)
{
  (void)get(current->priority);
  
  call_functions(type, param);

  return;
}

/// サービスコールの処理
/// ただの関数呼び出し
static void srvcall_proc(kz_syscall_type_t type, kz_syscall_param_t * param)
{
  current = NULL;
  call_functions(type, param);
  
  return;
}

/// スケジューリング
///
/// 次に実行するスレッドを選択する。
/// 選択されるスレッドは最も優先度が高いスレッドを格納するキューの先頭にあるスレッド。
static kz_thread * schedule(void)
{
  kz_thread * next_thread = NULL;
  for(int i = 0; i < PRIORITY_NUM && next_thread == NULL; i++){
      next_thread = readyque[i].head;
  }
  
  if(next_thread == NULL){
    // didn't find next thread
    kz_sysdown("no thread to be scheduled\n");
  }

  return next_thread;
}

static void syscall_intr(void)
{
  syscall_proc(current->syscall.type, current->syscall.param);

  return;
}

static void softerr_intr(void)
{
  puts(current->name); puts(" Down.\n");
  (void)get(current->priority);
  thread_exit(current);

  return;
}  

static void thread_intr(softvec_type_t type, stack_ptr_t sp)
{
  // save context of current thread
  current->context.sp = sp;

  if(handlers[type] != NULL){
    handlers[type]();
  }

  current = schedule();

  dispatch(&current->context);
  // no return 
}

/**
   OS開始
 */
void kz_start(kz_func_t func, String name, int priority, uint16 stack_size, int argc, char * argv[])
{
  // initialize dynamic memory area
  kzmem_init();
  
  // initialization
  current = NULL;
  memset(readyque, 0, sizeof(readyque));
  memset(threads, 0, sizeof(threads));
  memset(handlers, 0, sizeof(handlers));
  memset(msg_boxes, 0, sizeof(msg_boxes));

  // register interrupt handler
  thread_setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
  thread_setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);
  
  current = (kz_thread *)thread_run(name, func, priority, argc, argv, stack_size);
  
  dispatch(&current->context);

  //no return
}

void kz_sysdown(cString message)
{
  puts(message);
  puts("system error!\n");
  
  // infinity loop
  while(1)
    ;
}

void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t * param)
{
  current->syscall.type = type;
  current->syscall.param = param;
  
  asm volatile ("trapa #0"); // trap interrupt ( go to xxx_intr that set by kz_setintr() )
  
  //no return;
}

void kz_srvcall(kz_syscall_type_t type, kz_syscall_param_t * param)
{
  srvcall_proc(type, param);
  return;
}
