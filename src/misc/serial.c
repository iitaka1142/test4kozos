/** @file serial.c
 * シリアル通信
 *
 * ソースコード
 */

#include "defines.h"
#include "serial.h"

struct _h8_3069f_sci {
  volatile uint8 smr;
  volatile uint8 brr;
  volatile uint8 scr;
  volatile uint8 tdr;
  volatile uint8 ssr;
  volatile uint8 rdr;
  volatile uint8 scmr;
};

#define H8_3069F_SCI_SMR_CKS_PER1	(0<<0)
#define H8_3069F_SCI_SMR_CKS_PER4	(1<<0)
#define H8_3069F_SCI_SMR_CKS_PER16 	(2<<0)
#define H8_3069F_SCI_SMR_MP 		(1<<2)
#define H8_3069F_SCI_SMR_STOP		(1<<3)
#define H8_3069F_SCI_SMR_OE		(1<<4)
#define H8_3069F_SCI_SMR_PE		(1<<5)
#define H8_3069F_SCI_SMR_CHR		(1<<6)
#define H8_3069F_SCI_SMR_CA		(1<<7)

#define H8_3069F_SCI_SCR_CKE0		(1<<0)
#define H8_3069F_SCI_SCR_CKE1		(1<<1)
#define H8_3069F_SCI_SCR_TEIE		(1<<2)
#define H8_3069F_SCI_SCR_MPIE		(1<<3)
#define H8_3069F_SCI_SCR_RE		(1<<4) /**< 受信有効 */
#define H8_3069F_SCI_SCR_TE		(1<<5) /**< 送信有効 */
#define H8_3069F_SCI_SCR_RIE		(1<<6) /**< 受信割り込み有効 */
#define H8_3069F_SCI_SCR_TIE		(1<<7) /**< 送信割り込み有効 */

#define H8_3069F_SCI_SSR_MPBT		(1<<0)
#define H8_3069F_SCI_SSR_MPB		(1<<1)
#define H8_3069F_SCI_SSR_TEND		(1<<2)
#define H8_3069F_SCI_SSR_PER		(1<<3)
#define H8_3069F_SCI_SSR_FERERS		(1<<4)
#define H8_3069F_SCI_SSR_ORER		(1<<5)
#define H8_3069F_SCI_SSR_RDRF		(1<<6) /**< 受信完了 */
#define H8_3069F_SCI_SSR_TDRE		(1<<7) /**< 送信完了 */

/** デバイスの初期化 */
int serial_init( serial_port port )
{
  volatile serial_port sci = port;

  /* レジスタを変更する前にシリアル通信を不許可にする */
  sci->scr = 0x00;
  
  sci->smr = 0x00;
  /* 20MHzのクロックから9600bpsを生成(25MHzの場合は80にする) */
  sci->brr = 64;
  /* */
  sci->tdr = 0xFF;

  sci->ssr = 0x00;
  
  sci->scmr = 0xF2;

  /* 送受信可能 */
  sci->scr = H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE;


  return EXIT_SUCCESS;
}

/** 送信可能かの判定 */
port_status serial_is_send_enable( serial_port port )
{
  volatile serial_port sci = port;

  return (sci->ssr & H8_3069F_SCI_SSR_TDRE)? ENABLE : DISABLE;
}

/** 1文字送信 */
int serial_send_byte( serial_port port, char c )
{
  volatile serial_port sci = port;

  while( !serial_is_send_enable(port) )
    ;

  sci->tdr = (unsigned char)c;
  sci->ssr &= (uint8)(~H8_3069F_SCI_SSR_TDRE); /* 送信開始 */

  return EXIT_SUCCESS;
}

/** 受信可能かの判定 */
port_status serial_is_recv_enable(serial_port port)
{
  volatile serial_port sci = port;

  return (sci->ssr & H8_3069F_SCI_SSR_RDRF)? ENABLE : DISABLE;
}

/** 1文字受信 */
char serial_recv_byte(serial_port port)
{
  volatile serial_port sci = port;
  unsigned char c;

  /* 受信文字が来るまで待つ */
  while (!serial_is_recv_enable(port)) {
    ;
  }
  c = sci->rdr;
  sci->ssr &= (uint8)(~H8_3069F_SCI_SSR_RDRF);

  return (char)c;
}

/** (割り込み)シリアル通信受信有効チェック */ 
port_status serial_intr_is_send_enable(serial_port port)
{
  volatile serial_port sci = port;
  
  return (sci->scr & H8_3069F_SCI_SCR_TIE) ? ENABLE : DISABLE;
}

/** (割り込み)シリアル通信受信有効化 */ 
void serial_intr_send_enable(serial_port port)
{
  volatile serial_port sci = port;
  
  sci->scr |= H8_3069F_SCI_SCR_TIE;
  
  return;
}
  

/** (割り込み)シリアル通信受信無効化 */ 
void serial_intr_send_disable(serial_port port)
{
  volatile serial_port sci = port;
  
  sci->scr &= (uint8)(~H8_3069F_SCI_SCR_TIE);
  
  return;
}

/** (割り込み)シリアル通信送信有効チェック */ 
port_status serial_intr_is_recv_enable(serial_port port)
{
  volatile serial_port sci = port;
  
  return (sci->scr & H8_3069F_SCI_SCR_RIE) ? ENABLE : DISABLE;
}

/** (割り込み)シリアル通信送信有効化 */ 
void serial_intr_recv_enable(serial_port port)
{
  volatile serial_port sci = port;

  sci->scr |= H8_3069F_SCI_SCR_RIE;
  
  return;
}

/** (割り込み)シリアル通信送信無効化 */ 
void serial_intr_recv_disable(serial_port port)
{
  volatile serial_port sci = port;

  sci->scr &= (uint8)(~H8_3069F_SCI_SCR_RIE);
  
  return;
}

serial_port get_serial_port_from(port_index index)
{
  h8_3069f_sci port; 

  switch(index) {
  case 0:
    port = SERIAL_SCI_0;
    break;
  case 1:
    port = SERIAL_SCI_1;
    break;
  case 2:
    port = SERIAL_SCI_2;
    break;
  default:
    return NULL;
  }
  
  return (serial_port)port;
}

port_index get_port_index_from(serial_port port)
{
  port_index index;
  
  
  if(port == (serial_port)SERIAL_SCI_0) {
    index = 0;
  }
  else if(port == (serial_port)SERIAL_SCI_1) {
    index = 1;
  }
  else if(port == (serial_port)SERIAL_SCI_2) {
    index = 2;
  }
  else {
    index = (port_index)-1;
  }

  return index;
}
