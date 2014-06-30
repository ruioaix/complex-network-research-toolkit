#include "linefile.h"
#include "net.h"
#include <stdio.h>

int main(int argc, char **argv) {
	struct LineFile *lf = create_LineFile("/tmp/polblogs", 1, 1, -1);
	struct Net *net = create_directed_Net(lf);
	printf("maxId: %d, minId: %d, idNum: %d\n", net->maxId, net->minId, net->idNum);
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (net->degree[i] == 0 && net->indegree[i] == 0) 
			printf("%d\n", i);
	}
	return 0;
}
