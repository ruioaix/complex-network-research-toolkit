#include <check.h>
#include "spath.h"
#include "mtprand.h"
#include "../research/tdpotn/common.h"
#include <stdio.h>
#include "base.h"
#include <math.h>
#define ES  0.0000000000000001


START_TEST (test_spath_avesp_coupling_undirect_unweight_Net_0)
{
	set_timeseed_MTPR();
	int D_12 = 1, N = 1000, seed=1, limitN =5;
	double theta=1.0+get_d_MTPR(), lambda = 0;
	double alpha = get_d_MTPR()+0.5;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	struct Net *base = create_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;	
	}
	baself->d1=weight;

	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	struct Net *air = create_Net(airlf);
	free_LineFile(airlf);
	double avesp, coupling;
	spath_avesp_coupling_undirect_unweight_Net(base, air, &avesp, &coupling);
	struct Net *addnet = create_weighted_Net(addlf, addlf->d1);
	double avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	printf("avesp: %f, avesp_dj: %f\n", avesp, avesp_dj);
	ck_assert(fabs(avesp - avesp_dj) < ES);
}
END_TEST

START_TEST (test_spath_avesp_coupling_undirect_unweight_Net_1)
{
	set_timeseed_MTPR();
	int D_12 = 2, N = 34*34, seed=1, limitN =5;
	double theta=1.0 + get_d_MTPR(), lambda = 0;
	double alpha = 0.5 + get_d_MTPR();

	struct LineFile *baself = tdpotn_lf(D_12, N);
	struct Net *base = create_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;	
	}
	baself->d1=weight;

	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	struct Net *air = create_Net(airlf);
	free_LineFile(airlf);
	double avesp, coupling;
	spath_avesp_coupling_undirect_unweight_Net(base, air, &avesp, &coupling);
	struct Net *addnet = create_weighted_Net(addlf, addlf->d1);
	double avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	printf("avesp: %f, avesp_dj: %f\n", avesp, avesp_dj);
	ck_assert(fabs(avesp - avesp_dj) < ES);
}
END_TEST

Suite *spath_suite(void) {
	Suite *s = suite_create("shortest path");

	TCase *tc_spath_avesp_coupling_undirect_unweight_Net = tcase_create("spath_avesp_coupling_undirect_unweight_Net");
	tcase_set_timeout(tc_spath_avesp_coupling_undirect_unweight_Net, 0);
	tcase_add_test(tc_spath_avesp_coupling_undirect_unweight_Net, test_spath_avesp_coupling_undirect_unweight_Net_0);
	tcase_add_test(tc_spath_avesp_coupling_undirect_unweight_Net, test_spath_avesp_coupling_undirect_unweight_Net_1);
	suite_add_tcase(s, tc_spath_avesp_coupling_undirect_unweight_Net);

	return s;
}
