#include "linefile.h"
#include "net.h"
#include "base.h"
#include "pgrk.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
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

#define N 50

int main(int argc, char **argv) {
	int i;
	//the net.
	struct LineFile *lf = create_LineFile("/tmp/leadership_data", 1, 1, -1);	
	struct Net *net = create_directed_Net(lf);

	double *pgrk = pagerank(net, 0.15);
	int *id = smalloc((net->maxId + 1)*sizeof(int));
	//qsort_di_desc(pgrk, 0, net->maxId, id);
	maxN_di_select(pgrk, net->maxId + 1, N, id);
	FILE *fp = sfopen("/tmp/pgrk_result_0.15", "w");
	for (i = 0; i < N; ++i) {
		fprintf(fp, "%d\t%d\t%f\n", i, id[i], pgrk[i]);
	}
	fclose(fp);
	free(pgrk);

	//the simnet.
	struct Net *tmp = create_Net(lf);
	free_LineFile(lf);
	delete_duplicatepairs_Net(tmp);
	assert(tmp->maxId == net->maxId);
	struct LineFile *simlf = similarity_CN_Net(tmp, net);
	free_Net(tmp);
	struct Net *simnet = create_directed_Net(simlf);
	free_LineFile(simlf);
	onion_pgrk_simnet_weight_normalize(simnet, 1);

	double *spgrk = simpagerank(net, 0.15, simnet);
	for (i = 0; i < net->maxId + 1; ++i) {
		id[i] = i;
	}
	maxN_di_select(spgrk, net->maxId + 1, N, id);
	//qsort_di_desc(spgrk, 0, net->maxId, id);
	free_Net(simnet);
	fp = sfopen("/tmp/spgrk_result_0.15", "w");
	for (i = 0; i < N; ++i) {
		fprintf(fp, "%d\t%d\t%f\n", i, id[i], spgrk[i]);
	}
	fclose(fp);
	free(spgrk);

	free_Net(net);
	return 0;
}
