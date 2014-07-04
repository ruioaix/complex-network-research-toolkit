#include "linefile.h"
#include "hash.h"
#include "base.h"
#include "net.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc != 3) isError("argc");
	struct LineFile *lf = create_LineFile(argv[1], 1, 1, -1);
	struct Net *net = create_Net(lf);
	struct HT *ht1 = create_HT(100000000);
	//struct HT *ht2 = create_HT(100000000);
	long i;
	for (i = 0; i < lf->linesNum; ++i) {
		insert_HE_HT(ht1, lf->i1[i]);	
		insert_HE_HT(ht1, lf->i2[i]);	
		//insert_HE_HT(ht2, lf->i2[i]);	
	}
	FILE *fp = sfopen(argv[2], "w");
	for (i = 0; i < net->maxId + 1; ++i) {
		fprintf(fp, "%d\t%d\n", i, get_index_HT(ht1, i));
	}
	fclose(fp);
	free_HT(ht1);
	//free_HT(ht2);
	free_LineFile(lf);

	return 0;
}


//usage: ./tool-serial dataset_filename(input) new_serialized_dataset_filename(output)
