#include "spath.h"
#include "base.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "limits.h"

/*****************standard dijkstra algorithm, used in test.*******************************/
static double ** dijkstra_setasp_undirected_weighted_Net(struct Net *net) {
	double *asp_onetime = malloc((net->maxId + 1) * (net->maxId + 1) * sizeof(double));
	int i, j;
	double **asp = smalloc((net->maxId + 1) * sizeof(double *));
	for (i = 0; i < net->maxId + 1; ++i) {
		asp[i] = asp_onetime + (net->maxId + 1)*i;
	}
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->maxId + 1; ++j) {
			asp[i][j] = INT_MAX;
		}
	}
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->degree[i]; ++j) {
			int id = net->edges[i][j];
			asp[i][id] = net->weight[i][j];
		}
	}
	return asp;
}
double *dijkstra_1A_undirected_weighted_Net(struct Net *net, int nid) {
	if (net->weight == NULL || net->directStatus == NS_DIRECTED) {
		isError("the net should be weighted and undirected.");
	}
	if (nid < 0 || nid > net->maxId) {
		isError("nid should be in [0, maxId]");
	}
	double **asp = dijkstra_setasp_undirected_weighted_Net(net);
	double *sp = smalloc((net->maxId + 1) * sizeof(double));
	char *flag = smalloc(net->maxId + 1);
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		sp[i] = asp[nid][i];
		flag[i] = 0;
	}
	flag[nid] = 1;
	sp[nid] = -1;
	int alreadyflag = 1;
	while (alreadyflag != net->maxId + 1) {
		int be = -1;
		double min = 2.0*INT_MAX;
		for (i = 0; i < net->maxId + 1; ++i) {
			if (flag[i] == 0 && min > sp[i]) {
				min = sp[i];
				be = i;
			}
		}
		flag[be] = 1;
		++alreadyflag;
		for (i = 0; i < net->maxId + 1; ++i) {
			if (flag[i] == 0 && sp[i] > sp[be] + asp[be][i]) {
				sp[i] = sp[be] + asp[be][i];
			}
		}
	}

	free(flag);
	free(asp[0]);
	free(asp);

	return sp;
}
double dijkstra_avesp_undirected_weighted_Net(struct Net *net) {
	if (net->weight == NULL || net->directStatus == NS_DIRECTED) {
		isError("the net should be weighted and undirected.");
	}
	double **asp = dijkstra_setasp_undirected_weighted_Net(net);
	double *sp = smalloc((net->maxId + 1) * sizeof(double));
	char *flag = smalloc(net->maxId + 1);

	double avesp = 0;
	int j;
	for (j = 0; j < net->maxId; ++j) {
		int i;
		for (i = 0; i < net->maxId + 1; ++i) {
			sp[i] = asp[j][i];
			flag[i] = 0;
		}
		flag[j] = 1;
		sp[j] = -1;
		int alreadyflag = 1;
		while (alreadyflag != net->maxId + 1) {
			int be = -1;
			double min = 2.0*INT_MAX;
			for (i = 0; i < net->maxId + 1; ++i) {
				if (flag[i] == 0 && min > sp[i]) {
					min = sp[i];
					be = i;
				}
			}
			flag[be] = 1;
			++alreadyflag;
			for (i = 0; i < net->maxId + 1; ++i) {
				if (flag[i] == 0 && sp[i] > sp[be] + asp[be][i]) {
					sp[i] = sp[be] + asp[be][i];
				}
			}
		}

		for (i = j+1; i < net->maxId + 1; ++i) {
			avesp += sp[i];
			//printf("%d-%d: %f\n", j, i, sp[i]);
		}
	}

	free(flag);
	free(asp[0]);
	free(asp);
	double EE = (double)(net->maxId + 1)*net->maxId/2;
	return avesp/EE;
}
/********************************************************************************************************/

