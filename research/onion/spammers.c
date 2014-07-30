#include "linefile.h"
#include "net.h"

int main(int argc, char **argv) {
	struct LineFile *lf = create_LineFile("/tmp/leadership_data", 1, 1, -1);
	struct Net *net = create_directed_Net(lf);
	free_LineFile(lf);
	free_Net(net);

	return 0;
}
