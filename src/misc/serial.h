#ifndef _SERIAL_H_INCLUDED_
#define _SERIAL_H_INCLUDED_
/** @file serial.h
 * シリアル通信
 *
 * 公開関数
 */

/** 有効無効 */
typedef enum {
  DISABLE,
  ENABLE
} port_status;

/** シリアル通信ポート */
typedef enum {
  SERIAL_SCI_1 = 0xffffb0,
  SERIAL_SCI_2 = 0xffffb8,
  SERIAL_SCI_3 = 0xffffc0,
  SERIAL_SCI_NUM = 3
} h8_3069f_sci;

/** シリアル通信ポート構造体 */
typedef struct _h8_3069f_sci * serial_port;

#define SERIAL_DEFAULT_DEVICE (serial_port)SERIAL_SCI_2 /**< シリアル通信のポート番号 */

/** デバイス初期化 */
int serial_init( serial_port port );
/**<
 * @return EXIT_SUCCESS
 */

/** @breif 送信可能か? */
port_status serial_is_send_enable( serial_port port );
/**<
 * @retval 1 送信可能
 * @retval 0 送信不可
 */

/**  1文字送信 */
int serial_send_byte( serial_port port, char c );
/**<
 * @param c 送信文字
 * @param port 通信ポートアドレス
 *
 * @return EXIT_SUCCESS
 */

/** 受信可能か? */
port_status serial_is_recv_enable(serial_port port);
/**<
 * @retval 1 受信可能
 * @retval 0 受信不可
 */

/** 1文字受信 */
char serial_recv_byte(serial_port port);
/**<
 * @return 受信した文字
 */

/** (割り込み)シリアル通信受信有効チェック */ 
port_status serial_intr_is_send_enable(serial_port port);

/** (割り込み)シリアル通信受信有効化 */ 
void serial_intr_send_enable(serial_port port);

/** (割り込み)シリアル通信受信無効化 */ 
void serial_intr_send_disable(serial_port port);

/** (割り込み)シリアル通信送信有効チェック */ 
port_status serial_intr_is_recv_enable(serial_port port);

/** (割り込み)シリアル通信送信有効化 */ 
void serial_intr_recv_enable(serial_port port);

/** (割り込み)シリアル通信送信無効化 */ 
void serial_intr_recv_disable(serial_port port);

#endif
