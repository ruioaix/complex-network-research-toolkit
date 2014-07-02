#include "linefile.h"
#include "net.h"
#include "base.h"
#include "pgrk.h"
#include <stdio.h>
#include <math.h>

static void onion_pgrk_simnet_weight_normalize(struct Net *net, double theta) {
	int i, j;
	double totalw;
	for (i = 0; i < net->maxId + 1; ++i) {
		totalw = 0;
		for (j = 0; j < net->degree[i]; ++j) {
			net->weight[i][j] = pow(net->weight[i][j], theta);
			totalw += net->weight[i][j];
		}
		for (j = 0; j < net->degree[i]; ++j) {
			net->weight[i][j] /= totalw;
		}
	}
}

int main(int argc, char **argv) {
	struct LineFile *lf = create_LineFile("/tmp/leadership_data", 1, 1, -1);	
	struct Net *net = create_directed_Net(lf);
	free_LineFile(lf);

	double *classic = pagerank(net, 0.15);
	double total = 0;
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		total += classic[i];
	}
	printf("%f\n", total);

	struct LineFile *simlf = similarity_linkboth_CN_directed_Net(net);
	struct Net *simnet= create_Net(simlf);
	delete_duplicatepairs_Net(simnet);

	double theta = 0.1;
	onion_pgrk_simnet_weight_normalize(simnet, theta);

	double *pgrk = simpagerank(net, 0.15, simnet);
	total = 0;
	for (i = 0; i < net->maxId + 1; ++i) {
		total += pgrk[i];
	}
	printf("%f\n", total);

	free_LineFile(simlf);
	free_Net(simnet);
	free_Net(net);
	return 0;
}
