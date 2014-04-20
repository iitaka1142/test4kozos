#ifndef _DEFINES_H_INCLUDED_
#define _DEFINES_H_INCLUDED_

/**
   @file defines.h
   共通定義
*/

#define NULL ((void *) 0) ///< NULLポインタ
#define EXIT_SUCCESS 0 ///< 正常終了

#define NoReturnFunction __attribute__((noreturn))

typedef unsigned char uint8; ///< 8byteデータ
typedef unsigned short uint16; ///< 16byteデータ
typedef unsigned long uint32; ///< 32byteデータ
typedef char * String; ///< 文字列型
typedef const char *cString; ///< 変更されない文字列型

typedef enum {
  false = 0,
  true,
} Bool;

typedef enum {
  MSGBOX_ID_CONSINPUT = 0,
  MSGBOX_ID_CONSOUTPUT,
  MSGBOX_ID_NUM
} kz_msgbox_id_t;

#endif
