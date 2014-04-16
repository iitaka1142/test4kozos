#ifndef _KOZOS_SYSCALL_H_INCLUDED_
#define _KOZOS_SYSCALL_H_INCLUDED_

#include "../misc/defines.h"

/// システムコール番号の定義
typedef enum {
  KZ_SYSCALL_TYPE_RUN, ///< run thread
  KZ_SYSCALL_TYPE_EXIT, ///< exit thread
  KZ_SYSCALL_TYPE_WAIT, ///< wait thread
  KZ_SYSCALL_TYPE_SLEEP, ///< sleep thread
  KZ_SYSCALL_TYPE_WAKEUP, ///< thread wake up
  KZ_SYSCALL_TYPE_GETID, ///< get thread id
  KZ_SYSCALL_TYPE_CHPRI ///< change thread priority
} kz_syscall_type_t;

typedef uint32 kz_thread_id_t;
typedef int (* kz_func_t)(int argc, char * argv[]);
typedef void (* kz_handler_t)(void);

/// システムコール時のパラメータ格納領域
typedef union {
  /// kz_runのためのパラメータ
  struct {
    kz_func_t func; ///< 関数 
    String name; ///< thread name
    int priority; ///< priority
    int stack_size; ///< stack size
    int argc; ///< count of arguments
    char * *argv; ///< arry of arguments
    kz_thread_id_t ret; ///< return value of arguments
  } run;
    
  /// kz_exitのためのパラメータ
  struct {
    int dummy[0]; ///< dummy data;
  } exit;
  
  /// kz_waitのためのパラメータ　
  struct {
    int ret;
  } wait;
  
  /// kz_sleepのためのパラメータ　
  struct {
    int ret;
  } sleep;
  
  /// kz_wakeupのためのパラメータ　
  struct {
    kz_thread_id_t id;
    int ret;
  } wakeup;
  
  /// kz_getidのためのパラメータ　
  struct {
    kz_thread_id_t ret;
  } getid;
  
  /// kz_chpriのためのパラメータ　
  struct {
    int priority;
    int ret;
  } chpri;

} kz_syscall_param_t;

#endif
