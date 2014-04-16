/**
   @file main.c

   エントリポイント
*/

#include "../misc/defines.h"
#include "../misc/interrupt.h"
#include "../misc/serial.h"
#include "../misc/lib.h"
#include "xmodem.h"
#include "elf.h"

int global_data = 0x10;
int global_bss;
//static int static_data = 0x20;
//static int static_bss;

/// 初期化関数
static int init(void);
/**<
   @return EXIT_SUCCESS

   メモリ領域とシリアルポートの初期化を行う
*/

#if 0
/// 変数値出力関数
static void printval(void);
#endif

/// メモリの16進ダンプ出力
static int dump(char * buf, long size);
/**<
   @param[in] buf 表示するデータの格納バッファ
   @param size データサイズ
   
   @retval EXIT_SUCCESS 正常終了
   @retval -1 エラー
 */

/** ウェイト関数 */
static void wait();

/* ウェイト関数 */
static void wait()
{
  volatile long i;
  for(i = 0; i < 300000; i++){
    ;
  }
}

/** 初期化関数 */
static int init(void)
{
  /* 以下はリンカスクリプトで定義されたシンボル */
  extern int erodata, data_start, edata, bss_start, ebss;

  /*
   * データ領域とBSS領域を初期化する
   * この処理以降、グローバル変数が利用できる
   */
  memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
  memset(&bss_start, 0, (long)&ebss - (long)&bss_start);

  /*
   * ソフトウェア割り込みベクタの初期化
   */
  softvec_init();

  /*
   * シリアルポートの初期化
   */
  serial_init(SERIAL_DEFAULT_DEVICE);

  return EXIT_SUCCESS;
}

/* メモリの16進ダンプ出力 */
static int dump(char * buf, long size)
{
  long i;

  if (size < 0) {
    puts("no data.\n");
    return -1;
  }

  for (i=0; i < size ; i++) {
    putxval(buf[i], 2);
    if ((i & 0xf) == 0xf) {
      puts("\n");
    } else {
      if ((i & 0xf) == 0x7){
	puts(" ");
      }
      puts(" ");
    }
  }
  puts(" ");

  return EXIT_SUCCESS;
}

#if 0
static void printval(void)
{
  puts("global_data = "); putxval(global_data, 0); puts("\n");
  puts("global_bss = "); putxval(global_bss, 0); puts("\n");
  puts("static_data = "); putxval(static_data, 0); puts("\n");
  puts("static_bss = "); putxval(static_bss, 0); puts("\n");
}
#endif

int main(void)
{
  static char buf[16];
  static long size = -1;
  static String loadbuf = NULL;
  char * entry_point;
  void (*f)(void);
	
  extern int buffer_start;
	
  INTR_DISABLE;
  init();

  /* 4th step */
  puts("kzload (kozos boot loader) started. \n");

  while (1) {
    puts("kzload> "); /* プロンプト表示 */
    gets(buf);

    if (!strcmp(buf, "load")) {
      /* ファイル転送 */
      loadbuf = (char *)(&buffer_start);
      size = xmodem_recv(loadbuf);
      wait();
      if (size < 0) {
	puts("\nXMODEM receive error!\n");
      } else {
	puts("\nXMODEM receive succeed.\n");
      }
    } else if (!strcmp(buf, "dump")) {
      /* バッファの16進ダンプ */
      puts("size : ");
      putxval(size, 0);
      puts("\n");

      dump(loadbuf, size);
    } else if (!strcmp(buf, "run")) {
      /* ELF形式ファイルをメモリ上に展開 */
      entry_point = elf_load(loadbuf);
      if(!entry_point) {
	puts("run error!");
      } else {
	puts("starting from entry_point ");
	putxval((uint32)entry_point, 0);
	puts("\n");

	f = (void (*)(void))entry_point;
	f(); /* ロードしたプログラムに処理を渡す */
      }
    } else if (!strcmp(buf, "exit")) {
      break;
    } else {
      puts("unknown.\n");
    }
  }

  return EXIT_SUCCESS;
}

