#ifndef _SERIAL_H_INCLUDED_
#define _SERIAL_H_INCLUDED_
/** @file serial.h
 * シリアル通信
 *
 * 公開関数
 */

/** シリアルポート番号 */
typedef enum {
  SERIAL_SCI_1,
  SERIAL_SCI_2,
  SERIAL_SCI_3,
  SERIAL_SCI_NUM
} serial_port;

#define SERIAL_DEFAULT_DEVICE SERIAL_SCI_2 ///< シリアル通信のポート番号
  
/** 有効無効 */
typedef enum {
  DISABLE,
  ENABLE,
} port_status;

/** デバイス初期化 */
int serial_init( serial_port index );
/**<
 * @return EXIT_SUCCESS
 */

/** @breif 送信可能か? */
port_status serial_is_send_enable( serial_port index );
/**<
 * @retval 1 送信可能
 * @retval 0 送信不可
 */

/**  1文字送信 */
int serial_send_byte( serial_port index, char c );
/**<
 * @param c 送信文字
 *
 * @return EXIT_SUCCESS
 */

/** 受信可能か? */
port_status serial_is_recv_enable(serial_port index);
/**<
 * @retval 1 受信可能
 * @retval 0 受信不可
 */

/** 1文字受信 */
char serial_recv_byte(serial_port index);
/**<
 * @return EXIT_SUCCESS
 */

/** (割り込み)シリアル通信受信有効チェック */ 
port_status serial_intr_is_send_enable(serial_port index);

/** (割り込み)シリアル通信受信有効化 */ 
void serial_intr_send_enable(serial_port index);

/** (割り込み)シリアル通信受信無効化 */ 
void serial_intr_send_disable(serial_port index);

/** (割り込み)シリアル通信送信有効チェック */ 
port_status serial_intr_is_recv_enable(serial_port index);

/** (割り込み)シリアル通信送信有効化 */ 
void serial_intr_recv_enable(serial_port index);

/** (割り込み)シリアル通信送信無効化 */ 
void serial_intr_recv_disable(serial_port index);

#endif
