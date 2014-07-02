#include "linefile.h"
#include "base.h"
#include "net.h"
#include "pgrk.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	//struct LineFile *lf = create_LineFile("/tmp/polblogs", 1, 1, -1);
	struct LineFile *lf = create_LineFile("/tmp/leadership_data", 1, 1, -1);
	struct Net *net = create_directed_Net(lf);

	double *pgrk = pagerank(net, 0.15);
	double total = 0;
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		total += pgrk[i];
	}
	printf("%.17f\n", total);

	free(pgrk);
	free_LineFile(lf);
	free_Net(net);
	return 0;
}
