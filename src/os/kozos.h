#ifndef _KOZOS_H_INCLUDED_
#define _KOZOS_H_INCLUDED_

#include "../misc/defines.h"
#include "syscall.h"

///  システムコール
kz_thread_id_t kz_run(kz_func_t func, String name, int priority, uint16 stack_size, int argc, char * argv[]);

/// スレッド強制終了
void kz_exit(void);


// ライブラリ関数

/// start tasking(os)
void kz_start(kz_func_t func, String name, int priority, uint16 stack_size, int argc, char * argv[]) __attribute__((noreturn));

/// shutdown tasking
void kz_sysdown(void) __attribute__((noreturn));

/// syscall from thread
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t * param);

/// ユーザースレッド
//  テストプログラムをここに書くのは、ちょっと疑問
int test08_1_main(int argc, char * argv[]);

#endif 
