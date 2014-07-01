#include "linefile.h"
#include "base.h"
#include "net.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	if (argc != 2) isError("wrong argc");
	int loopNum;
	char *p;
	loopNum = strtol(argv[1], &p, 10);

	struct LineFile *lf = create_LineFile("/tmp/polblogs", 1, 1, -1);
	struct Net *net = create_directed_Net(lf);
	//printf("maxId: %d, minId: %d, idNum: %d\n", net->maxId, net->minId, net->idNum);

	double *pgrk = smalloc((net->maxId + 1)*sizeof(double));
	double *pgrk_swap = smalloc((net->maxId + 1)*sizeof(double));
	int *noOutlinks = smalloc((net->maxId + 1)*sizeof(int));
	int noOutlinksNum = 0;
	double d = 0.15;
	int i, j, k;
	for (i = 0; i < net->maxId + 1; ++i) {
		pgrk[i] = 1.0;
		if (net->degree[i] == 0) {
			noOutlinks[noOutlinksNum++] = i;
		}
	}
	for (i = 0; i < loopNum; ++i) {
		for (j = 0; j < net->maxId + 1; ++j) {
			double pr = d;
			for (k = 0; k < net->indegree[j]; ++k) {
				int neigh = net->inedges[j][k];
				int outdg = net->degree[neigh];
				pr += (1-d)*pgrk[neigh]/outdg;
			}
			for (k = 0; k < noOutlinksNum; ++k) {
				int id = noOutlinks[k];
				pr += (1-d)*pgrk[id]/(net->maxId+1);
			}
			pgrk_swap[j] = pr;
		}
		double *tmp = pgrk;
		pgrk = pgrk_swap;
		pgrk_swap = tmp;
	}
	double total = 0;
	for (i = 0; i < net->maxId + 1; ++i) {
		//if (net->degree[i] == 0 && net->indegree[i] == 0) continue; 
		printf("%.15f\n", pgrk[i]);
		total += pgrk[i];
		//if (net->degree[i] == 0 && net->indegree[i] == 0) {
		//	if (pgrk[i] != 1 -d ) {
		//		isError("something is wrong");
		//	}
		//}
	}
	printf("%.10f\n", total);

	return 0;
}
