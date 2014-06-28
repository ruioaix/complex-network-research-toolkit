#include <check.h>
#include "spath.h"
#include "mtprand.h"
#include "../research/tdpotn/common.h"
#include <stdio.h>
#include "dataset.h"
#include "base.h"
#include <math.h>
#define ES  0.0000000000000001
#define LOOPNUM  3

static void printff(int D_12, int N, double avesp, double avesp_dj) {
	printf(" => D_12:%-2d, N:%-5d, avesp:%-15.10f, avesp_dj:%-15.10f\n", D_12, N, avesp, avesp_dj);
}

static test_spath_avesp_undirect_unweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN =5;
	double theta=1.0+get_d_MTPR(), lambda = 0;
	double alpha = get_d_MTPR()+0.5;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;	
	}
	baself->d1=weight;
	struct Net *base = create_Net(baself);

	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	free_Net(base);

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	printtb("dj");
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	printte("dj");
	free_Net(addnet);

	addnet = create_Net(addlf);
	addnet->duplicatepairsStatus = NS_NON_DUPPAIRS;
	addnet->connectnessStatus = NS_CNNTNESS;
	printtb("ud,uw,a,-");
	spath_avesp_undirect_unweight_Net(addnet, avesp);
	printte("ud,uw,a,-");
	free_Net(addnet);
	free_LineFile(addlf);
}

START_TEST (test_spath_avesp_undirect_unweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;

	int i;
	for (i = 0; i < LOOPNUM; ++i) {
		D_12 = get_i31_MTPR()%2 + 1; 
		int L = 5+get_i31_MTPR()%35;
		N = L*L;
		test_spath_avesp_undirect_unweight_Net_core(D_12, N, &avesp, &avesp_dj);
		printf("ud,uw,a,-");
		printff(D_12, N, avesp, avesp_dj);
		ck_assert(fabs(avesp - avesp_dj) < ES);
	}

}
END_TEST

static test_spath_avesp_coupling_undirect_unweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN =5;
	double theta=1.0+get_d_MTPR(), lambda = 0;
	double alpha = get_d_MTPR()+1.5;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;	
	}
	baself->d1=weight;
	struct Net *base = create_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;

	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	struct Net *air = create_Net(airlf);
	double coupling;
	printtb("ud,uw,a,c");
	spath_avesp_coupling_undirect_unweight_Net(base, air, avesp, &coupling);
	printte("ud,uw,a,c");
	free_Net(base);
	free_Net(air);

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	free_LineFile(addlf);
	printtb("dj");
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	printte("dj");
	free_Net(addnet);
}

START_TEST (test_spath_avesp_coupling_undirect_unweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;

	int i;
	for (i = 0; i < LOOPNUM; ++i) {
		D_12 = get_i31_MTPR()%2 + 1; 
		int L = 5+get_i31_MTPR()%35;
		N = L*L;
		test_spath_avesp_coupling_undirect_unweight_Net_core(D_12, N, &avesp, &avesp_dj);
		printf("ud,uw,a,c");
		printff(D_12, N, avesp, avesp_dj);
		ck_assert(fabs(avesp - avesp_dj) < ES);
	}

}
END_TEST

static void test_spath_avesp_gini_undirect_unweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN =5;
	double theta=1.0+get_d_MTPR(), lambda = 0;
	double alpha = get_d_MTPR()+1.5;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *edgesAttr = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		edgesAttr[i] = 0.0;
	}
	baself->d2 = edgesAttr;
	double *weight = smalloc(baself->linesNum * sizeof(double));
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;
	}
	baself->d1 = weight;
	struct Net *base = create_weighted_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;

	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	free_Net(base);
	double *edgesAttr2 = smalloc(baself->linesNum * sizeof(double));
	for (i = 0; i < baself->linesNum; ++i) {
		edgesAttr2[i] = 0.0;
	}
	airlf->d2 = edgesAttr2;

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	free_LineFile(addlf);
	printtb("dj");
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	printte("dj");
	double gini;
	double **wt = addnet->weight;
	addnet->weight = NULL;
	printtb("ud,uw,a,g");
	spath_avesp_gini_undirect_unweight_Net(addnet, avesp, &gini);
	printte("ud,uw,a,g");
	addnet->weight = wt;
	free_Net(addnet);
}

