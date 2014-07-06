#include "linefile.h"
#include "net.h"
#include "spath.h"
#include "base.h"

int main(int argc, char **argv) {
	struct LineFile *lf = create_LineFile("/home/rui/data/delicious_leadership/leadership_data", 1, 1, -1);
	struct Net *net = create_Net(lf);
	delete_duplicatepairs_Net(net);
	set_status_connectness_Net(net);
	if (net->connectnessStatus != NS_CNNTNESS) isError("not connectness");
	double avesp;
	spath_avesp_undirect_unweight_Net(net, &avesp);
	free_LineFile(lf);
	free_Net(net);
	return 0;
}
