#include <check.h>
#include <stdlib.h>
#include "dataset.h"

START_TEST (test_dataset_ring)
{
	struct LineFile *ring = line1d_DS(100, CYCLE, NON_DIRECT);
	ck_assert_int_eq(ring->linesNum, 100);
	free_LineFile(ring);
}
END_TEST

Suite *dataset_suite(void) {
	Suite *s = suite_create("dataset");
	TCase *tc_ring = tcase_create("ring");
	tcase_add_test(tc_ring, test_dataset_ring);
	suite_add_tcase(s, tc_ring);
	return s;
}

int main(void) {
	int number_failed;
	Suite *s = dataset_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS: EXIT_FAILURE;
}
