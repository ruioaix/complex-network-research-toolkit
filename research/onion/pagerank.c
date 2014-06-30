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

	struct LineFile *lf = create_LineFile("/tmp/pbdata", 1, 1, -1);
	struct Net *net = create_directed_Net(lf);
	printf("maxId: %d, minId: %d, idNum: %d\n", net->maxId, net->minId, net->idNum);

	double *pgrk = smalloc((net->maxId + 1)*sizeof(double));
	double d = 0.85;
	int i, j, k;
	for (i = 0; i < net->maxId + 1; ++i) {
		pgrk[i] = 1.0;
	}
	for (i = 0; i < loopNum; ++i) {
		for (j = 0; j < net->maxId + 1; ++j) {
			double pr = 1-d;
			for (k = 0; k < net->indegree[j]; ++k) {
				int neigh = net->inedges[j][k];
				int outdg = net->degree[neigh];
				pr += d*pgrk[neigh]/outdg;
			}
			pgrk[j] = pr;
		}
	}
	for (i = 0; i < net->maxId + 1; ++i) {
		printf("%d\t%f\n", i, pgrk[i]);
		if (net->degree[i] == 0 && net->indegree[i] == 0) {
			if (pgrk[i] != 1 -d ) {
				isError("something is wrong");
			}
		}
	}

	return 0;
}
