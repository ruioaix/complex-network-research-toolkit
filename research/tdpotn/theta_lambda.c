#include <stdio.h>
#include "base.h"
#include "net.h"
//#include "shortestpath.h"
#include "mtprand.h"
#include "common.h"

/**
 * default: ./tdpotn-theta_lambda  D_12   N    seed limitN theta lambda
 * default: ./tdpotn-theta_lambda   1    2500   1     5     1.0    0
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
 * lambda = 1, this decide the effect of a air edge. 
 * 		if lambda = 0, all air edges's weight is 1, it's the best experience.
 * 		if lambda = 1, all air edges has no advance to base net. it's the worst experience.
 * 		actural lambda is only used here.
 */
int main (int argc, char **argv) {

	print_time();

	int D_12, N, seed, limitN;
	double theta, lambda;
	tdpotn_argcv(argc, argv, &D_12, &N, &seed, &limitN, &theta, &lambda);

	set_seed_MTPR(seed);
	double coupling = -1, gini = -1;

	int kk=0;
	for (kk = 0; kk < 41; ++kk) {
		double alpha = kk * 0.1;

		struct LineFile *baself = tdpotn_lf(D_12, N);
		struct Net *base = create_Net(baself);
		free_LineFile(baself);
		struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
		struct iidNet *air = create_iidNet(airlf);
		print1l("%s =>> create iidnet air, Max: %d, Min: %d, idNum: %d, edgesNum: %ld, countMax: %ld, countMin: %ld\n", \
				__func__, air->maxId, air->minId, air->idNum, air->edgesNum, air->countMax, air->countMin);
		free_LineFile(airlf);

		tdpotn_writenettofile_ii_iid(base, air, "tnet", "wtnet");

		double avesp;
		avesp_spath03_Net(base, air, &avesp);
		tdpotn_print(D_12, base->idNum, seed, limitN, theta, lambda, alpha, avesp, coupling, gini);

		free_iiNet(base);
		free_iidNet(air);
	}

	print_time();
	return 0;
}