START_TEST (test_spath_avesp_gini_undirect_unweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;
	
	int i;
	for (i = 0; i < LOOPNUM; ++i) {
		D_12 = get_i31_MTPR()%2 + 1; 
		int L = 5+get_i31_MTPR()%35;
		N = L*L;
		test_spath_avesp_gini_undirect_unweight_Net_core(D_12, N, &avesp, &avesp_dj);
		printf("ud,uw,a,g");
		printff(D_12, N, avesp, avesp_dj);
		ck_assert(fabs(avesp - avesp_dj) < ES);
	}
}
END_TEST

static void test_spath_avesp_undirect_1upweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN = 5;
	double theta=1.0 + get_d_MTPR(), lambda = get_d01_MTPR();
	double alpha = 1.5 + get_d_MTPR();

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;
	}
	baself->d1 = weight;
	struct Net *base = create_weighted_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;
	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	free_Net(base);

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	printtb("ud,1w,a,-");
	spath_avesp_undirect_1upweight_Net(addnet, avesp);
	printte("ud,1w,a,-");
	free_LineFile(addlf);
	printtb("dj");
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	printte("dj");
	free_Net(addnet);
}

START_TEST (test_spath_avesp_undirect_1upweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;

	int i;
	for (i = 0; i < LOOPNUM; ++i) {
		D_12 = get_i31_MTPR()%2 + 1; 
		int L = 5+get_i31_MTPR()%35;
		N = L*L;
		test_spath_avesp_undirect_1upweight_Net_core(D_12, N, &avesp, &avesp_dj);
		printf("ud,1w,a,-");
		printff(D_12, N, avesp, avesp_dj);
		ck_assert(fabs(avesp - avesp_dj) < ES);
	}
}
END_TEST

static void test_spath_avesp_coupling_undirect_1upweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN = 5;
	double theta=1.0 + get_d_MTPR(), lambda = get_d01_MTPR();
	double alpha = 1.5 + get_d_MTPR();

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;
	}
	baself->d1 = weight;
	struct Net *base = create_weighted_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;
	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	struct Net *air = create_weighted_Net(airlf);
	double coupling;
	printtb("ud,1w,a,c");
	spath_avesp_coupling_undirect_1upweight_Net(base, air, avesp, &coupling);
	printte("ud,1w,a,c");
	free_Net(base);
	free_Net(air);

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	free_LineFile(addlf);
	printtb("dj");
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	printte("dj");
	free_Net(addnet);
}

START_TEST (test_spath_avesp_coupling_undirect_1upweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;

	int i;
	for (i = 0; i < LOOPNUM; ++i) {
		D_12 = get_i31_MTPR()%2 + 1; 
		int L = 5+get_i31_MTPR()%35;
		N = L*L;
		test_spath_avesp_coupling_undirect_1upweight_Net_core(D_12, N, &avesp, &avesp_dj);
		printf("ud,1w,a,c");
		printff(D_12, N, avesp, avesp_dj);
		ck_assert(fabs(avesp - avesp_dj) < ES);
	}
}
END_TEST

static void test_spath_avesp_gini_undirect_1upweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN =5;
	double theta=1.0+get_d_MTPR(), lambda = get_d01_MTPR();
	double alpha = get_d_MTPR()+1.5;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *edgesAttr = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		edgesAttr[i] = 0.0;
	}
	baself->d2 = edgesAttr;
	double *weight = smalloc(baself->linesNum * sizeof(double));
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;
	}
	baself->d1 = weight;
	struct Net *base = create_weighted_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;

	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	free_Net(base);
	double *edgesAttr2 = smalloc(baself->linesNum * sizeof(double));
	for (i = 0; i < baself->linesNum; ++i) {
		edgesAttr2[i] = 0.0;
	}
	airlf->d2 = edgesAttr2;

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	free_LineFile(addlf);
	printtb("dj");
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	printte("dj");
	double gini;
	printtb("ud,1w,a,g");
	spath_avesp_gini_undirect_1upweight_Net(addnet, avesp, &gini);
	printte("ud,1w,a,g");
	free_Net(addnet);
}

