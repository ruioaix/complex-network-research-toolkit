#include <check.h>
#include "net.h"
#include "dataset.h"
#include <stdlib.h>
#include <math.h>


START_TEST (test_net_ds_lattice2d_cycle_direct)
{
	int ledge = 100;
	struct LineFile *ds = lattice2d_DS(ledge*ledge, DS_CYCLE, DS_DIRECT);
	struct Net *net = create_directed_Net(ds);

}
END_TEST

//START_TEST (test_net_create_d_uw)
//{
//}
//END_TEST
//
//START_TEST (test_net_create_ud_w)
//{
//}
//END_TEST
//
//START_TEST (test_net_create_d_w)
//{
//}
//END_TEST

Suite *net_suite(void) {
	Suite *s = suite_create("net");

	TCase *tc_net_ds_lattice2d_direct = tcase_create("ds_lattice2d_direct");
	tcase_set_timeout(tc_net_ds_lattice2d_direct, 0);
	tcase_add_test(tc_net_ds_lattice2d_direct, test_net_ds_lattice2d_direct);
	suite_add_tcase(s, tc_net_ds_lattice2d_direct);

//	TCase *tc_create_d_uw = tcase_create("create_d_uw");
//	tcase_set_timeout(tc_create_d_uw, 0);
//	tcase_add_test(tc_create_d_uw, test_net_create_d_uw);
//	suite_add_tcase(s, tc_create_d_uw);
//
//	TCase *tc_create_ud_w = tcase_create("create_ud_w");
//	tcase_set_timeout(tc_create_ud_w, 0);
//	tcase_add_test(tc_create_ud_w, test_net_create_ud_w);
//	suite_add_tcase(s, tc_create_ud_w);
//
//	TCase *tc_create_d_w = tcase_create("create_d_w");
//	tcase_set_timeout(tc_create_d_w, 0);
//	tcase_add_test(tc_create_d_w, test_net_create_d_w);
//	suite_add_tcase(s, tc_create_d_w);

	return s;
}
