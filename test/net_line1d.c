#include <check.h>
#include "linefile.h"
#include "net.h"
#include "dataset.h"

START_TEST (test_net_ds_line1d_cycle_direct)
{
	int rl = 139;
	struct LineFile *ds = line1d_DS(rl, DS_CYCLE, DS_DIRECT);
	struct Net *net = create_directed_Net(ds);
	set_option_edgesMatrix_Net(net);
	ck_assert_int_eq(net->idNum, rl);
	ck_assert_int_eq(net->maxId, rl - 1);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->edgesNum, 2*rl);
	ck_assert_int_eq(net->edgesNum, ds->linesNum);
	ck_assert_int_eq(net->directStatus, NS_DIRECTED);
	ck_assert_int_eq(net->weight, NULL);
	ck_assert_int_eq(net->connectnessStatus, NS_NOTSURE);
	ck_assert_int_eq(net->duplicatepairsStatus, NS_NOTSURE);
	ck_assert_int_eq(net->degreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMax.value, 2);
	ck_assert_int_eq(net->degreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMin.value, 2);
	ck_assert_int_eq(net->indegreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->indegreeMax.value, 2);
	ck_assert_int_eq(net->indegreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->indegreeMin.value, 2);
	ck_assert_int_eq(net->avesp.sign, NS_NON_VALID);
	int j;	
	long en=0;
	for (j = 0; j < net->maxId + 1; ++j) {
		if (j== 0 ) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]], j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+rl-1]], j+rl-1);
			en += 2;
		}
		else if (j==rl-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-rl + 1]],j-rl+1);
			en += 2;
		}
		else {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			en += 2;
		}
	}
	ck_assert_int_eq(en, net->edgesNum);
	free_LineFile(ds);
	free_Net(net);
}
END_TEST

START_TEST (test_net_ds_line1d_uncycle_direct)
{
	int rl = 139;
	struct LineFile *ds = line1d_DS(rl, DS_NON_CYCLE, DS_DIRECT);
	struct Net *net = create_directed_Net(ds);
	set_option_edgesMatrix_Net(net);
	ck_assert_int_eq(net->idNum, rl);
	ck_assert_int_eq(net->maxId, rl - 1);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->edgesNum, 2*rl-2);
	ck_assert_int_eq(net->edgesNum, ds->linesNum);
	ck_assert_int_eq(net->directStatus, NS_DIRECTED);
	ck_assert_int_eq(net->weight, NULL);
	ck_assert_int_eq(net->connectnessStatus, NS_NOTSURE);
	ck_assert_int_eq(net->duplicatepairsStatus, NS_NOTSURE);
	ck_assert_int_eq(net->degreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMax.value, 2);
	ck_assert_int_eq(net->degreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMin.value, 1);
	ck_assert_int_eq(net->indegreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->indegreeMax.value, 2);
	ck_assert_int_eq(net->indegreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->indegreeMin.value, 1);
	ck_assert_int_eq(net->avesp.sign, NS_NON_VALID);
	int j;	
	long en=0;
	for (j = 0; j < net->maxId + 1; ++j) {
		if (j== 0 ) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]], j+1);
			en += 1;
		}
		else if (j==rl-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			en += 1;
		}
		else {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			en += 2;
		}
	}
	ck_assert_int_eq(en, net->edgesNum);
	free_LineFile(ds);
	free_Net(net);
}
END_TEST

