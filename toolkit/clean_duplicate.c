#include "linefile.h"
#include "net.h"

int main(int argc, char **argv) {
	struct LineFile *lf = create_LineFile("/tmp/pbdata", 1, 1, -1);
	struct Net *net = create_directed_Net(lf);
	clean_duplicatepairs_Net(net, "/tmp/cleanpbdata", "/tmp/duppbdata");
	return 0;
}
