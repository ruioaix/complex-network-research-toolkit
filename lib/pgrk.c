#include "pgrk.h"
#include "base.h"
#include <math.h>

double *pagerank(struct Net *net, double c) {
	double *pgrk = smalloc((net->maxId + 1)*sizeof(double));
	int *noOutlinks = smalloc((net->maxId + 1)*sizeof(int));
	int noOutlinksNum = 0;
	char *sign = scalloc((net->maxId + 1), sizeof(char));
	int i, j, k;
	for (i = 0; i < net->maxId + 1; ++i) {
		pgrk[i] = 1.0;
		if (net->degree[i] == 0) {
			noOutlinks[noOutlinksNum++] = i;
		}
	}
	int loopNum = 0;
	int getright = 0;
	double precision = 0.0000000000001;
	printf("%d\n", noOutlinksNum);
	while(1) {
		for (j = 0; j < net->maxId + 1; ++j) {
			double pr = c;
			for (k = 0; k < net->indegree[j]; ++k) {
				int neigh = net->inedges[j][k];
				int outdg = net->degree[neigh];
				pr += (1-c)*pgrk[neigh]/outdg;
			}
			for (k = 0; k < noOutlinksNum; ++k) {
				int id = noOutlinks[k];
				pr += (1-c)*pgrk[id]/(net->maxId+1);
			}
			if (fabs(pgrk[j] - pr) < precision && sign[j] == 0) {
				sign[j] = 1;
				++getright;
			}
			pgrk[j] = pr;
			//printf("j:%d\n", j);
		}
		if (getright == net->maxId + 1) {
			break;
		}
		++loopNum;
		printf("%d\t%d\n", loopNum, getright);
	}
	printf("%d\n", loopNum);

	return pgrk;
}
