#include "../misc/misra2012deviation.h"
#include "../misc/defines.h"
#include "../misc/intr.h"
#include "../misc/interrupt.h"
#include "../misc/lib.h"
#include "syscall.h"
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

/// ready queue of thread
static struct {
  kz_thread * head; ///< head item of queue
  kz_thread * tail; ///< last item of queue
} readyque[PRIORITY_NUM];


static kz_thread * current; ///< current thread
static kz_thread threads[THREAD_NUM]; ///< Task Control Block(TCB)
static kz_handler_t handlers[SOFTVEC_TYPE_NUM]; ///< interrupt handler

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
static inline kz_thread * get( int priority )
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
static inline int put( kz_thread * thread )
{
  if(thread == NULL) {
    return -1;
  }
  else if(thread->flag.bit.ready == true) {
    // if thread has already been in ready state
  }
  else if(readyque[thread->priority].head == NULL) {
    // queue is empty
    readyque[thread->priority].head = thread;
  }
  else {
    // append given thread to ready queue
    readyque[thread->priority].tail->next = thread;
  }

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
  thread->priority = 0;
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

/// set interrupt handler
static int setintr(softvec_type_t type, kz_handler_t handler)
{
  if( type < 0 || SOFTVEC_TYPE_NUM <= type ) {
    return -1;
  } 

  softvec_setintr(type, thread_intr);

  handlers[type] = handler;

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
  default:
    break;
  }
  
  return;
}

static void syscall_proc(kz_syscall_type_t type, kz_syscall_param_t * param)
{
  (void)get(current->priority);
  
  call_functions(type, param);

  return;
}

static void schedule(void)
{
  kz_thread * next_thread = NULL;
  for(int i = 0; i < PRIORITY_NUM || next_thread == NULL; i++){
      next_thread = readyque[i].head;
  }
  
  if(next_thread == NULL){
    // didn't find next thread
    kz_sysdown();
  }
  else{
    current = next_thread;
  }

  return;
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

  schedule();
  
  dispatch(&current->context);
  // no return 
}

void kz_start(kz_func_t func, String name, int priority, uint16 stack_size, int argc, char * argv[])
{
  // initialization
  current = NULL;
  memset(readyque, 0, sizeof(readyque));
  memset(threads, 0, sizeof(threads));
  memset(handlers, 0, sizeof(handlers));

  // register interrupt handler
  setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
  setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

  current = (kz_thread *)thread_run(name, func, priority, argc, argv, stack_size);
  
  dispatch(&current->context);
  
  //no return
}

void kz_sysdown(void)
{
  puts("system error!\n");
  
  // infinity loop
  while(1)
    ;
}

void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t * param)
{
  current->syscall.type = type;
  current->syscall.param = param;
  
  asm volatile ("trapa #0"); // trap interrupt
  
  //no return;
}

