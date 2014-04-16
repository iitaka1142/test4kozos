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

