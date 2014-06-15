#include <cstdio>
#include <CppUTest/CommandLineTestRunner.h>

extern "C"{
#include <defines.h>
#include <serial.c> ///< test target
}

using namespace std;

TEST_GROUP(serial_test)
{
 serial_port sci;
  
  void setup()
  {
    sci = (serial_port)malloc(sizeof(struct _h8_3069f_sci));
    
    sci->smr = 0xAA;
    sci->brr = 0xAA;
    sci->scr = 0xAA;
    sci->tdr = 0xAA;
    sci->ssr = 0xAA;
    sci->rdr = 0xAA;
    sci->scmr = 0xAA;
  }

  void teardown()
  {
    free(sci);
  }
};

TEST(serial_test, check_init_value)
{
  LONGS_EQUAL(EXIT_SUCCESS, serial_init(sci));
  LONGS_EQUAL(0x00, sci->smr);
  LONGS_EQUAL(  64, sci->brr); //ボーレート9600bps, 動作周波数20MHz, smrのCKS1=0,CKS0=0の場合の設定値
  LONGS_EQUAL(0x30, sci->scr); //送受信は有効、それ以外の割り込みは無効
  LONGS_EQUAL(0xFF, sci->tdr);
  LONGS_EQUAL(0x00, sci->ssr);
  LONGS_EQUAL(0xAA, sci->rdr); //RDRはリード専用なので、初期化しないこと
  LONGS_EQUAL(0xF2, sci->scmr);//SCMRはリザーブ以外、全て0
}

TEST(serial_test, check_get_serial_port_from)
{
  // 正常系
  POINTERS_EQUAL((serial_port)SERIAL_SCI_0, get_serial_port_from(0));
  POINTERS_EQUAL((serial_port)SERIAL_SCI_1, get_serial_port_from(1));
  POINTERS_EQUAL((serial_port)SERIAL_SCI_2, get_serial_port_from(2));

  // 異常系
  // 負の数を与えた場合は、コンパイルエラーになるのでチェックしない
  POINTERS_EQUAL(NULL, get_serial_port_from(3));
}

TEST(serial_test, check_get_port_index_from)
{
  // 正常系
  LONGS_EQUAL(0, get_port_index_from((serial_port)SERIAL_SCI_0));
  LONGS_EQUAL(1, get_port_index_from((serial_port)SERIAL_SCI_1));
  LONGS_EQUAL(2, get_port_index_from((serial_port)SERIAL_SCI_2));

  // 異常系
  LONGS_EQUAL((port_index)-1, get_port_index_from((serial_port)SERIAL_SCI_NUM));

}

int main(int argc, char * argv[])
{
  return CommandLineTestRunner::RunAllTests(argc, argv);
}

