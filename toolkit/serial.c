#include "linefile.h"
#include "hash.h"
#include "base.h"
#include "net.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc != 3) isError("argc");
	struct LineFile *lf = create_LineFile(argv[1], 1, 1, -1);
	struct Net *net = create_directed_Net(lf);
	struct HT *ht1 = create_HT(100000000);
	long i;
	for (i = 0; i < lf->linesNum; ++i) {
		insert_HE_HT(ht1, lf->i1[i]);	
		insert_HE_HT(ht1, lf->i2[i]);	
	}

	FILE *fp = sfopen("/tmp/originid_nowid", "w");
	int j;
	for (j = 0; j < net->maxId + 1; ++j) {
		fprintf(fp, "%d\t%d\n", j, get_index_HT(ht1, j));
	}
	fclose(fp);

	fp = sfopen(argv[2], "w");
	for (i = 0; i < lf->linesNum; ++i) {
		fprintf(fp, "%d\t%d\n", get_index_HT(ht1, lf->i1[i]), get_index_HT(ht1, lf->i2[i]));
	}
	fclose(fp);

	free_HT(ht1);
	free_LineFile(lf);
	free_Net(net);

	return 0;
}


//usage: ./tool-serial dataset_filename(input) new_serialized_dataset_filename(output)
