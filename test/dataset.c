#include <check.h>
#include "dataset.h"
#include "iinet.h"

START_TEST (test_dataset_ring)
{
	struct LineFile *ring = line1d_DS(100, CYCLE, NON_DIRECT);
	struct iiNet *net = create_iiNet(ring);
	ck_assert_int_eq(ring->linesNum, 100);
	ck_assert_int_eq(net->idNum, 100);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->maxId, 99);
	ck_assert_int_eq(net->edgesNum, 100);
	ck_assert_int_eq(net->countMin, 2);
	ck_assert_int_eq(net->countMax, 2);
	free_LineFile(ring);
	free_iiNet(net);
}
END_TEST

START_TEST (test_dataset_lattice)
{
	struct LineFile *lattice = lattice2d_DS(100, CYCLE, NON_DIRECT);
	struct iiNet *net = create_iiNet(lattice);
	ck_assert_int_eq(lattice->linesNum, 20000);
	ck_assert_int_eq(net->idNum, 10000);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->maxId, 9999);
	ck_assert_int_eq(net->edgesNum, 20000);
	ck_assert_int_eq(net->countMin, 4);
	ck_assert_int_eq(net->countMax, 4);
	free_LineFile(lattice);
	free_iiNet(net);
}
END_TEST

Suite *dataset_suite(void) {
	Suite *s = suite_create("dataset");

	TCase *tc_ring = tcase_create("ring");
	tcase_add_test(tc_ring, test_dataset_ring);
	suite_add_tcase(s, tc_ring);

	TCase *tc_lattice = tcase_create("lattice");
	tcase_add_test(tc_lattice, test_dataset_lattice);
	suite_add_tcase(s, tc_lattice);

	return s;
}
