#include <check.h>
#include "net.h"
#include "dataset.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


START_TEST (test_net_ds_lattice2d_cycle_direct)
{
	int ledge = 139;
	struct LineFile *ds = lattice2d_DS(ledge*ledge, DS_CYCLE, DS_DIRECT);
	struct Net *net = create_directed_Net(ds);
	set_edgesMatrix_Net(net);
	ck_assert_int_eq(net->idNum, ledge*ledge);
	ck_assert_int_eq(net->maxId, ledge*ledge - 1);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->edgesNum, ledge*ledge*4);
	ck_assert_int_eq(net->edgesNum, ds->linesNum);
	ck_assert_int_eq(net->directStatus, NS_DIRECTED);
	ck_assert_int_eq(net->weightStatus, NS_UNWEIGHTED);
	ck_assert_int_eq(net->connectnessStatus, NS_NOTSURE);
	ck_assert_int_eq(net->duplicatepairsStatus, NS_NOTSURE);
	ck_assert_int_eq(net->degreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMax.value, 4);
	ck_assert_int_eq(net->degreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMin.value, 4);
	ck_assert_int_eq(net->indegreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->indegreeMax.value, 4);
	ck_assert_int_eq(net->indegreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->indegreeMin.value, 4);
	ck_assert_int_eq(net->avesp.sign, NS_NON_VALID);
	int j;
	for (j = 0; j < net->maxId + 1; ++j) {
		ck_assert_int_eq(net->degree[j], 4);
		ck_assert_int_eq(net->indegree[j], 4);
	}
	for (j = 0; j < net->maxId + 1; ++j) {
		if (j== 0 ) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]], j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]], j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge*(ledge-1)]],j+ledge*(ledge-1));
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge-1]],j+ledge-1);
		}
		else if (j==ledge-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge+1]],j-ledge+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge*(ledge-1)]],j+ledge*(ledge-1));
		}
		else if (j== ledge*(ledge-1)) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge*(ledge-1)]],j-ledge*(ledge-1));
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge-1]],j+ledge-1);
		}
		else if (j==ledge*ledge -1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge*(ledge -1)]],j-ledge*(ledge -1));
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-(ledge -1)]],j-(ledge -1));
		}
		else if (j < ledge-1 && j>0) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge * (ledge - 1)]],j+ledge * (ledge - 1));
		}
		else if (j%ledge== 0) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge-1]],j+ledge-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
		}
		else if (j%ledge== ledge-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-(ledge-1)]],j-(ledge-1));
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
		}
		else if (j<ledge*ledge-1 && j>(ledge-1)*ledge) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge*(ledge-1)]],j-ledge*(ledge-1));
		}
		else {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
		}
	}
	free_LineFile(ds);
	free_Net(net);
}
END_TEST

START_TEST (test_net_ds_lattice2d_uncycle_direct)
{
	int ledge = 139;
	struct LineFile *ds = lattice2d_DS(ledge*ledge, DS_NON_CYCLE, DS_DIRECT);
	struct Net *net = create_directed_Net(ds);
	set_edgesMatrix_Net(net);
	ck_assert_int_eq(net->idNum, ledge*ledge);
	ck_assert_int_eq(net->maxId, ledge*ledge - 1);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->edgesNum, 2*((ledge-1)*(ledge-1)*2+2*ledge-2));
	ck_assert_int_eq(net->edgesNum, ds->linesNum);
	ck_assert_int_eq(net->directStatus, NS_DIRECTED);
	ck_assert_int_eq(net->weightStatus, NS_UNWEIGHTED);
	ck_assert_int_eq(net->connectnessStatus, NS_NOTSURE);
	ck_assert_int_eq(net->duplicatepairsStatus, NS_NOTSURE);
	ck_assert_int_eq(net->degreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMax.value, 4);
	ck_assert_int_eq(net->degreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMin.value, 2);
	ck_assert_int_eq(net->indegreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->indegreeMax.value, 4);
	ck_assert_int_eq(net->indegreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->indegreeMin.value, 2);
	ck_assert_int_eq(net->avesp.sign, NS_NON_VALID);
	vertex_t j;
	for (j = 0; j < net->maxId + 1; ++j) {
		if (j== 0 ) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]], j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]], j+ledge);
		}
		else if (j==ledge-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
		}
		else if (j== ledge*(ledge-1)) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
		}
		else if (j==ledge*ledge -1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
		}
		else if (j < ledge-1 && j>0) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
		}
		else if (j%ledge== 0) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
		}
		else if (j%ledge== ledge-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
		}
		else if (j<ledge*ledge-1 && j>(ledge-1)*ledge) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
		}
		else {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
		}
	}
	free_LineFile(ds);
	free_Net(net);
}
END_TEST

