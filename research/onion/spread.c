#include "linefile.h"
#include "net.h"
#include "base.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 2) isError("wrong argc");
	char *p;
	int N = strtol(argv[1], &p, 10);
	struct LineFile *plf = create_LineFile("/tmp/pgrk_result_0.15", 1, 1, 2, -1);
	struct LineFile *slf = create_LineFile("/tmp/spgrk_result_0.15", 1, 1, 2, -1);
	struct LineFile *lf = create_LineFile("/tmp/leadership_data", 1, 1, -1);
	struct Net *net = create_directed_Net(lf);
	char fn[100], sfn[100];
	sprintf(fn, "/tmp/spread_pgrk_%d", N);
	sprintf(sfn, "/tmp/spread_spgrk_%d", N);
	int *pgrk_step_scope = sir_spread_scope_Net(net, 0.5, plf->i2, N);
	int *spgrk_step_scope = sir_spread_scope_Net(net, 0.5, slf->i2, N);
	FILE *fp = sfopen(fn, "w");
	int i;
	for (i = 0; i < 1000; ++i) {
		//if (pgrk_step_scope[i] == 0) break;
		fprintf(fp, "%d\t%d\n", i, pgrk_step_scope[i]);
	}
	fclose(fp);
	fp = sfopen(sfn, "w");
	for (i = 0; i < 1000; ++i) {
		//if (spgrk_step_scope[i] == 0) break;
		fprintf(fp, "%d\t%d\n", i, spgrk_step_scope[i]);
	}
	fclose(fp);

	free_LineFile(plf);
	free_LineFile(slf);
	free_LineFile(lf);
	free_Net(net);
	free(pgrk_step_scope);
	free(spgrk_step_scope);
	return 0;
}
