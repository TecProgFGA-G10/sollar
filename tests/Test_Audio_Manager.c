#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

int play_music(Mix_Music *music);

void testplay_music() {
    int result_play_music = 0;
    int first_case_music = -1;
    int second_case_music = 1;
    int third_case_music = NULL;
    result_play_music_first_case = play_music(first_case_music);
    result_play_music_second_case = play_music(second_case_music);
    result_play_music_third_case = play_music(third_case_music);
  
    CU_ASSERT_EQUAL(result_play_music_first_case, 1);
    CU_ASSERT_EQUAL(result_play_music_second_case, 1);
    CU_ASSERT_TRUE(result_play_music_third_case);
}

int main() {
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("Test_Audio_Manager", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "testplay_music", testplay_music))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}