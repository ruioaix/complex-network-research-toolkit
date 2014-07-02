#include "pgrk.h"
#include "base.h"
#include <stdlib.h>
#include <math.h>

double *pagerank(struct Net *net, double c) {
	printgfb();
	double *pgrk = smalloc((net->maxId + 1)*sizeof(double));
	char *sign = scalloc((net->maxId + 1), sizeof(char));
	double od0Source = 0;
	double initOutlinkSource = (1-c)*1.0/(net->maxId + 1);
	int i, j, k;
	for (i = 0; i < net->maxId + 1; ++i) {
		pgrk[i] = 1.0;
		if (net->degree[i] == 0) {
			od0Source += initOutlinkSource;
		}
	}
	int loopNum = 0;
	int getright = 0;
	double precision = 1E-17;
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
				od0Source += (1-c)*(pr-pgrk[j])/(net->maxId + 1);
			}
			pgrk[j] = pr;
		}
		if (getright == net->maxId + 1) {
			break;
		}
		++loopNum;
	}
	printgf("loopNum: %d\n", loopNum);
	free(sign);
	printgfe();
	return pgrk;
}
