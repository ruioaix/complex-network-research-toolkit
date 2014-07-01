#include "linefile.h"
#include "net.h"
#include <stdio.h>

int main(int argc, char **argv) {
	//struct LineFile *lf = create_LineFile("/tmp/leadership_data", 1, 1, -1);	
	struct LineFile *lf = create_LineFile("/tmp/leadership.dat", 1, 1, -1);	
	struct Net *net = create_directed_Net(lf);
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		printf("%d\t%d\t%d\n", i, net->degree[i], net->indegree[i]);
	}
	
	free_LineFile(lf);
	free_Net(net);
	return 0;
}
