#include "common.h"
#include "base.h"
#include "dataset.h"
#include "math.h"
#include "mtprand.h"
#include <stdlib.h>
#include "spath.h"

void tdpotn_argcv(int argc, char **argv, int *D_12, int *N, int *seed, int *limitN, double *theta, double *lambda) {
	printgfb();
	if (argc == 7) {
		char *p;
		*D_12 = strtol(argv[1], &p, 10);
		*N = strtol(argv[2], &p, 10);
		*seed = strtol(argv[3], &p, 10);
		*limitN = strtol(argv[4], &p, 10);
		*theta = strtod(argv[5], &p);
		*lambda = strtod(argv[6], &p);
	}
	else if (argc == 1) {
		*D_12 = 1;
		*N = 50;
		*seed = 1;
		*limitN = 5;
		*theta = 1;
		*lambda = 0;
	}
	else {
		isError("wrong args");
	}
	printgfe();
}

struct LineFile * tdpotn_lf(int D_12, int N) {
	printgfb();
	struct LineFile *file;
	if (1 == D_12) {
		file = line1d_DS(N, DS_CYCLE, DS_NON_DIRECT);
	}
	else if (2 == D_12) {
		file = lattice2d_DS(N, DS_CYCLE, DS_NON_DIRECT);
	}
	else {
		isError("wrong D_12 type");
	}
	printgfe();
	return file;
}

static void tdpotn_get_all_degree(int *sp, int N, int **alld, int *alldNum, double **p_alld, double alpha) {
	printsfb();
	int *ddis = calloc(N, sizeof(int));

	int i;
	for (i=0; i<N; ++i) {
		if (sp[i] > 0) {
			ddis[sp[i]]++;
		}
	}
	*alld = malloc(N*sizeof(int));
	*alldNum = 0;
	for (i=2; i<N; ++i) {
		if (ddis[i]) {
			(*alld)[(*alldNum)++] = i;
		}
	}
	free(ddis);

	*p_alld = malloc((*alldNum)*sizeof(double));
	for (i=0; i<*alldNum; ++i) {
		(*p_alld)[i] = pow((*alld)[i], 0-alpha);
	}
	double total = 0;
	for (i=0; i<*alldNum; ++i) {
		total += (*p_alld)[i];
	}
	for (i=0; i<*alldNum; ++i) {
		(*p_alld)[i] /= total;
	}
	for (i=1; i<*alldNum; ++i) {
		(*p_alld)[i] += (*p_alld)[i-1];
	}
	printsfe();
}

struct LineFile *tdpotn_create_air(struct Net * net, double alpha, int limitN, double theta, double lambda) {
	printgfb();
	if (theta < 0.5) isError("theta should be [0.5, +00)");

	double **weight_tmp = net->weight;
	net->weight = NULL;

	//the point 0 can get all kinds of degree in both cycle or non_cycle net.
	if (net->connectnessStatus == NS_NOTSURE) {
		set_status_connectness_Net(net);
	}
	if (net->connectnessStatus == NS_NON_CNNTNESS) {
		isError("for tdpotn, net should be fully connected");
	}
	int *sp = spath_1A_undirect_unweight_Net(net, 0);
	int *alld, alldNum; double *p_alld;
	tdpotn_get_all_degree(sp, net->maxId + 1, &alld, &alldNum, &p_alld, alpha);
	free(sp);

	int N = net->maxId + 1;
	double limit = (double)N*limitN;

	int *id1 = malloc(limitN*N*sizeof(int));
	int *id2 = malloc(limitN*N*sizeof(int));
	double *weight = malloc(limitN*N*sizeof(double));
	int *hash1 = calloc((net->maxId + 1)*3, sizeof(int));
	int *hash2 = calloc((net->maxId + 1)*2, sizeof(int));
	int *hash3 = calloc((net->maxId + 1)*3, sizeof(int));
	int idNum = 0;
	int badluck = 0;
	double totalL = 0;
	while (1) {
		double chooseSPL = get_d_MTPR();
		int splength = 0;
		int i;
		for (i=0; i<alldNum; ++i) {
			if (p_alld[i] > chooseSPL) {
				splength = alld[i];
				break;
			}
		}
		double dsplength = pow(splength, theta);
		double tmp = totalL + dsplength;
		if (tmp > limit && idNum > 0) {
			break;
		}
		else if (tmp > limit) {
			continue;
		}
		int i1 = get_i31_MTPR()%(net->maxId + 1);
		int lNum;
		int *left=NULL;
		spath_1A_step_undirect_unweight_Net(net, i1, splength, &lNum, &left);
		if (lNum > 0) {
			int random = get_i31_MTPR()%lNum;
			int i2 = left[random];
			int min = i1<i2?i1:i2;
			int max = i1>i2?i1:i2;
			if (hash1[min + 2*max] && hash2[min + max] && hash3[min*2 + max]) {
				printlp("not lucky, drop on same positon. try again.\n");
				badluck ++;
				free(left);
				continue;
			}
			hash1[min + 2*max] = 1;
			hash2[min + max] = 1;
			hash3[min*2 + max] = 1;
			id1[idNum] = i1;
			id2[idNum] = i2;
			weight[idNum] = pow(splength, lambda);
			++idNum;
			totalL += dsplength;
		}
		free(left);
	}
	free(hash1);
	free(hash2);
	free(hash3);
	free(p_alld);
	free(alld);
	printgf("badluck: %d, NumofAddedLinks: %d\n", badluck, idNum);

	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->d1 = weight;
	lf->linesNum = idNum;
	lf->memNum = limitN*N*sizeof(int);
	lf->filename = "air";
	net->weight = weight_tmp;

	printgfe();
	return lf;
}

void tdpotn_print(int D_12, int N, int seed, int limitN, double theta, double lambda, double alpha, double avesp, double coupling, double gini) {
	printf("D_12: %d\tN: %d\tseed: %d\tlimitN: %d\ttheta: %f\tlambda: %f\talpha: %f\tasp: %f\tcoupling: %f\tgini: %f\n", \
			D_12, N, seed, limitN, theta, lambda, alpha, avesp, coupling, gini);
}

/*
void tdpotn_writenettofile_ii_iid(struct iiNet *base, struct iidNet *air, char *filename, char *wfilename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, "tdpotn_writenettofile_ii_iid open error: %s\n", filename);
	FILE *fp1 = fopen(wfilename, "w");
	fileError(fp1, "tdpotn_writenettofile_ii_iid open error: %s\n", wfilename);
	int i;
	long j;
	for (i = 0; i < base->maxId + 1; ++i) {
		for (j = 0; j < base->count[i]; ++j) {
			if (i < base->edges[i][j]) {
				fprintf(fp, "%d %d\n", i, base->edges[i][j]);
				fprintf(fp1, "%f\n", 1.0);
			}
		}
	}
	for (i = 0; i < air->maxId + 1; ++i) {
		for (j = 0; j < air->count[i]; ++j) {
			if (i < air->edges[i][j]) {
				fprintf(fp, "%d %d\n", i, air->edges[i][j]);
				fprintf(fp1, "%f\n", air->d[i][j]);
			}
		}
	}
	fclose(fp);
	fclose(fp1);
}
*/
