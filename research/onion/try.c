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

int main(int argc, char **argv) {
	//the net.
	struct LineFile *lf = create_LineFile("/tmp/leadership_data", 1, 1, -1);	
	struct Net *net = create_directed_Net(lf);

	//the simnet.
	struct Net *tmp = create_Net(lf);
	free_LineFile(lf);
	delete_duplicatepairs_Net(tmp);
	struct LineFile *simlf = similarity_CN_Net(tmp);
	struct Net *simnet = create_Net(simlf);

	double *pgrk = simpagerank(net, 0.15, simnet);

	return 0;

	/*
	//double *classic = pagerank(net, 0.15);
	double total = 0;
	int i;
	//printf("%d\t%d\n", sim2->maxId, net->maxId);
	//assert(sn->maxId == net->maxId);
	////return 0;
	//for (i = 0; i < sim2->maxId + 1; ++i) {
	//	if (sim2->degree[i] != net->degree[i]) {
	//		printf("xx: %d\t%d\t%d\n", i, sim2->degree[i], net->degree[i]);
	//	}
	//}
	//int *id = smalloc((net->maxId + 1) * sizeof(int));
	//int outNum = 0;
	//for (i = 0; i < net->maxId + 1; ++i) {
	//	if (net->degree[i]) ++outNum;
	//}
	//printf("%d\n", outNum);
	//for (i = 0; i < net->maxId + 1; ++i) {
	//	//total += classic[i];
	//	id[i] = i;
	//}
	//printf("%f\n", total);
	//fflush(stdout);
	////qsort_di_desc(classic, 0, net->maxId, id);
	//for (i = 0; i < 50; ++i) {
	//	printf("%d\t%d\n", i, id[i]);
	//	fflush(stdout);
	//}
	////free(classic);

	//struct LineFile *simlf = similarity_linkout_CN_directed_Net(net);
	//struct Net *simnet= create_directed_Net(simlf);
	//outNum = 0;
	//for (i = 0; i < simnet->maxId + 1; ++i) {
	//	if (simnet->degree[i]) ++outNum;
	//}
	//printf("%d\n", outNum);
	
	//clean_duplicatepairs_Net(simnet, "1", "2");
	//delete_duplicatepairs_Net(simnet);
	for (i = 0; i < simnet->maxId + 1; ++i) {
		if (simnet->degree[i] > net->degree[i]) {
			printf("xx: %d\t%d\t%d\n", i, simnet->degree[i], net->degree[i]);
		}
	}

	double theta = 1;
	onion_pgrk_simnet_weight_normalize(simnet, theta);

	int j;
	for (i = 0; i < simnet->maxId + 1; ++i) {
		double tmp = 0;
		for (j = 0; j < simnet->degree[i]; ++j) {
			tmp += simnet->weight[i][j];
		}
		if (fabs(tmp -1) > 1E-7 && tmp != 0) {
			printf("%.17f\n", tmp);
			isError("xxx");
		}
	}

	total = 0;
	for (i = 0; i < net->maxId + 1; ++i) {
		total += pgrk[i];
		id[i] = i;
	}
	printf("%f\n", total);fflush(stdout);
	qsort_di_desc(pgrk, 0, net->maxId, id);
	for (i = 0; i < 50; ++i) {
		printf("%d\t%d\n", i, id[i]);
	}
	free(pgrk);

	free_LineFile(simlf);
	free_Net(simnet);
	free_Net(net);
	*/
	return 0;

}
