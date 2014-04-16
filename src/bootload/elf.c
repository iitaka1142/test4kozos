/**
   @file elf.c
   ELF形式の解析
  
   ソースコード
 */
#include "../misc/defines.h"
#include "../misc/lib.h"
#include "elf.h"

/**
 * ELFヘッダ構造体
 */
struct elf_header {
	/**
	 * 先頭16byteの識別情報
	 */
	struct {
		uint8 magic[4]; ///< マジックナンバ
		uint8 class; ///< 32/64ビットの区別
		uint8 format; ///< エンディアン情報
		uint8 version; ///< ELFフォーマットのバージョン
		uint8 abi; ///< OSの種別
		uint8 abi_version; ///< OSのバージョン
		uint8 reserve[7]; ///< 予約領域（未使用）
	} id;
	short type; ///< ファイルの種別
	short arch; ///< CPUの種別
	long version; ///< ELF形式のバージョン
	long entry_point; ///< 実行開始アドレス
	long program_header_offset; ///< プログラム・ヘッダ・テーブルの位置
	long section_header_offset; ///< セクション・ヘッダ・テーブルの位置
	long flags; ///< 各種フラグ
	short header_size; ///< プログラム・ヘッダのサイズ
	short program_header_size; ///< プログラム・ヘッダのサイズ
	short program_header_num; ///< プログラム・ヘッダの個数
	short section_header_size; ///< セクション・ヘッダのサイズ
	short section_header_num; ///< セクション・ヘッダの個数
	short section_name_index; ///< セクション名を格納するセクション
};


/**
 * プログラムヘッダの定義
 */
struct elf_program_header {
	long type; ///< セグメントの種別
	long offset; ///< ファイル中の位置
	long virtual_addr; ///< 論理アドレス(VA)
	long physical_addr; ///< 物理アドレス(PA)
	long file_size; ///< ファイル中のサイズ
	long memory_size; ///< メモリ城でのサイズ
	long flag; ///< 各種フラグ
	long align; ///< アラインメント
};


/** 
 * ELFヘッダのチェック
 */
static int elf_check(struct elf_header * header);
/**<
 * \param header チェックするELFヘッダ
 * \return 解析結果
 * \retval EXIT_SUCCESS OK
 * \retval -1 NG
 */

/** 
 * セグメント単位でのロード
 */
static int elf_load_program(struct elf_header * header);
/**<
 * \param header 解析するELFヘッダ
 * \return EXIT_SUCCESS
 *
 * \pre ELFヘッダのフォーマットはチェック済み
 */

/** 
 * 1つのプログラム・ヘッダのロード
 */
static int elf_load_header(struct elf_header * header, struct elf_program_header * phdr);
/**<
 * \param header ELFヘッダ情報
 * \param phdr ロードするプログラム・ヘッダ
 *
 * \return EXIT_SUCCESS
 */

/* ELFヘッダのチェック */
static int elf_check(struct elf_header * header)
{
	if (memcmp(header->id.magic, "\x7f" "ELF", 4)) { 
		puts("magic is wrong\n");
	} else if (header->id.class != 1) {
		puts("id class is wrong\n");
	} else if (header->id.format != 2) {
		puts("id format is wrong\n");
	} else if (header->id.version != 1) {
		puts("id version is wrong\n");
	} else if (header->type != 2) {
		puts("id type is wrong\n");
	} else if (header->version != 1) {
		puts("version is wrong\n");
	} else if ((header->arch != 46) && (header->arch != 47)) {
		puts("arch is wrong\n");
	} else {
		return EXIT_SUCCESS;
	}

	puts("elf header check fail.\n");
	return -1;
}

/* セグメント単位でのロード */
static int elf_load_program(struct elf_header * header)
{
	for (int i=0; i<header->program_header_num; i++) {
		struct elf_program_header * phdr = 
			(struct elf_program_header *) ((char *)header + header->program_header_offset + header->program_header_size * i);

		elf_load_header(header, phdr);
	}

	return EXIT_SUCCESS;
}

/* 1つのプログラム・ヘッダのロード */
static int elf_load_header(struct elf_header * header, struct elf_program_header * phdr)
{
	char * start_addr = (char *)phdr->physical_addr;
	long bss = phdr->memory_size - phdr->file_size;
	
	if (phdr->type != 1) {
	} else {
		memcpy(start_addr, (char *)header + phdr->offset, phdr->file_size);
		memset(start_addr + phdr->file_size, 0, bss);
	}

	return EXIT_SUCCESS;
}

/* ELF形式の解析 */
char * elf_load(const char * buf)
{
	struct elf_header * header = (struct elf_header *)buf;

	if (elf_check(header) < 0) {
	} else if (elf_load_program(header) == EXIT_SUCCESS) {
		return (char *)header->entry_point;
	}

	return NULL;
}
