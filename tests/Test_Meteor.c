#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

int posicaoVaziameteors(game_item *meteors);

void testposicaoVaziameteors() {
    int result_empty_meteors = 0;
    int number_of_meteors = 20;

    result_empty_meteors = create_empty_shot_position(number_of_meteors);
  
    CU_ASSERT_EQUAL(result_empty_meteors, 20);
}

int main() {
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("Test_Meteor", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "testposicaoVaziameteors", testposicaoVaziameteors))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}