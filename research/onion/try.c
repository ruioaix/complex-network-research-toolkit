#include "linefile.h"
#include "net.h"
#include "base.h"
#include <stdio.h>

int main(int argc, char **argv) {
	struct LineFile *lf = create_LineFile("/tmp/leadership_data", 1, 1, -1);	
	struct Net *net = create_directed_Net(lf);
	free_LineFile(lf);

	struct LineFile *clf = create_LineFile("/tmp/clean", 1, 1, -1);
	struct Net *snet = create_Net(clf);
	set_status_duplicatepairs_Net(snet);
	if (snet->duplicatepairsStatus != NS_NON_DUPPAIRS) {
		isError("xx");
	}
	//clean_duplicatepairs_Net(snet, "/tmp/clean", "/tmp/dup");
	struct LineFile *slf = similarity_CN_Net(snet);
	print_LineFile(slf, "/tmp/similarity_leadership");
	free_Net(snet);
	//struct Net *sim = create_Net(slf);
	//free_Net(sim);
	free_LineFile(slf);
	
	free_Net(net);
	return 0;
}