START_TEST (test_spath_avesp_gini_undirect_1upweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;
	
	int i;
	for (i = 0; i < LOOPNUM; ++i) {
		D_12 = get_i31_MTPR()%2 + 1; 
		int L = 5+get_i31_MTPR()%35;
		N = L*L;
		test_spath_avesp_gini_undirect_1upweight_Net_core(D_12, N, &avesp, &avesp_dj);
		printf("ud,1w,a,g");
		printff(D_12, N, avesp, avesp_dj);
		ck_assert(fabs(avesp - avesp_dj) < ES);
	}
}
END_TEST


Suite *spath_suite(void) {
	Suite *s = suite_create("shortest path");

	TCase *tc_spath_avesp_undirect_unweight_Net = tcase_create("spath_avesp_undirect_unweight_Net");
	tcase_set_timeout(tc_spath_avesp_undirect_unweight_Net, 0);
	tcase_add_test(tc_spath_avesp_undirect_unweight_Net, test_spath_avesp_undirect_unweight_Net);
	suite_add_tcase(s, tc_spath_avesp_undirect_unweight_Net);

	TCase *tc_spath_avesp_coupling_undirect_unweight_Net = tcase_create("spath_avesp_coupling_undirect_unweight_Net");
	tcase_set_timeout(tc_spath_avesp_coupling_undirect_unweight_Net, 0);
	tcase_add_test(tc_spath_avesp_coupling_undirect_unweight_Net, test_spath_avesp_coupling_undirect_unweight_Net);
	suite_add_tcase(s, tc_spath_avesp_coupling_undirect_unweight_Net);


	TCase *tc_spath_avesp_gini_undirect_unweight_Net = tcase_create("spath_avesp_gini_undirect_unweight_Net");
	tcase_set_timeout(tc_spath_avesp_gini_undirect_unweight_Net, 0);
	tcase_add_test(tc_spath_avesp_gini_undirect_unweight_Net, test_spath_avesp_gini_undirect_unweight_Net);
	suite_add_tcase(s, tc_spath_avesp_gini_undirect_unweight_Net);


	TCase *tc_spath_avesp_undirect_1upweight_Net = tcase_create("spath_avesp_undirect_1upweight_Net");
	tcase_set_timeout(tc_spath_avesp_undirect_1upweight_Net, 0);
	tcase_add_test(tc_spath_avesp_undirect_1upweight_Net, test_spath_avesp_undirect_1upweight_Net);
	suite_add_tcase(s, tc_spath_avesp_undirect_1upweight_Net);


	TCase *tc_spath_avesp_coupling_undirect_1upweight_Net = tcase_create("spath_avesp_coupling_undirect_1upweight_Net");
	tcase_set_timeout(tc_spath_avesp_coupling_undirect_1upweight_Net, 0);
	tcase_add_test(tc_spath_avesp_coupling_undirect_1upweight_Net, test_spath_avesp_coupling_undirect_1upweight_Net);
	suite_add_tcase(s, tc_spath_avesp_coupling_undirect_1upweight_Net);

	TCase *tc_spath_avesp_gini_undirect_1upweight_Net = tcase_create("spath_avesp_gini_undirect_1upweight_Net");
	tcase_set_timeout(tc_spath_avesp_gini_undirect_1upweight_Net, 0);
	tcase_add_test(tc_spath_avesp_gini_undirect_1upweight_Net, test_spath_avesp_gini_undirect_1upweight_Net);
	suite_add_tcase(s, tc_spath_avesp_gini_undirect_1upweight_Net);

	return s;
}