START_TEST (test_net_ds_line1d_uncycle_undirect)
{
	int rl = 139;
	struct LineFile *ds = line1d_DS(rl, DS_NON_CYCLE, DS_NON_DIRECT);
	struct Net *net = create_Net(ds);
	set_option_edgesMatrix_Net(net);
	ck_assert_int_eq(net->idNum, rl);
	ck_assert_int_eq(net->maxId, rl - 1);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->edgesNum, rl-1);
	ck_assert_int_eq(net->edgesNum, ds->linesNum);
	ck_assert_int_eq(net->directStatus, NS_UNDIRECTED);
	ck_assert_int_eq(net->weight, NULL);
	ck_assert_int_eq(net->connectnessStatus, NS_NOTSURE);
	ck_assert_int_eq(net->duplicatepairsStatus, NS_NOTSURE);
	ck_assert_int_eq(net->degreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMax.value, 2);
	ck_assert_int_eq(net->degreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMin.value, 1);
	ck_assert_int_eq(net->indegreeMax.sign, NS_NON_VALID);
	ck_assert_int_eq(net->indegreeMin.sign, NS_NON_VALID);
	ck_assert_int_eq(net->avesp.sign, NS_NON_VALID);
	int j;	
	long en=0;
	for (j = 0; j < net->maxId + 1; ++j) {
		if (j== 0 ) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]], j+1);
			en += 1;
		}
		else if (j==rl-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			en += 1;
		}
		else {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			en += 2;
		}
	}
	ck_assert_int_eq(en, 2*net->edgesNum);
	free_LineFile(ds);
	free_Net(net);
}
END_TEST

START_TEST (test_net_ds_line1d_cycle_undirect)
{
	int rl = 139;
	struct LineFile *ds = line1d_DS(rl, DS_CYCLE, DS_NON_DIRECT);
	struct Net *net = create_Net(ds);
	set_option_edgesMatrix_Net(net);
	ck_assert_int_eq(net->idNum, rl);
	ck_assert_int_eq(net->maxId, rl - 1);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->edgesNum, rl);
	ck_assert_int_eq(net->edgesNum, ds->linesNum);
	ck_assert_int_eq(net->directStatus, NS_UNDIRECTED);
	ck_assert_int_eq(net->weight, NULL);
	ck_assert_int_eq(net->connectnessStatus, NS_NOTSURE);
	ck_assert_int_eq(net->duplicatepairsStatus, NS_NOTSURE);
	ck_assert_int_eq(net->degreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMax.value, 2);
	ck_assert_int_eq(net->degreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMin.value, 2);
	ck_assert_int_eq(net->indegreeMax.sign, NS_NON_VALID);
	ck_assert_int_eq(net->indegreeMin.sign, NS_NON_VALID);
	ck_assert_int_eq(net->avesp.sign, NS_NON_VALID);
	int j;	
	long en=0;
	for (j = 0; j < net->maxId + 1; ++j) {
		if (j== 0 ) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]], j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+rl-1]], j+rl-1);
			en += 2;
		}
		else if (j==rl-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-rl+1]], j-rl+1);
			en += 2;
		}
		else {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			en += 2;
		}
	}
	ck_assert_int_eq(en, 2*net->edgesNum);
	free_LineFile(ds);
	free_Net(net);
}
END_TEST

Suite *net_line1d_suite(void) {
	Suite *s = suite_create("net line1d ds");

	TCase *ds_line1d_cycle_direct = tcase_create("ds_line1d_cycle_direct");
	tcase_set_timeout(ds_line1d_cycle_direct, 0);
	tcase_add_test(ds_line1d_cycle_direct, test_net_ds_line1d_cycle_direct);
	suite_add_tcase(s, ds_line1d_cycle_direct);

	TCase *ds_line1d_uncycle_direct = tcase_create("ds_line1d_uncycle_direct");
	tcase_set_timeout(ds_line1d_uncycle_direct, 0);
	tcase_add_test(ds_line1d_uncycle_direct, test_net_ds_line1d_uncycle_direct);
	suite_add_tcase(s, ds_line1d_uncycle_direct);

	TCase *ds_line1d_uncycle_undirect = tcase_create("ds_line1d_uncycle_undirect");
	tcase_set_timeout(ds_line1d_uncycle_undirect, 0);
	tcase_add_test(ds_line1d_uncycle_undirect, test_net_ds_line1d_uncycle_undirect);
	suite_add_tcase(s, ds_line1d_uncycle_undirect);

	TCase *ds_line1d_cycle_undirect = tcase_create("ds_line1d_cycle_undirect");
	tcase_set_timeout(ds_line1d_cycle_undirect, 0);
	tcase_add_test(ds_line1d_cycle_undirect, test_net_ds_line1d_cycle_undirect);
	suite_add_tcase(s, ds_line1d_cycle_undirect);

	return s;
}
