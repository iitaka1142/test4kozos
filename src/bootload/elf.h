#ifndef _ELF_H_INCLUDED_
#define _ELF_H_INCLUDED_

/** \file elf.h
 * ELF形式の解析
 *
 * 他ファイルへの公開関数
 */

/** ELF形式の解析 */
char * elf_load(const char * buf);
/**<
 * \param[in] buf 解析するメモリ領域
 * 
 * \return 解析成功の場合、エントリポイントを返す
 * \retval NULL 解析失敗
 *
 * bufのサイズは、ELF形式の中にあるので、
 * 引数にサイズはいらない。
 */

#endif
