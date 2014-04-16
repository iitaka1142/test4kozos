#ifndef _INTERRUPT_H_INCLUDED_
#define _INTERRUPT_H_INCLUDED_

#include "defines.h"

/* 以下は、リンカ・スクリプトで定義されたシンボル */
extern char softvec;

#define SOFTVEC_ADDR (&softvec)

/** ソフトウェア割り込み種類の定義 */
typedef short softvec_type_t;
/**<
   @see inter.h
 */

/** スタックポインタ型 */
typedef uint32 stack_ptr_t;

/**
   割り込みハンドラの型
 */
typedef void (* softvec_handler_t)(softvec_type_t type, stack_ptr_t sp);

#define SOFTVECS ((softvec_handler_t *) SOFTVEC_ADDR)

#define INTR_ENABLE  asm volatile ("andc.b #0x3f,ccr")
#define INTR_DISABLE asm volatile ("orc.b #0xc0,ccr")

/** ソフトウェア割り込みの初期化 */
int softvec_init(void);

/** ソフトウェア割り込みベクタの設定 */
int softvec_setintr(softvec_type_t type, softvec_handler_t handler);

/** 割り込みハンドラ */
void interrupt(softvec_type_t type, stack_ptr_t sp);
/**<
   IRQ発生によってキックされ、引数として受け取ったhandlerを実行する
 */

#endif