/******************1A&average shortest path for undirect unweight.***************************************/
static void core_spath_1A_undirect_unweight_Net(int *sp, int **left, int **right, int *lNum, int *rNum, struct Net *net, int *STEP_END) {
	printsfb();
	int i,j;
	int STEP = 0;
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->degree[id]; ++j) {
				int neigh = net->edges[id][j];
				if (sp[neigh] == 0) {
					sp[neigh] = STEP;
					(*right)[(*rNum)++] = neigh;
				}
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
	printsfe();
}
//the returned sp's length is net->maxId + 1.
//sp[x] = 0 means originId is not connected to x.
//only sp[originId] = -1;
//for the other vertices, sp[x] is the shortest path between x and originId.
int *spath_1A_undirect_unweight_Net(struct Net *net, int originId) {
	if (originId<net->minId || originId>net->maxId) {
		isError("originId is not a valid vertex id");
	}
	if (net->directStatus != NS_UNDIRECTED || net->weight != NULL) {
		isError("the net is not an undirected or not an unweighted network");
	}
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = originId;
	sp[originId] = -1;
	int STEP_END = -1;
	core_spath_1A_undirect_unweight_Net(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
	free(left);
	free(right);
	return sp;
}
//return a list of vertex id, the shortest path between these vertices and originId is the value of step.
//*Num is the number of the vertices in the list.
//*ret is the list.
void spath_1A_step_undirect_unweight_Net(struct Net *net, int originId, int step, int *Num, int **ret) {
	if (originId<net->minId || originId>net->maxId) {
		isError("originId is not a valid vertex id");
	}
	if (net->directStatus != NS_UNDIRECTED || net->weight != NULL) {
		isError("the net is not an undirected or not an unweighted network");
	}
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = originId;
	sp[originId] = -1;
	int STEP_END = step;
	core_spath_1A_undirect_unweight_Net(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
	free(sp);
	free(right);
	*Num = lNum;
	*ret = left;
}
void spath_avesp_undirect_unweight_Net(struct Net *net, double *avesp) {
	if (net->directStatus != NS_UNDIRECTED || net->weight != NULL) {
		isError("the net is not an undirected or not an unweighted network");
	}
	if (net->connectnessStatus != NS_CNNTNESS ) {
		isError("this method can only calculate the avesp of a net which is fully connected.");
	}
	int *sp = malloc((net->maxId + 1)*sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;

	int i,j;
	int STEP_END = -1;
	*avesp = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		lNum = 1;
		left[0] = i;
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
		}
		sp[i] = -1;
		core_spath_1A_undirect_unweight_Net(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
		for (j=i+1; j<net->maxId + 1; ++j) {
			*avesp += sp[j];
		}
	}

	free(left);
	free(right);
	free(sp);
	double EE = (double)(net->maxId + 1)*(net->maxId)/2.0;
	*avesp /= EE;
}
/********************************************************************************************************/

/*******get avesp and coupling between two undirected&unweighted net: base and air.**********************/
//to find coupling of two net: base and air.
static void core_spath_avesp_coupling_undirect_unweight_Net(int *sp, char *stage, int **left, int **right, int *lNum, int *rNum, struct Net *base, struct Net *air, double *spa, double *spb, double *spab) {
	int i;
	int j;
	int STEP = 1;
	memset(stage, 0 ,sizeof(char)*(base->maxId + 1));
	while (*lNum) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<base->degree[id]; ++j) {
				int neigh = base->edges[id][j];
				if (sp[neigh] == 0) {
					spab[neigh] += spab[id];
					spab[neigh] += spa[id];
					spb[neigh] += spb[id];
					if (stage[neigh] == 0) {
						stage[neigh] = 1;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->degree[id]; ++j) {
					int neigh = air->edges[id][j];
					if (sp[neigh] == 0) {
						spab[neigh] += spab[id];
						spab[neigh] += spb[id];
						spa[neigh] += spa[id];
						if (stage[neigh] == 0) {
							stage[neigh] = 1;
							(*right)[(*rNum)++] = neigh;
						}
					}
				}
			}
		}
		for (i = 0; i < *rNum; ++i) {
			sp[(*right)[i]] = STEP;
			stage[(*right)[i]] = 0;
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
//the vertices in air is a subset of the vertices in base.
void spath_avesp_coupling_undirect_unweight_Net(struct Net *base, struct Net *air, double *avesp, double *coupling) {
	if (base->directStatus != NS_UNDIRECTED || base->weight != NULL) {
		isError("the base net is not undirected or not unweighted.");
	}
	if (air->directStatus != NS_UNDIRECTED || air->weight != NULL) {
		isError("the air net is not undirected or not unweighted.");
	}
	int *sp = smalloc((base->maxId + 1)*sizeof(int));
	int *left = smalloc((base->maxId + 1)*sizeof(int));
	int *right = smalloc((base->maxId + 1)*sizeof(int));
	double *spa = smalloc((base->maxId + 1) * sizeof(double));
	double *spb = smalloc((base->maxId + 1) * sizeof(double));
	double *spab = smalloc((base->maxId + 1) * sizeof(double));
	char *stage = smalloc((base->maxId + 1) * sizeof(char));
	int lNum, rNum;

	int i,j;
	int k;
	*avesp = 0;
	*coupling =0;
	for (i=0; i<base->maxId; ++i) {
		for (j=0; j<base->maxId + 1; ++j) {
			sp[j] = 0;
			spa[j] = 0;
			spb[j] = 0;
			spab[j] = 0;
		}
		sp[i] = -1;
		lNum = 0;
		for (k = 0; k < base->degree[i]; ++k) {
			int to = base->edges[i][k];
			left[lNum++] = to;
			sp[to] = 1;
			++spb[to];
		}
		if (i < air->maxId + 1) {
			for (k = 0; k < air->degree[i]; ++k) {
				int to = air->edges[i][k];
				left[lNum++] = to;
				sp[to] = 1;
				++spa[to];
			}
		}
		core_spath_avesp_coupling_undirect_unweight_Net(sp, stage, &left, &right, &lNum, &rNum, base, air, spa, spb, spab);
		for (j = i+1; j < base->maxId + 1; ++j) {
			*avesp += sp[j];
			*coupling += spab[j]/(spa[j] + spb[j] + spab[j]);
		}
	}

	free(left); free(right);
	free(spa); free(spb); free(spab);
	free(sp);
	free(stage);
	double ij = (double)(base->maxId + 1)*base->maxId/2;
	*avesp /= ij;
	*coupling /= ij;
}
/********************************************************************************************************/

/***********get avesp of two undirected&1up_weighted net: base and air*****************/
//the vertices in air is a subset of the vertices in base.
//this can only handle the net in which the weight of all edges are not less than 1.0, so called 1up.
//both base and air are requited to satisfy this prerequisite.
static void core_spath_avesp_undirect_1upweight_Net(double *sp, signed char *gs, int *upCSlist, signed char *ups, int upCSlistNum, int **left, int **right, int *lNum, int *rNum, struct Net *base, struct Net *air) {
	int i;
	int j;
	int STEP = 1;
	while (*lNum) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<base->degree[id]; ++j) {
				int neigh = base->edges[id][j];
				sp[neigh] = dmin(sp[neigh], sp[id] + base->weight[id][j]);
				if (sp[neigh] < STEP + 1.0 && gs[neigh] == 0) {
					gs[neigh] = 1;
					(*right)[(*rNum)++] = neigh;
				}
				else if (sp[neigh] >= STEP + 1.0 && gs[neigh] == 0 && ups[neigh] == 0) {
					upCSlist[upCSlistNum++] = neigh;
					ups[neigh] = 1;
				}

			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->degree[id]; ++j) {
					int neigh = air->edges[id][j];
					sp[neigh] = dmin(sp[neigh], sp[id] + air->weight[id][j]);
					if (sp[neigh] < STEP + 1.0 && gs[neigh] == 0) {
						gs[neigh] = 1;
						(*right)[(*rNum)++] = neigh;
					}
					else if (sp[neigh] >= STEP + 1.0 && gs[neigh] == 0 && ups[neigh] == 0) {
						upCSlist[upCSlistNum++] = neigh;
						ups[neigh] = 1;
					}
				}
			}
		}
		for (i = 0; i <upCSlistNum; ) {
			int id = upCSlist[i];
			if (gs[id] == 1) {
				ups[id] = 0;
				upCSlist[i] = upCSlist[--upCSlistNum];
			}
			else if(sp[id]<STEP+1) {
				gs[id] = 1;
				(*right)[(*rNum)++] = id;
				upCSlist[i] = upCSlist[--upCSlistNum];
				ups[id] = 0;
			}
			else {
				++i;
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
void spath_avesp_undirect_1upweight_Net(struct Net *base, struct Net *air, double *avesp) {
	if (base->directStatus != NS_UNDIRECTED || base->weight == NULL) {
		isError("the base net should be undirected and weighted.");
	}
	if (air->directStatus != NS_UNDIRECTED || air->weight == NULL) {
		isError("the air net should be undirected and weighted.");
	}
	double *sp = smalloc((base->maxId + 1)*sizeof(double));
	int *left = smalloc((base->maxId + 1)*sizeof(int));
	int *right = smalloc((base->maxId + 1)*sizeof(int));
	int *upCSlist = smalloc((base->maxId + 1)*sizeof(int));
	signed char *gs = smalloc((base->maxId + 1) * sizeof(signed char));
	signed char *ups = scalloc((base->maxId + 1) , sizeof(signed char));
	int lNum, rNum, upCSlistNum;

	int i,j,k;
	*avesp = 0;
	for (i=0; i<base->maxId; ++i) {
		for (j=0; j<base->maxId + 1; ++j) {
			sp[j] = INT_MAX;
			gs[j] = 0;
			//ups[j] = 0;
		}
		gs[i] = sp[i] = -1;
		lNum = 0;
		upCSlistNum = 0;
		for (k = 0; k < base->degree[i]; ++k) {
			int to = base->edges[i][k];
			left[lNum++] = to;
			sp[to] = 1;
			gs[to] = 1;
		}
		if (i < air->maxId + 1) {
			for (k = 0; k < air->degree[i]; ++k) {
				int to = air->edges[i][k];
				sp[to] = air->weight[i][k];
				if (sp[to] < 2.0 && gs[to] == 0) {
					left[lNum++] = to;
					gs[to] = 1;
				}
				else {
					upCSlist[upCSlistNum++] = to;
					ups[to] = 1;
				}
			}
		}
		core_spath_avesp_undirect_1upweight_Net(sp, gs, upCSlist, ups, upCSlistNum, &left, &right, &lNum, &rNum, base, air);
		for (j = i+1; j < base->maxId + 1; ++j) {
			//printf("gs:%d\t%d\t%f\n", j, gs[j],sp[j]);
			*avesp += sp[j];
			//printf("%d-%d: %f\n", i, j, sp[j]);
		}
		//exit(0);
	}

	free(left); free(right);
	free(upCSlist);
	free(sp); free(gs); free(ups);
	double ij = (double)(base->maxId + 1)*base->maxId/2;
	*avesp /= ij;
}
/*


//this spath02 is FW algorithm for unweighted and undirected net.
int **spath02_AA_iiNet(struct iiNet *net) {
//FILE *fp = fopen("fw", "w");
int **apsp = malloc((net->maxId + 1)*sizeof(void *));
int i,j,k;
for (i=0; i<net->maxId + 1; ++i) {
apsp[i] = calloc((net->maxId + 1), sizeof(int));
}
for (i=0; i<net->maxId + 1; ++i) {
for (j=0; j<net->count[i]; ++j) {
int id = net->edges[i][j];
apsp[i][id] = 1;
}
}
for (i=0; i<net->maxId + 1; ++i) {
for (j=0; j<net->maxId + 1; ++j) {
if (!apsp[i][j]) {
apsp[i][j] = 2*(net->maxId + 1);
}
else if (i<j){
//fprintf(fp, "%d\t%d\t%d\n", i, j, apsp[i][j]);
}
if (i==j) {
apsp[i][j] = 0;
}
//fprintf(fp, "%d\t%d\t%d\n", i, j, apsp[i][j]);
}
}
for (k=0; k<net->maxId + 1; ++k) {
printf("%.2f%%\r", k*100.0/net->maxId);fflush(stdout);
for (i=0; i<net->maxId + 1; ++i) {
for (j=0; j<net->maxId + 1; ++j) {
int havek = apsp[i][k] + apsp[k][j];
apsp[i][j] = havek < apsp[i][j] ? havek : apsp[i][j];
}
}
//printf("%d\t%d\t%d\n", apsp[7][0], apsp[8][8], apsp[7][8]);
}
printf("\n");fflush(stdout);
return apsp;
}

#define EPS 0.0000001
//this spath03 is for unweighted_undirected base and weighted_undirected air network.
//the weight of each link in air network is >= 1.0.
static void spath03_core_Net(double *sp, char *gs, int *blist, int *nlist, int bNum, int nNum, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iidNet *air, int *STEP_END) {
int i,j;
int STEP = 1;
while (*lNum && STEP != *STEP_END) {
 *rNum = 0;
 ++STEP;

 for (i = 0; i < nNum; ++i) {
 int id = nlist[i];
 if (gs[id] != 3) {
 if (i != nNum - 1) {
 nlist[i] = nlist[--nNum];
 --i;
 }
 else {
 --nNum;
 }
 }
 else if(sp[id] < STEP + 1 - EPS && sp[id] >= STEP) {
 blist[bNum++] = id;
 gs[id] = 2;
 if (i != nNum - 1) {
 nlist[i] = nlist[--nNum];
--i;
}
else {
	--nNum;
}
}
else if (sp[id] >= STEP + 1 - EPS && sp[id] < STEP + 1 ) {
	gs[id] = 3;
	sp[id] = STEP + 1;
}
else if (sp[id] >= STEP + 1) {
}
else {
	printf("%d\t%f\t%d\n", gs[id], sp[id], STEP);
	isError("spath03_core_Net nlist");
}
}

for (i=0; i<*lNum; ++i) {
	int id = (*left)[i];
	for (j=0; j<net->count[id]; ++j) {
		int neigh = net->edges[id][j];
		//if gs = 1, neigh is already defined.
		if (gs[neigh] != 1) {
			sp[neigh] = dmin(sp[id] + 1, sp[neigh]);
			if (sp[neigh] == STEP) {
				(*right)[(*rNum)++] = neigh;
				gs[neigh] = 1;
			}
			else if (sp[neigh] < STEP + 1 && sp[neigh] > STEP) {
				if (gs[neigh] != 2) {
					gs[neigh] = 2;
					blist[bNum++] = neigh;
				}
			}
			else {
				printf("err: %d\t%.17f\t%.17f\t%d\n", STEP, sp[id], sp[neigh], gs[neigh]);
				isError("spath03_core_Net, tt");
			}
		}
	}
	if(id < air->maxId + 1) {
		for (j=0; j<air->count[id]; ++j) {
			int neigh = air->edges[id][j];
			double airl = air->d[id][j];
			if (gs[neigh] != 1) {
				sp[neigh] = dmin(sp[id] + airl, sp[neigh]);
				if (sp[neigh] == STEP) {
					(*right)[(*rNum)++] = neigh;
					gs[neigh] = 1;
				}
				else if (sp[neigh] < STEP + 1 && sp[neigh] > STEP) {
					if (gs[neigh] != 2) {
						gs[neigh] = 2;
						blist[bNum++] = neigh;
					}
				}
				else if (sp[neigh] >= STEP + 1) {
					if (gs[neigh] == 0) {
						nlist[nNum++] = neigh;
						gs[neigh] = 3;
					}
					else if (gs[neigh] == 3) {
					}
					else {
						isError("spath03_core_Net x");
					}
				}
				else {
					printf("err: %d\t%d\t%d\t%f\n", STEP, id, neigh, sp[neigh]);
					isError("spath03_core_Net 2");
				}
			}
		}
	}
}

for (j = 0; j < bNum; ++j) {
	int id = blist[j];	
	if (gs[id] == 2) {
		(*right)[(*rNum)++] = id;
		gs[id] = 1;
	}
	else if (gs[id] == 1) {
	}
	else {
		printf("err: %d\t%d\t%f\n", gs[id], STEP, sp[id]);
		isError("spath03_core_Net 3");
	}
}
bNum = 0;

int *tmp = *left;
*left = *right;
*right = tmp;
*lNum = *rNum;
}
}
void avesp_spath03_Net(struct iiNet *net, struct iidNet *air, double *avesp) {
	double *sp = malloc((net->maxId + 1)*sizeof(double));
	assert(sp != NULL);
	int *left = malloc((net->maxId + 1)*sizeof(int));
	assert(left != NULL);
	int *right = malloc((net->maxId + 1)*sizeof(int));
	assert(right != NULL);
	int lNum, rNum;

	char *gs = malloc((net->maxId + 1)*sizeof(char));
	int *blist = malloc((net->maxId + 1)*sizeof(int));
	int *nlist = malloc((net->maxId + 1)*sizeof(int)); int bNum, nNum;

	*avesp = 0;
	int cc = 0;

	int i,j;
	int STEP_END = -1;
	//FILE *fp = fopen("1", "w"); fileError(fp, "get_avesp_Net");
	for (i=0; i<net->maxId + 1; ++i) {
		//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = INT_MAX;
			gs[j] = 0;
		}
		sp[i] = -1;
		gs[i] = 1;
		bNum = nNum = lNum = 0;
		for (j = 0; j < net->count[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1.0;
			gs[to] = 1;
		}
		if (i < air->maxId + 1) {
			for (j = 0; j < air->count[i]; ++j) {
				int to = air->edges[i][j];
				double airl = air->d[i][j];
				if (gs[to] == 0) {
					if (airl < 2) {
						sp[to] = airl;
						gs[to] = 1;
						left[lNum++] = to;
					}
					else {
						sp[to] = airl;
						gs[to] = 3;
						nlist[nNum++] = to;
					}
				}
			}
		}
		spath03_core_Net(sp, gs, blist, nlist, bNum, nNum, &left, &right, &lNum, &rNum, net, air, &STEP_END);
		//int co = 0;
		//double avess = 0;
		for (j = 0; j < net->maxId + 1; ++j) {
			//fprintf(fp, "sp: %d\t%d\t%f\n", i, j, sp[j]);
			//if (i==0 && sp[j]>0) {
			//	printf("%d, %f\n", j, sp[j]);
			//}
			if (sp[j] > 0) {
				*avesp += sp[j];
				++cc;
				//avess += sp[j];
				//++co;
			}
		}
		//if (i==0) printf("i: %d, co: %d, ave: %f, avesp: %f\n", i, co, avess, avess/co);
	}
	//fclose(fp);

	free(left);
	free(right);
	free(sp);
	free(gs);
	free(blist);
	free(nlist);
	double all = (double)(net->maxId + 1)*net->maxId;
	*avesp = (*avesp)/all;
	//printf("check: %d\t%f\n", cc, all);
}

//this spath04 is for unweighted and undirected net.
//to find how many shortest pathes use only base or only air or both.
static void spath04_core_iiNet(int *sp, char *use, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iiNet *air, int *STEP_END) {
	int i,j;
	int STEP = 1;
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			//printf("id:%d\n", id);
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (!sp[neigh]) {
					//pn q xiang yu net
					if (0 == use[id]) {
						if (-1 == use[neigh]) {
							use[neigh] = 3;
						}
						else if (4 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
					else if (1 == use[id]) {
						if (-1 == use[neigh]) {
							use[neigh] = 4;
						}
						else if (3 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
					else if (2 == use[id]) {
						use[neigh] = 2;
						sp[neigh] = STEP;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->count[id]; ++j) {
					int neigh = air->edges[id][j];
					if (!sp[neigh]) {
						if (-1 == use[neigh]) {
							use[neigh] = 4;
						}
						else if (3 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
				}
			}
		}
		//static int kk = 0;
		for (j = 0; j < net->maxId + 1; ++j) {
			if (3 == use[j] || 4 == use[j]) {
				use[j] -= 3;
				sp[j] = STEP;
				(*right)[(*rNum)++] = j;
			}
			//printf("%d\t%d\n", j, use[j]);fflush(stdout);
		}
		//printf("******************************************\n");
		//if (kk++ == 4) exit(0);
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
void useRate_spath04_iiNet(struct iiNet *net, struct iiNet *air, double *useRate, double *sameRate, double *cleanRate, double *avesp) {
	int *sp = malloc((net->maxId + 1)*sizeof(int));
	assert(sp != NULL);
	int *left = malloc((net->maxId + 1)*sizeof(int));
	assert(left != NULL);
	int *right = malloc((net->maxId + 1)*sizeof(int));
	assert(right != NULL);
	char *use = malloc((net->maxId + 1) * sizeof(char));
	assert(use != NULL);
	int lNum, rNum;

	int i,j;
	int STEP_END = -1;
	*useRate = 0;
	*sameRate = 0;
	*cleanRate = 0;
	*avesp = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
			use[j] = -1;
		}
		sp[i] = -1;
		lNum = 0;
		for (j = 0; j < net->count[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1;
			use[to] = 0;
		}
		if (i < air->maxId + 1) {
			for (j = 0; j < air->count[i]; ++j) {
				int to = air->edges[i][j];
				left[lNum++] = to;
				sp[to] = 1;
				use[to] = 1;
			}
		}
		spath04_core_iiNet(sp, use, &left, &right, &lNum, &rNum, net, air, &STEP_END);
		sp[i] = 0;
		for (j = 0; j < net->maxId + 1; ++j) {
			if (1 == use[j]) {
				++(*useRate);
			}
			else if (2 == use[j]) {
				++(*sameRate);
			}
			else if (0 == use[j]) {
				++(*cleanRate);
			}
			*avesp += sp[j];
		}
	}

	free(left);
	free(right);
	free(sp);
	free(use);
	double all = (double)(net->maxId + 1)*net->maxId;
	*useRate /= all;
	*sameRate /= all;
	*cleanRate /= all;
	*avesp /= all;
}

*/

//this spath06 is for unweighted_undirected base and weighted_undirected air network.
//to find gini of two net: base and air.
static void set_spAspall_spath_avesp_gini_undirect_unweight_Net(int *sp, char *stage,  int **left, int **right, int *lNum, int *rNum, struct Net *net, double *spall) {
	int i,j;
	int STEP = 1;
	memset(stage, 0 ,sizeof(char)*(net->maxId + 1));
	while (*lNum) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->degree[id]; ++j) {
				int neigh = net->edges[id][j];
				if (sp[neigh] == 0) {
					spall[neigh] += spall[id];
					if (stage[neigh] == 0) {
						stage[neigh] = 1;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
		}
		for (j = 0; j < *rNum; ++j) {
			sp[(*right)[j]] = STEP;
			stage[(*right)[j]] = 0;
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
static void set_edgesAttr_spath_avesp_gini_undirect_unweight_Net(int source, int *sp, char *stage, int **left, int **right, int *lNum, int *rNum, struct Net *net, double *spall) {
	int i,j,k;
	*rNum = 0;
	memset(stage, 0 ,sizeof(char)*(net->maxId + 1));
	for (i = 0; i < net->maxId + 1; ++i) {
		int step = sp[i];
		double aij = spall[i];
		if (step == 1) {
			net->edgesAttr[i][net->edgesMatrix.pp[i][source]] += 1;
			net->edgesAttr[source][net->edgesMatrix.pp[source][i]] += 1;
		}
		else if (step > 1) {
			*lNum = 0;
			(*left)[(*lNum)++] = i;
			while (step != 1) {
				step--;
				*rNum = 0;

				for (k=0; k<*lNum; ++k) {
					int id = (*left)[k];
					for (j=0; j<net->degree[id]; ++j) {
						int neigh = net->edges[id][j];
						if (sp[neigh] == step) {
							net->edgesAttr[id][net->edgesMatrix.pp[id][neigh]] += spall[neigh]/aij;
							net->edgesAttr[neigh][net->edgesMatrix.pp[neigh][id]] += spall[neigh]/aij;
							if (stage[neigh] == 0) {
								stage[neigh] = 1;
								(*right)[(*rNum)++] = neigh;
							}
						}
					}
				}

				for (k = 0; k < *rNum; ++k) {
					int id = (*right)[k];
					stage[id] = 0;	
				}

				int *tmp = *left;
				*left = *right;
				*right = tmp;
				*lNum = *rNum;
			}
			for (k=0; k<*lNum; ++k) {
				int id = (*left)[k];
				net->edgesAttr[id][net->edgesMatrix.pp[id][source]] += 1/aij;
				net->edgesAttr[source][net->edgesMatrix.pp[source][id]] += 1/aij;
			}
		}
	}
}

static double calculate_gini_spath_avesp_gini_undirect_unweight_Net(struct Net *net) {
	int i,j;
	int m,n;
	double diff = 0.0;
	double total = 0.0;
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->degree[i]; ++j) {
			if (i<net->edges[i][j]) {
				double x1 = net->edgesAttr[i][j];	
				total += x1;
				for (m = 0; m < net->maxId + 1; ++m) {
					for (n = 0; n < net->degree[m]; ++n) {
						if (m < net->edges[m][n]) {
							double x2 = net->edgesAttr[m][n];
							diff += fabs(x1 - x2);
						}
					}
				}
			}
		}
	}
	//double Tij=(double)(net->maxId + 1)*net->maxId/2;
	double E = (double)net->edgesNum;
	double G = diff/(2*E*total/**Tij*/);
	return G;
}
void spath_avesp_gini_undirect_unweight_Net(struct Net *net, double *avesp, double *gini) {
	int *sp = smalloc((net->maxId + 1)*sizeof(int));
	int *left = smalloc((net->maxId + 1)*sizeof(int));
	int *right = smalloc((net->maxId + 1)*sizeof(int));
	double *spall = smalloc((net->maxId + 1) * sizeof(double));
	char *stage = smalloc((net->maxId + 1) * sizeof(char));
	int lNum, rNum;

	*avesp = 0;
	*gini = 0;
	int i,j;
	for (i=0; i<net->maxId; ++i) {
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
			spall[j] = 0;
		}
		sp[i] = -1;
		lNum = 0;
		for (j = 0; j < net->degree[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1;
			++spall[to];
		}
		set_spAspall_spath_avesp_gini_undirect_unweight_Net(sp, stage, &left, &right, &lNum, &rNum, net, spall);
		for (j = i+1; j < net->maxId + 1; ++j) {
			*avesp += sp[j];
		}
		set_edgesAttr_spath_avesp_gini_undirect_unweight_Net(i, sp, stage, &left, &right, &lNum, &rNum, net, spall);
	}

	free(left);
	free(right);
	free(sp);
	free(spall);
	free(stage);
	*avesp /= (double)(net->maxId + 1)*net->maxId/2;
	*gini = calculate_gini_spath_avesp_gini_undirect_unweight_Net(net);
}
