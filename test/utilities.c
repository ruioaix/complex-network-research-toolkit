#include <check.h>
#include "utilities.h"

START_TEST (test_check_square)
{
	int i;
	long long pre=0, j;
	for (i=0; i < 1000; ++i) {
		long long sq = i*i;
		for (j=pre; j<sq; ++j) {
			ck_assert_int_eq(check_square(j), 0);
		}
		ck_assert_int_eq(check_square(sq), 1);
		pre = sq + 1;
	}
}
END_TEST

Suite *utilities_suite(void) {
	Suite *s = suite_create("utilities");

	TCase *tc_square = tcase_create("check_square");
	tcase_set_timeout(tc_square, 0);
	tcase_add_test(tc_square, test_check_square);
	suite_add_tcase(s, tc_square);

	return s;
}
