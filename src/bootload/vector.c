/**
   @file vector.c
   割り込み処理関数の定義
 */

#include "../misc/defines.h"

extern void start(void); ///< 実行開始時の関数 @see startup.s
extern void intr_softerr(void); ///< ソフトウェア エラー @see startup.s
extern void intr_syscall(void); ///< システム コール @see startup.s
extern void intr_serintr(void); ///< シリアル割り込み @see startup.s

/**
   割り込みベクタの設定。
   
   リンカスクリプトの起動により、先頭番地に配置される。
 */
void (*vectors[])(void) = {
  start, NULL, NULL, NULL, NULL, NULL, NULL, NULL,//  0 -  7
  intr_syscall, intr_softerr, intr_softerr, intr_softerr, // 8 - 11
  NULL, NULL, NULL, NULL, // 12 - 15
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 16 - 23
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 24 - 31
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 32 - 39
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 40 - 47
  NULL, NULL, NULL, NULL, // 48 - 51
  intr_serintr, intr_serintr, intr_serintr, intr_serintr, // 52 - 55
  intr_serintr, intr_serintr, intr_serintr, intr_serintr, // 56 - 59
  intr_serintr, intr_serintr, intr_serintr, intr_serintr, // 60 - 63
};

