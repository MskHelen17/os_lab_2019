#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>

#include "revert_string.h"
#include <CUnit/CUnit.h>
void test2(void){
  char our_string[] = "1+2";
  
  int f = 0;
  RevertString(our_string);
  if(!strcmp(our_string,"2+1")){
      f=1;
  }
  CU_ASSERT_TRUE(f);
}
void testRevertString(void) {
  char simple_string[] = "Hello";
  char str_with_spaces[] = "String with spaces";
  char str_with_odd_chars_num[] = "abc";
  char str_with_even_chars_num[] = "abcd";

  RevertString(simple_string);
  CU_ASSERT_STRING_EQUAL_FATAL(simple_string, "olleH");

  RevertString(str_with_spaces);
  CU_ASSERT_STRING_EQUAL_FATAL(str_with_spaces, "secaps htiw gnirtS");

  RevertString(str_with_odd_chars_num);
  CU_ASSERT_STRING_EQUAL_FATAL(str_with_odd_chars_num, "cba");

  RevertString(str_with_even_chars_num);
  CU_ASSERT_STRING_EQUAL_FATAL(str_with_even_chars_num, "dcba");
}

int main() {
  CU_pSuite pSuite = NULL; //создали набор тестов

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();   //инициализировали тестовый реестр

  /* add a suite to the registry */
  pSuite = CU_add_suite("Suite", NULL, NULL);   //добавляем набор тестов в реестр, даем ему имя Suite
                                                //и при этом не указываем инициализирующую функцию
                                                
  if (NULL == pSuite) { 
    CU_cleanup_registry();  //если произошел сбой, чистим память
    return CU_get_error();
  }

  /* add the tests to the suite */
  /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
  if ((NULL == CU_add_test(pSuite, "test2",test2)) ||
  (NULL == CU_add_test(pSuite, "test of RevertString function",testRevertString))) {
                               //добавляем созданный тест в набор
    CU_cleanup_registry();  //если произошел сбой, чистим память
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);    
  CU_basic_run_tests(); //запускаем тест
  CU_cleanup_registry();
  return CU_get_error();
}
