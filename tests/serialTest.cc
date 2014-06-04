#include <cstdio>
#include <CppUTest/CommandLineTestRunner.h>

extern "C"{
#include <defines.h>
#include <serial.c>
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

TEST(serial_test, return_value)
{
  LONGS_EQUAL(EXIT_SUCCESS, serial_init(sci));
  LONGS_EQUAL( 0, sci->smr);
  LONGS_EQUAL(64, sci->brr);
  LONGS_EQUAL(H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE, sci->scr);
  LONGS_EQUAL( 0, sci->tdr);
  LONGS_EQUAL( 0, sci->ssr);
  LONGS_EQUAL( 0, sci->rdr);
  LONGS_EQUAL( 0, sci->scmr);
}

int main(int argc, char * argv[])
{
  return CommandLineTestRunner::RunAllTests(argc, argv);
}