START_TEST (test_net_ds_lattice2d_cycle_undirect)
{
	int ledge = 139;
	struct LineFile *ds = lattice2d_DS(ledge*ledge, DS_CYCLE, DS_NON_DIRECT);
	struct Net *net = create_Net(ds);
	set_edgesMatrix_Net(net);
	ck_assert_int_eq(net->idNum, ledge*ledge);
	ck_assert_int_eq(net->maxId, ledge*ledge - 1);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->edgesNum, ledge*ledge*2);
	ck_assert_int_eq(net->edgesNum, ds->linesNum);
	ck_assert_int_eq(net->directStatus, NS_UNDIRECTED);
	ck_assert_int_eq(net->weightStatus, NS_UNWEIGHTED);
	ck_assert_int_eq(net->connectnessStatus, NS_NOTSURE);
	ck_assert_int_eq(net->duplicatepairsStatus, NS_NOTSURE);
	ck_assert_int_eq(net->degreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMax.value, 4);
	ck_assert_int_eq(net->degreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMin.value, 4);
	ck_assert_int_eq(net->indegreeMax.sign, NS_NON_VALID);
	ck_assert_int_eq(net->indegreeMin.sign, NS_NON_VALID);
	ck_assert_int_eq(net->avesp.sign, NS_NON_VALID);
	int j;
	for (j = 0; j < net->maxId + 1; ++j) {
		ck_assert_int_eq(net->degree[j], 4);
		if (j== 0 ) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]], j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]], j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge*(ledge-1)]],j+ledge*(ledge-1));
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge-1]],j+ledge-1);
		}
		else if (j==ledge-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge+1]],j-ledge+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge*(ledge-1)]],j+ledge*(ledge-1));
		}
		else if (j== ledge*(ledge-1)) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge*(ledge-1)]],j-ledge*(ledge-1));
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge-1]],j+ledge-1);
		}
		else if (j==ledge*ledge -1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge*(ledge -1)]],j-ledge*(ledge -1));
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-(ledge -1)]],j-(ledge -1));
		}
		else if (j < ledge-1 && j>0) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge * (ledge - 1)]],j+ledge * (ledge - 1));
		}
		else if (j%ledge== 0) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge-1]],j+ledge-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
		}
		else if (j%ledge== ledge-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-(ledge-1)]],j-(ledge-1));
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
		}
		else if (j<ledge*ledge-1 && j>(ledge-1)*ledge) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge*(ledge-1)]],j-ledge*(ledge-1));
		}
		else {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
		}
	}
	free_LineFile(ds);
	free_Net(net);
}
END_TEST

