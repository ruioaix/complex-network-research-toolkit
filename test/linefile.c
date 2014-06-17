#include <check.h>
#include "linefile.h"

START_TEST (test_linefile_create)
{
	struct LineFile *empty = create_LineFile(NULL);
	ck_assert_ptr_eq(empty->ilist[0], &(empty->i1));
	free_LineFile(empty);
}
END_TEST

Suite *linefile_suite(void) {
	Suite *s = suite_create("linefile");

	TCase *tc_ring = tcase_create("create");
	tcase_add_test(tc_ring, test_linefile_create);
	suite_add_tcase(s, tc_ring);

	return s;
}
