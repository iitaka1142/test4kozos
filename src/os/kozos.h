#ifndef _KOZOS_H_INCLUDED_
#define _KOZOS_H_INCLUDED_

#include "../misc/defines.h"
#include "syscall.h"


// システムコール

/// スレッドの実行
kz_thread_id_t kz_run(kz_func_t func, String name, int priority, uint16 stack_size, int argc, char * argv[]);

/// スレッド強制終了
void kz_exit(void);

/// スレッド待機
int kz_wait(void);

/// スレッドスリープ
int kz_sleep(void);

/// スレッド再起動
int kz_wakeup(kz_thread_id_t id);

/// スレッドID取得
kz_thread_id_t kz_getid(void);

/// スレッド優先度変更
int kz_chpri(int priority);

/// メモリ獲得
void * kz_kmalloc(unsigned int size);

/// メモリ開放
int kz_kmfree(void *p);

/// メッセージ送信
int kz_send(kz_msgbox_id_t msg_id, unsigned int size, char * data);

/// メッセージ受信
kz_thread_id_t kz_recv(kz_msgbox_id_t msg_id, unsigned int * size, char ** data);

/// 割り込みハンドラ登録
int kz_setintr(unsigned int type, kz_handler_t handler);

// サービスコール

int kx_wakeup(kz_thread_id_t id);
void * kx_kmalloc(unsigned int size);
int kx_kmfree(void *p);
int kx_send(kz_msgbox_id_t msg_id, unsigned int size, char * data);
kz_thread_id_t kx_recv(kz_msgbox_id_t msg_id, unsigned int * size, char ** data);

// ライブラリ関数

/// start tasking(os)
void kz_start(kz_func_t func, String name, int priority, uint16 stack_size, int argc, char * argv[]) __attribute__((noreturn));

/// shutdown tasking
void kz_sysdown(cString message) __attribute__((noreturn));

/// system call from thread
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t * param);

/// service call from interrupt
void kz_srvcall(kz_syscall_type_t type, kz_syscall_param_t * param);

/// ユーザースレッド
//  テストプログラムをここに書くのは、ちょっと疑問
int consdrv_main(int argc, char * argv[]);
int command_main(int argc, char * argv[]);

#endif 
