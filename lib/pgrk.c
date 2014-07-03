#include "pgrk.h"
#include "base.h"
#include <stdlib.h>
#include <math.h>

double *pagerank(struct Net *net, double c) {
	printgfb();
	double *pgrk = smalloc((net->maxId + 1)*sizeof(double));
	double *pgrk_swap = smalloc((net->maxId + 1)*sizeof(double));
	char *sign = scalloc((net->maxId + 1), sizeof(char));
	double od0Source = 0, od0Source_swap;
	double initOutlinkSource = (1-c)*1.0/(net->maxId + 1);
	int i, j, k;
	for (i = 0; i < net->maxId + 1; ++i) {
		pgrk[i] = 1.0;
		if (net->degree[i] == 0) {
			od0Source += initOutlinkSource;
		}
	}
	od0Source_swap = od0Source;
	int loopNum = 0;
	int getright = 0;
	double precision = 1E-12;
	while(1) {
		for (j = 0; j < net->maxId + 1; ++j) {
			double pr = c;
			for (k = 0; k < net->indegree[j]; ++k) {
				int neigh = net->inedges[j][k];
				int outdg = net->degree[neigh];
				pr += (1-c)*pgrk[neigh]/outdg;
			}
			pr += od0Source;
			if (fabs(pgrk[j] - pr) < precision && sign[j] != 4) {
				++sign[j];
				if (sign[j] == 4) ++getright;
			}
			if (net->degree[j] == 0) {
				od0Source_swap += (1-c)*(pr-pgrk[j])/(net->maxId + 1);
			}
			pgrk_swap[j] = pr;
		}
		double *tmp = pgrk;
		pgrk = pgrk_swap;
		pgrk_swap = tmp;
		od0Source = od0Source_swap;
		if (getright == net->maxId + 1) {
			break;
		}
		++loopNum;
		printgf("loopNum: %d\tgetright: %d\n", loopNum, getright);
	}
	printgf("loopNum: %d\n", loopNum);
	free(sign);
	free(pgrk_swap);
	printgfe();
	return pgrk;
}

static double find_sim(struct Net *simnet, int from, int to) {
	if (from > simnet->maxId) return 0;
	int i;
	for (i = 0; i < simnet->degree[from]; ++i) {
		if (simnet->edges[from][i] == to) return simnet->weight[from][i];
	}
	return 0;
}

double *simpagerank(struct Net *net, double c, struct Net *simnet) {
	printgfb();
	double *pgrk = smalloc((net->maxId + 1)*sizeof(double));
	double *pgrk_swap = smalloc((net->maxId + 1)*sizeof(double));
	char *sign = scalloc((net->maxId + 1), sizeof(char));
	double od0Source = 0, od0Source_swap;
	double initOutlinkSource = (1-c)*1.0/(net->maxId + 1);
	int i, j, k;
	for (i = 0; i < net->maxId + 1; ++i) {
		pgrk[i] = 1.0;
		//if (net->degree[i] == 0) {
		if (i > simnet->maxId || simnet->degree[i] == 0) {
			od0Source += initOutlinkSource;
		}
	}
	od0Source_swap = od0Source;
	int loopNum = 0;
	int getright = 0;
	double precision = 1E-7;
	while(1 && loopNum < 200) {
		for (j = 0; j < net->maxId + 1; ++j) {
			double pr = c;
			for (k = 0; k < net->indegree[j]; ++k) {
				int neigh = net->inedges[j][k];
				double sim = find_sim(simnet, neigh, j);
				pr += (1-c)*pgrk[neigh]*sim;
			}
			pr += od0Source;
			if (fabs(pgrk[j] - pr) < precision && sign[j] != 4) {
				++sign[j];
				if (sign[j] == 4) ++getright;
			}
			//if (net->degree[j] == 0) {
			if (j > simnet->maxId || simnet->degree[j] == 0) {
				od0Source_swap += (1-c)*(pr-pgrk[j])/(net->maxId + 1);
			}
			pgrk_swap[j] = pr;
		}
		double *tmp = pgrk;
		pgrk = pgrk_swap;
		pgrk_swap = tmp;
		od0Source = od0Source_swap;
		if (getright == net->maxId + 1) {
			break;
		}
		++loopNum;
		printgf("loopNum: %d\tgetright: %d\n", loopNum, getright);
	}
	free(sign);
	free(pgrk_swap);
	printgfe();
	double total=0;
	for (i = 0; i < net->maxId + 1; ++i) {
		total += pgrk[i];
	}
	printf("simpagerank: %f\n", total);
	return pgrk;
}