START_TEST (test_net_ds_lattice2d_uncycle_undirect)
{
	int ledge = 139;
	struct LineFile *ds = lattice2d_DS(ledge*ledge, DS_NON_CYCLE, DS_NON_DIRECT);
	struct Net *net = create_Net(ds);
	set_edgesMatrix_Net(net);
	ck_assert_int_eq(net->idNum, ledge*ledge);
	ck_assert_int_eq(net->maxId, ledge*ledge - 1);
	ck_assert_int_eq(net->minId, 0);
	ck_assert_int_eq(net->edgesNum, (ledge-1)*(ledge-1)*2+2*ledge-2);
	ck_assert_int_eq(net->edgesNum, ds->linesNum);
	ck_assert_int_eq(net->directStatus, NS_UNDIRECTED);
	ck_assert_int_eq(net->weightStatus, NS_UNWEIGHTED);
	ck_assert_int_eq(net->connectnessStatus, NS_NOTSURE);
	ck_assert_int_eq(net->duplicatepairsStatus, NS_NOTSURE);
	ck_assert_int_eq(net->degreeMax.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMax.value, 4);
	ck_assert_int_eq(net->degreeMin.sign, NS_VALID);
	ck_assert_int_eq(net->degreeMin.value, 2);
	ck_assert_int_eq(net->indegreeMax.sign, NS_NON_VALID);
	ck_assert_int_eq(net->indegreeMin.sign, NS_NON_VALID);
	ck_assert_int_eq(net->avesp.sign, NS_NON_VALID);
	vertex_t j;	
	edge_t en=0;
	for (j = 0; j < net->maxId + 1; ++j) {
		if (j== 0 ) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]], j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]], j+ledge);
			en += 2;
		}
		else if (j==ledge-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			en += 2;
		}
		else if (j== ledge*(ledge-1)) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			en += 2;
		}
		else if (j==ledge*ledge -1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			en += 2;
		}
		else if (j < ledge-1 && j>0) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			en += 3;
		}
		else if (j%ledge== 0) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			en += 3;
		}
		else if (j%ledge== ledge-1) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			en += 3;
		}
		else if (j<ledge*ledge-1 && j>(ledge-1)*ledge) {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			en += 3;
		}
		else {
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-1]],j-1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+1]],j+1);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j-ledge]],j-ledge);
			ck_assert_int_eq(net->edges[j][net->edgesMatrix.pp[j][j+ledge]],j+ledge);
			en += 4;
		}
	}
	ck_assert_int_eq(en, 2*net->edgesNum);
	free_LineFile(ds);
	free_Net(net);
}
END_TEST

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

Suite *net_lattice2d_suite(void) {
	Suite *s = suite_create("net lattice2d ds");

	TCase *ds_lattice2d_cycle_direct = tcase_create("ds_lattice2d_cycle_direct");
	tcase_set_timeout(ds_lattice2d_cycle_direct, 0);
	tcase_add_test(ds_lattice2d_cycle_direct, test_net_ds_lattice2d_cycle_direct);
	suite_add_tcase(s, ds_lattice2d_cycle_direct);

	TCase *ds_lattice2d_uncycle_direct = tcase_create("ds_lattice2d_uncycle_direct");
	tcase_set_timeout(ds_lattice2d_uncycle_direct, 0);
	tcase_add_test(ds_lattice2d_uncycle_direct, test_net_ds_lattice2d_uncycle_direct);
	suite_add_tcase(s, ds_lattice2d_uncycle_direct);
	
	TCase *ds_lattice2d_cycle_undirect = tcase_create("ds_lattice2d_cycle_undirect");
	tcase_set_timeout(ds_lattice2d_cycle_undirect, 0);
	tcase_add_test(ds_lattice2d_cycle_undirect, test_net_ds_lattice2d_cycle_undirect);
	suite_add_tcase(s, ds_lattice2d_cycle_undirect);

	TCase *ds_lattice2d_uncycle_undirect = tcase_create("ds_lattice2d_uncycle_undirect");
	tcase_set_timeout(ds_lattice2d_uncycle_undirect, 0);
	tcase_add_test(ds_lattice2d_uncycle_undirect, test_net_ds_lattice2d_uncycle_undirect);
	suite_add_tcase(s, ds_lattice2d_uncycle_undirect);

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
