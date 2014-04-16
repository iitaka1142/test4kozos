#ifndef _XMODEM_H_INCLUDE_
#define _XMODEM_H_INCLUDE_

/** \file xmodem.h
 * シリアル通信用XMODEMプロトコル
 *
 * 公開関数
 */

/** XMODEMによる、ファイルの受信 */
long xmodem_recv(char * buf);
/**<
 * \param[out] buf 受信データ格納バッファ
 * \return ファイルサイズ
 */

#endif
