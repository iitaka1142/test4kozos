#include "../misc/defines.h"
#include "kozos.h"
#include "syscall.h"

/// run thread
kz_thread_id_t kz_run(kz_func_t func, String name, int priority, uint16 stack_size, int argc, char * argv[])
{
  kz_syscall_param_t param;

  param.run.func = func;
  param.run.name = name;
  param.run.priority = priority;
  param.run.stack_size = stack_size;
  param.run.argc = argc;
  param.run.argv = argv;

  kz_syscall(KZ_SYSCALL_TYPE_RUN, &param);

  return param.run.ret;
}

/// exit thread
void kz_exit(void)
{
  kz_syscall(KZ_SYSCALL_TYPE_EXIT, NULL);
}

/// wait this thread
int kz_wait(void)
{
  kz_syscall_param_t param;
  kz_syscall(KZ_SYSCALL_TYPE_WAIT, &param);
  
  return param.wait.ret;
}

/// sleep this thread
int kz_sleep(void)
{
  kz_syscall_param_t param;
  kz_syscall(KZ_SYSCALL_TYPE_SLEEP, &param);

  return param.sleep.ret;
}

/// wake up 1 thread
int kz_wakeup(kz_thread_id_t id)
{
  kz_syscall_param_t param;
  param.wakeup.id = id;

  kz_syscall(KZ_SYSCALL_TYPE_WAKEUP, &param);

  return param.wakeup.ret;
}

/// get id of thread
kz_thread_id_t kz_getid(void)
{
  kz_syscall_param_t param;
  kz_syscall(KZ_SYSCALL_TYPE_GETID, &param);
  
  return param.getid.ret;
}

/// change priority of a thread
int kz_chpri(int priority)
{
  kz_syscall_param_t param;

  param.chpri.priority = priority;
  kz_syscall(KZ_SYSCALL_TYPE_CHPRI, &param);
  
  return param.chpri.ret;
}

/// 動的メモリ確保
void * kz_kmalloc(unsigned int size)
{
  kz_syscall_param_t param;

  param.kmalloc.size = size;

  kz_syscall(KZ_SYSCALL_TYPE_KMALLOC, &param);
  
  return param.kmalloc.ret;
}

/// 動的メモリ開放
int kz_kmfree(void *p)
{
  kz_syscall_param_t param;

  param.kmfree.p = p;

  kz_syscall(KZ_SYSCALL_TYPE_KMFREE, &param);
  
  return param.kmfree.ret;
}

/// メッセージ送信
int kz_send(kz_msgbox_id_t msg_id, unsigned int size, char * data)
{
  kz_syscall_param_t param;

  param.send.id = msg_id;
  param.send.size = size;
  param.send.data_ptr = data;

  kz_syscall(KZ_SYSCALL_TYPE_SEND, &param);
  
  return param.send.ret;
}

/// メッセージ受信
/// @return 送信者のid
kz_thread_id_t kz_recv(kz_msgbox_id_t msg_id, unsigned int * size, char ** data)
{
  kz_syscall_param_t param;

  param.recv.id = msg_id;
  param.recv.size_ptr = size;
  param.recv.data_ptr_ptr = data;

  kz_syscall(KZ_SYSCALL_TYPE_RECV, &param);
  
  return param.recv.ret;
}

/// 割り込みハンドラ登録
int kz_setintr(unsigned int type, kz_handler_t handler)
{
  kz_syscall_param_t param;
  
  param.setintr.type = type;
  param.setintr.handler = handler;

  kz_syscall(KZ_SYSCALL_TYPE_SETINTR, &param);

  return param.setintr.ret;
}

int kx_wakeup(kz_thread_id_t id)
{
  kz_syscall_param_t param;
    
  param.wakeup.id = id;

  kz_srvcall(KZ_SYSCALL_TYPE_WAKEUP, &param);

  return param.wakeup.ret;
}

void * kx_kmalloc(unsigned int size)
{
  kz_syscall_param_t param;

  param.kmalloc.size = size;

  kz_srvcall(KZ_SYSCALL_TYPE_KMALLOC, &param);
  
  return param.kmalloc.ret;
}

int kx_kmfree(void *p)
{
  kz_syscall_param_t param;

  param.kmfree.p = p;

  kz_srvcall(KZ_SYSCALL_TYPE_KMFREE, &param);
  
  return param.kmfree.ret;
}

int kx_send(kz_msgbox_id_t msg_id, unsigned int size, char * data)
{
  kz_syscall_param_t param;

  param.send.id = msg_id;
  param.send.size = size;
  param.send.data_ptr = data;

  kz_srvcall(KZ_SYSCALL_TYPE_SEND, &param);
  
  return param.send.ret;
}

kz_thread_id_t kx_recv(kz_msgbox_id_t msg_id, unsigned int * size, char ** data)
{
  kz_syscall_param_t param;

  param.recv.id = msg_id;
  param.recv.size_ptr = size;
  param.recv.data_ptr_ptr = data;

  kz_srvcall(KZ_SYSCALL_TYPE_RECV, &param);
  
  return param.recv.ret;
}
