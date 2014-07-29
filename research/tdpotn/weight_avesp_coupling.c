#include <stdio.h>
#include "base.h"
#include "net.h"
#include "mtprand.h"
#include "common.h"
#include "spath.h"

/**
 * default: ./tdpotn-weight_avesp_coupling D_12   N    seed limitN theta lambda
 * default: ./tdpotn-weight_avesp_coupling  1    2500   1     5     1.2    0.2
 *
 * D_12 = 1, 1 means 1d, 2 means 2d.
 * N = 50, N is the number of vertices; 
 * 		for 2d lattice, if N is not a square number, there will be an error. 
 * 		e.g. 9/25/36 is square number, 21 is not.
 * seed = 1, it's the seed for PRNG. should be larger than 0.
 * limitN = 5, the constrain for this research, C = limitN * nodeNum.
 * theta = 1.0, it will change air. 
 * 		to guarantee we can generate air successful, theta >= 0.5.
 * 		theta decide the cost of building a new edges in air, 
 * 		if theta < 1, the cost of long edges is not so much, 
 * 		else the cost of long edges is much more expensive than short.
 * lambda = 0, this decide the effect of a air edge. 
 * 		if lambda = 0, all air edges's weight is 1, it's the best experience.
 * 		if lambda = 1, all air edges has no advance to base net. it's the worst experience.
 * 		lambda will keep 0 in this program, because net is unweighted.
 */
int main (int argc, char **argv) {

	print_time();

	int D_12, N, seed, limitN;
	double theta, lambda;
	tdpotn_argcv(argc, argv, &D_12, &N, &seed, &limitN, &theta, &lambda);

	set_seed_MTPR(seed);
	double gini = -1;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;
	}
	baself->d1 = weight;
	struct Net *base = create_Net(baself);
	set_status_duplicatepairs_Net(base);
	if (base->duplicatepairsStatus == NS_DUPPAIRS) {
		isError("the net has some duplicate pairs, please make the net clean");
	}
	free_LineFile(baself);

	int kk;
	for (kk = 0; kk < 41; ++kk) {
		double alpha = kk * 0.1;

		struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
		struct Net *air = create_Net(airlf);
		//printlp("air: Max: %d, Min: %d, idNum: %d, edgesNum: %ld\n", air->maxId, air->minId, air->idNum, air->edgesNum);
		//if (air->degreeMax.sign == NS_VALID) printlp("degreMax: %d\n", air->degreeMax.value);
		//if (air->degreeMin.sign == NS_VALID) printlp("degreMin: %d\n", air->degreeMin.value);
		free_LineFile(airlf);
		double avesp=0, coupling=0;
		spath_avesp_coupling_undirect_1upweight_Net(base, air, &avesp, &coupling);
		//spath_avesp_undirect_1upweight_Net(base, air, &avesp);
		tdpotn_print(D_12, base->idNum, seed, limitN, theta, lambda, alpha, avesp, coupling, gini);

		free_Net(air);
	}

	free_Net(base);
	print_time();
	return 0;
}
