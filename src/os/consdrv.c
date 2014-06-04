#include "../misc/defines.h"
#include "../misc/interrupt.h"
#include "../misc/serial.h"
#include "../misc/intr.h"
#include "../misc/lib.h"
#include "kozos.h"
#include "consdrv.h"

#define CONSDRV_DEVICE_NUM 1
#define CONS_BUFFER_SIZE 24

struct consreg {
  serial_port port_index; // 2byte
  // pud 2byte
  kz_thread_id_t sender; // 4byte

  char * send_buf; // 4byte
  unsigned int send_len; // 2byte
  // pud 2byte
  
  char * recv_buf; // 4 byte
  unsigned int recv_len; // 2byte
} __attribute__((aligned(32))) consreg[CONSDRV_DEVICE_NUM];

static int consdrv_init()
{
  memset(consreg, 0, sizeof(consreg));

  return EXIT_SUCCESS;
}
/// 1文字送信
static int send_char(struct consreg * cons)
{
  serial_send_byte(cons->port_index, cons->send_buf[0]);
  cons->send_len--;

  // バッファの内容を1文字ずつ詰める
  for( int i = 0; i < cons->send_len; i++ ) {
    cons->send_buf[i] = cons->send_buf[i + 1];
  }

  return EXIT_SUCCESS;
}

static inline unsigned int get_index(char * buf)
{
  return (unsigned int)(buf[0] - '0');
}

static int send_string(struct consreg * cons, char * str, unsigned int len)
{
  // 文字列を送信バッファにコピー
  // 改行コードを変更する必要があるため、
  // str中に複数の改行が含まれている場合を考えると、
  // strcpyは使用できない
  for (int i = 0; i < len; i++) {
    if ( str[i] == '\n' ) { // "\n" -> "\r\n"とするため、'\r'を追加
      cons->send_buf[cons->send_len++] = '\r';
    }
    else {
      // 改行コード以外は変更なし
    }
    cons->send_buf[cons->send_len++] = str[i];
  }

  if ((cons->send_len > 0) && (serial_intr_is_send_enable(cons->port_index) == DISABLE)) {
    // 送信割り込み無効の場合:
    send_char(cons); // 最初の1文字を送信(残りは送信有効割り込みのハンドラで送信)
    serial_intr_send_enable(cons->port_index); //   送信を開始する
  }
  else {
    // 送信割り込み有効の場合:
    //   なにもしない
    //   (送信が開始されており、送信割り込みのハンドラがデータを送信するため)
  }

  return EXIT_SUCCESS;
}

/// @require 送信割り込みが有効 
static int consdrv_send_intrproc(struct consreg * cons)
{ 
  if ( (cons->sender == (kz_thread_id_t)NULL) || (cons->send_len == 0) ) {
    // だれも送信を依頼していない、または送信データがない
    serial_intr_send_disable(cons->port_index);
  } else {
    send_char(cons);
  }

  return EXIT_SUCCESS;
}

/// @require 受信割り込みが有効
static int consdrv_recv_intrproc(struct consreg * cons)
{
  char c = serial_recv_byte(cons->port_index);

  // 改行コード変換 \r -> \n
  if ( c == '\r' ) {
    c = '\n';
  }

  send_string(cons, &c, 1); // echo back

  if( cons->sender == (kz_thread_id_t)NULL ) {
    // don't do anything
  }
  else if( c != '\n' ) {
    // not return code
    // store recieved charactor to buffer
    cons->recv_buf[cons->recv_len++] = c;
  } else {
    // send message to command analysis
    char * buf = kx_kmalloc(CONS_BUFFER_SIZE);
    memcpy(buf, cons->recv_buf, cons->recv_len);

    kx_send(MSGBOX_ID_CONSINPUT, cons->recv_len, buf);
    cons->recv_len = 0;
  }
  
  return EXIT_SUCCESS;
}

static void consdrv_intr(void)
{
  struct consreg * cons;

  for( int i = 0; i < CONSDRV_DEVICE_NUM; i++ ) {
    cons = &(consreg[i]);
    if ( cons->sender != (kz_thread_id_t)NULL ) {
      if ( serial_is_send_enable(cons->port_index) == ENABLE ) {
	(void)consdrv_send_intrproc(cons);
      }
      if ( serial_is_recv_enable(cons->port_index) == ENABLE ) {
	(void)consdrv_recv_intrproc(cons);
      }
    }
  }
    
  return;
}

 static int consdrv_command(struct consreg * cons, kz_thread_id_t id, unsigned int size, char * command)
{
  char com = command[0];
  size--; // 先頭のコマンドを分離したので、commandのサイズを更新する

  switch(com) {
  case CONSDRV_CMD_USE:
    cons->port_index = (serial_port)get_index(&(command[1]));
    cons->sender = id;

    cons->send_buf = kz_kmalloc(CONS_BUFFER_SIZE);
    cons->send_len = 0;
    
    cons->recv_buf = kz_kmalloc(CONS_BUFFER_SIZE);
    cons->recv_len = 0;

    serial_init(cons->port_index);
    serial_intr_recv_enable(cons->port_index);
    break;
  case CONSDRV_CMD_WRITE:
    // コンソールへの文字出力
    // 送信バッファを操作するので、割り込み禁止にする
    INTR_DISABLE;

    send_string(cons, &(command[1]), size);

    INTR_ENABLE;
    break;

  default:
    break;
  }
  
  return EXIT_SUCCESS;
}

int consdrv_main(int argc, char * argv[])
{
  unsigned int size;
  unsigned int index;
  String buf;
  kz_thread_id_t sender;

  (void)consdrv_init(consreg);
  kz_setintr(SOFTVEC_TYPE_SERINTR, consdrv_intr);

  while(True) {
    sender = kz_recv(MSGBOX_ID_CONSOUTPUT, &size, &buf);
    // 送信処理
    index = get_index(buf);
    consdrv_command(&(consreg[index]), sender, (size - 1), (buf + 1));

    kz_kmfree(buf);
  }
  
  return EXIT_SUCCESS;
}
