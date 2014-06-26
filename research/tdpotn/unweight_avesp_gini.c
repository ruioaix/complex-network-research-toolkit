#include <stdio.h>
#include "base.h"
#include "net.h"
#include "mtprand.h"
#include "common.h"
#include "spath.h"

/**
 * default: ./tdpotn-unweight_gini D_12   N    seed limitN theta lambda
 * default: ./tdpotn-unweight_gini  1    2500   1     5     1.0    0
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
	double coupling = -1;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *edgesAttr = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		edgesAttr[i] = 0.0;
	}
	baself->d2 = edgesAttr;
	struct Net *base = create_Net(baself);
	set_status_duplicatepairs_Net(base);
	if (base->duplicatepairsStatus == NS_DUPPAIRS) {
		isError("the net has some duplicate pairs, please make the net clean");
	}

	int kk=0;
	for (kk = 0; kk < 41; ++kk) {
		double alpha = kk * 0.1;

		struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
		for (i = 0; i < airlf->linesNum; ++i) {
			airlf->d1[i] = 0.0;
		}
		airlf->d2 = airlf->d1;
		airlf->d1 = NULL;
		struct LineFile *lf = add_LineFile(airlf, baself);
		free_LineFile(airlf);
		struct Net *net = create_Net(lf);
		free_LineFile(lf);
		set_option_edgesMatrix_Net(net);
		double avesp, gini;
		spath_avesp_gini_undirect_unweight_Net(net, &avesp, &gini);
		tdpotn_print(D_12, base->idNum, seed, limitN, theta, lambda, alpha, avesp, coupling, gini);

		free_Net(net);
	}

	free_LineFile(baself);
	free_Net(base);
	print_time();
	return 0;
}
