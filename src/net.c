#include "base.h"
#include "net.h"
#include "mtprand.h"
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void free_Net(struct Net *net) {
	print2l("%s =>> begin......\n", __func__);
	//free degree&indegree.
	free(net->degree);
	free(net->indegree);

	//free edges&inedges.
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		free(net->edges[i]);
		free(net->inedges[i]);
	}
	free(net->edges);
	free(net->inedges);
	
	//free weight
	if (net->weight.sign == NS_VALID) {
		for(i=0; i<net->maxId+1; ++i) {
			free(net->weight.pp[i]);
		}
		free(net->weight.pp);
	}

	//free net
	free(net);
	print2l("%s =>> ......end.\n", __func__);
}

static void create_Net_basic_lf_check(struct LineFile *lf) {
	if (lf->i1 == NULL || lf->i2 == NULL || lf->linesNum == 0) {
		isError("%s =>> i1 or/and i2 is NULL, or/and linesNum is 0.\n", __func__);
	}
}

static void create_Net_maxId_minId(struct LineFile *lf, int *maxId, int *minId) {
	*maxId = lf->i1[0];
	*minId = lf->i1[0];
	long i;
	for(i=0; i<lf->linesNum; ++i) {
		*maxId = *maxId > lf->i1[i] ? *maxId : lf->i1[i];	
		*maxId = *maxId > lf->i2[i] ? *maxId : lf->i2[i];	
		*minId = *minId < lf->i1[i] ? *minId : lf->i1[i];
		*minId = *minId < lf->i2[i] ? *minId : lf->i2[i];
	}
}

static void create_Net_undirect_degree_idNum(struct LineFile *lf, int maxId, long **degree, int *idNum) {
	*degree=calloc(maxId+1, sizeof(long)); assert(degree!=NULL);

	long i;
	for(i=0; i<lf->linesNum; ++i) {
		++(*degree)[lf->i1[i]];
		++(*degree)[lf->i2[i]];
	}
	int j;
	*idNum = 0;
	for(j=0; j<maxId+1; ++j) {
		if ((*degree)[j]>0) {
			++(*idNum);
		}
	}
}

static void create_Net_direct_degree_idNum(struct LineFile *lf, int maxId, long **degree, long **indegree ,int *idNum) {
	*degree=calloc(maxId+1, sizeof(long)); assert(degree!=NULL);
	*indegree=calloc(maxId+1, sizeof(long)); assert(indegree!=NULL);

	long i;
	for(i=0; i<lf->linesNum; ++i) {
		++degree[lf->i1[i]];
		++indegree[lf->i2[i]];
	}
	int j;
	*idNum = 0;
	for(j=0; j<maxId+1; ++j) {
		if (degree[j]>0 || indegree[j]>0) {
			++(*idNum);
		}
	}
}

static void create_Net_directed_edges(struct LineFile *lf, int maxId, long *degree, long *indegree, int ***edges, int ***inedges, long *degreeMax, long *degreeMin, long *indegreeMax, long *indegreeMin) {
	*edges=malloc((maxId+1)*sizeof(void *)); assert(*edges!=NULL);
	*inedges=malloc((maxId+1)*sizeof(void *)); assert(*inedges!=NULL);
	int i;
	long dmi=LONG_MAX, idmi=LONG_MAX; 
	long dma=0, idma=0;
	for(i=0; i<maxId+1; ++i) {
		if (degree[i]!=0) {
			dma = dma > degree[i] ? dma : degree[i];
			dmi = dmi < degree[i] ? dmi : degree[i];
			(*edges)[i]=malloc(degree[i]*sizeof(int));
			assert((*edges)[i]!=NULL);
		}
		else (*edges)[i] = NULL;
		
		if (indegree[i]!=0) {
			idma = idma > indegree[i] ? idma : indegree[i];
			idmi = idmi < indegree[i] ? idmi : indegree[i];
			(*inedges)[i]=malloc(indegree[i]*sizeof(int));
			assert((*inedges)[i]!=NULL);
		}
		else (*inedges)[i] = NULL;
	}
	*degreeMax = dma;
	*degreeMin = dmi;
	*indegreeMax = idma;
	*indegreeMin = idmi;

	long *out_count=calloc(maxId+1, sizeof(long)); assert(out_count!=NULL);
	long *in_count=calloc(maxId+1, sizeof(long)); assert(in_count!=NULL);
	long j;
	for(j=0; j<lf->linesNum; ++j) {
		int i1 =lf->i1[j];
		int i2 =lf->i2[j];
		(*edges)[i1][out_count[i1]++] = i2;
		(*inedges)[i2][in_count[i2]++] = i1;
	}
	free(out_count);
	free(in_count);
}

static void create_Net_undirected_edges(struct LineFile *lf, int maxId, long *degree, int ***edges, long *degreeMax, long *degreeMin) {
	*edges=malloc((maxId+1)*sizeof(void *)); assert(*edges!=NULL);
	int i;
	long dmi=LONG_MAX;
	long dma=0;
	for(i=0; i<maxId+1; ++i) {
		if (degree[i]!=0) {
			dma = dma > degree[i] ? dma : degree[i];
			dmi = dmi < degree[i] ? dmi : degree[i];
			(*edges)[i]=malloc(degree[i]*sizeof(int));
			assert((*edges)[i]!=NULL);
		}
		else (*edges)[i] = NULL;
	}
	*degreeMax = dma;
	*degreeMin = dmi;

	long *count=calloc(maxId+1, sizeof(long)); assert(count!=NULL);
	long j;
	for(j=0; j<lf->linesNum; ++j) {
		int i1 =lf->i1[j];
		int i2 =lf->i2[j];
		(*edges)[i1][count[i1]++] = i2;
		(*edges)[i2][count[i2]++] = i1;
	}
	free(count);
}

static struct Net *create_Net_init(long edgesNum, int maxId, int minId, int idNum, long *degree, long *indegree, int **edges, int **inedges, long degreeMax, long degreeMin, long indegreeMax, long indegreeMin, enum NET_STATUS directStatus, enum NET_STATUS connectnessStatus, enum NET_STATUS duplicatepairsStatus) { struct Net *net = malloc(sizeof(struct Net));
	assert(net != NULL);
	net->edgesNum=edgesNum;
	net->maxId=maxId;
	net->minId=minId;
	net->idNum=idNum;
	net->degree=degree;
	net->indegree=indegree;
	net->edges=edges;
	net->inedges = inedges;

	net->degreeMax.sign = NS_VALID;
	net->degreeMax.value = degreeMax;
	net->degreeMin.sign = NS_VALID;
	net->degreeMin.value = degreeMin;
	net->indegreeMax.sign = NS_VALID;
	net->indegreeMax.value = indegreeMax;
	net->indegreeMin.sign = NS_VALID;
	net->indegreeMin.value = indegreeMin;

	net->directStatus = directStatus;
	net->connectnessStatus = connectnessStatus;
	net->duplicatepairsStatus = duplicatepairsStatus;
	return net;
}

struct Net *create_directed_Net(struct LineFile * lf) {
	print2l("%s =>> begin......\n", __func__);
	create_Net_basic_lf_check(lf);

	int maxId, minId;
	create_Net_maxId_minId(lf, &maxId, &minId);
	print3l("%s =>> get maxId: %d, minId: %d\n", __func__, maxId, minId);

	int idNum;
	long *degree, *indegree;
	create_Net_direct_degree_idNum(lf, maxId, &degree, &indegree, &idNum);
	print3l("%s =>> get degree, idNum: %d.\n", __func__, idNum);

	long degreeMax, degreeMin;
	long indegreeMax, indegreeMin;
	int **edges, **inedges;
	create_Net_directed_edges(lf, maxId, degree, indegree, &edges, &inedges, &degreeMax, &degreeMin, &indegreeMax, &indegreeMin);
	print3l("%s =>> get degreeMax: %ld, degreeMin: %ld, indegreeMax: %ld, indegreeMin: %ld; and alloc memory and fill edges.\n", __func__, degreeMax, degreeMin, indegreeMax, indegreeMin);
	struct Net *net = create_Net_init(lf->linesNum, maxId, minId, idNum, degree, indegree, edges, inedges, degreeMax, degreeMin, indegreeMax, indegreeMin, NS_DIRECTED, NS_NOTSURE_CNNTNESS, NS_NOTSURE_DUPPAIRS); 

	print3l("%s =>> return iiNet.\n", __func__);
	print2l("%s =>> ......end.\n", __func__);
	return net;
}

struct Net *create_undirected_Net(struct LineFile * lf) {
	print2l("%s =>> begin......\n", __func__);
	create_Net_basic_lf_check(lf);

	int maxId, minId;
	create_Net_maxId_minId(lf, &maxId, &minId);
	print3l("%s =>> get maxId: %d, minId: %d\n", __func__, maxId, minId);

	int idNum;
	long *degree;
	create_Net_undirect_degree_idNum(lf, maxId, &degree, &idNum);
	print3l("%s =>> get degree, idNum: %d.\n", __func__, idNum);

	long degreeMax, degreeMin;
	int **edges;
	create_Net_undirected_edges(lf, maxId, degree, &edges, &degreeMax, &degreeMin);
	print3l("%s =>> get degreeMax: %ld, degreeMin: %ld; and alloc memory and fill edges.\n", __func__, degreeMax, degreeMin);

	struct Net *net = create_Net_init(lf->linesNum, maxId, minId, idNum, degree, degree, edges, edges, degreeMax, degreeMin, degreeMax, degreeMin, NS_UNDIRECTED, NS_NOTSURE_CNNTNESS, NS_NOTSURE_DUPPAIRS); 
	print3l("%s =>> return iiNet.\n", __func__);
	print2l("%s =>> ......end.\n", __func__);
	return net;
}

/*
void print_iiNet(struct iiNet *net, char *filename) {
	print2l("%s =>> begin......\n", __func__);
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_iiNet");
	int i;
	long j;
	for (i=0; i<net->maxId + 1; ++i) {
		if (net->count[i] > 0) {
			for (j=0; j<net->count[i]; ++j) {
				if (i < net->edges[i][j]) {
					fprintf(fp, "%d\t%d\n", i, net->edges[i][j]);
				}
			}
		}
	}
	fclose(fp);
	print2l("%s =>> ......end.\n", __func__);
}

int delete_node_iiNet(struct iiNet *net, int nid) {
	print2l("%s =>> begin......\n", __func__);
	long i, j;
	if (nid < 0 || nid > net->maxId || net->count[nid] == 0) {
		print2l("%s =>> ......end.\n", __func__);
		return 0;
	}
	net->edgesNum -= net->count[nid];
	for (i = 0; i < net->count[nid]; ++i) {
		int neigh = net->edges[nid][i];
		long count = net->count[neigh];
		for (j = 0; j < net->count[neigh]; ++j) {
			if (net->edges[neigh][j] == nid) {
				net->edges[neigh][j] = net->edges[neigh][--(net->count[neigh])];
				break;
			}
		}
		if (j == count) {
			isError("delete_node_iiNet net wrong");
		}
		if (net->count[neigh] == 0) {
			net->idNum--;
			free(net->edges[neigh]);
			net->edges[neigh] = NULL;
		}
	}
	free(net->edges[nid]);
	net->edges[nid] = NULL;
	//printf("delete node %d, node %d count is %ld, now the edgesNum is %ld\n", nid, nid, net->count[nid], net->edgesNum);
	net->count[nid] = 0;
	net->idNum--;
	print2l("%s =>> ......end.\n", __func__);
	return 1;
}

int delete_link_iiNet(struct iiNet *net, int x, int y) {
	print2l("%s =>> begin......\n", __func__);
	if (x > net->maxId || y > net->maxId || net->count[x] == 0 || net->count[y] == 0) {
		print2l("%s =>> ......end.\n", __func__);
		return 0;
	}
	long i;
	int find_x_in_y = 0, find_y_in_x = 0;
	for (i = 0; i < net->count[x]; ++i) {
		if (net->edges[x][i] == y) {
			net->edges[x][i] = net->edges[x][--(net->count[x])];
			find_y_in_x = 1;
			if (net->count[x] == 0) {
				free(net->edges[x]);
				net->edges[x] = NULL;
				net->idNum--;
			}
			break;
		}
	}
	for (i = 0; i < net->count[y]; ++i) {
		if (net->edges[y][i] == x) {
			net->edges[y][i] = net->edges[y][--(net->count[y])];
			find_x_in_y = 1;
			if (net->count[y] == 0) {
				free(net->edges[y]);
				net->edges[y] = NULL;
				net->idNum--;
			}
			break;
		}
	}
	int find = find_x_in_y + find_y_in_x;
	if (find == 0) {
		print2l("%s =>> ......end.\n", __func__);
		return 0;
	}
	else if (find == 2) {
		net->edgesNum--;
		print2l("%s =>> ......end.\n", __func__);
		return 1;
	}
	isError("%s =>> net has something wrong.\n", __func__);
	//can not get here.
	return -1;
}

long *degree_distribution_iiNet(struct iiNet *net) {
	print2l("%s =>> begin......\n", __func__);
	long *cd = calloc(net->countMax + 1, sizeof(long));
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		cd[net->count[i]]++;
	}
	print2l("%s =>> ......end.\n", __func__);
	return cd;
}

static int extract_backbone_iiNet(int nid, struct iiNet *net, char *fg, int *left, int *right) {
	print4l("%s =>> begin......\n", __func__);
	if (fg[nid] == 1) isError("%s =>> the %d node is in another backbone.\n", __func__, nid);
	int lN = 0, rN = 0;
	left[lN++] = nid;
	fg[nid] = 1;
	int conn = 1;
	int i;
	long j;
	while(lN) {
		rN = 0;
		for (i = 0; i < lN; ++i) {
			int id = left[i];
			for (j = 0; j < net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (fg[neigh] == 0) {
					fg[neigh] = 1;
					++conn;
					right[rN++] = neigh;
				}
			}
		}
		int *tmp = left;
		left = right;
		right = tmp;
		lN = rN;
	}
	print4l("%s =>> ......end.\n", __func__);
	return conn;
}

//if net->count[nid] == 0, then I presume nid node is not existed.
int robust_iiNet(struct iiNet *net, int *robust) {
	print2l("%s =>> begin......\n", __func__);
	int N = net->idNum;
	int maxru = 0;
	int already = 0;
	int maxi = 0;

	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));

	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (fg[i] == 0 && net->count[i]) {
			int conn = extract_backbone_iiNet(i, net, fg, left, right);
			already += conn;
			if (maxru < conn) {
				maxru = conn;
				maxi = i;
			}
			if (maxru >= N-already) break;
		}
	}

	free(fg);
	free(left);
	free(right);
	*robust = maxru;
	print2l("%s =>> ......end.\n", __func__);
	return maxi;
}

void verify_duplicatePairs_iiNet(struct iiNet *net) {
	print2l("%s =>> begin......\n", __func__);
	long i;
	int j,k;
	int *place = malloc((net->maxId+1)*sizeof(int));
	for (k=0; k<net->maxId + 1; ++k) {
		place[k] = -1;
	}
	long dpairsNum=0;
	for (j=0; j<net->maxId+1; ++j) {
		for (i=0; i < net->count[j]; ++i) {
			int neigh = net->edges[j][i];
			if (place[neigh] == -1) {
				place[neigh] = 1;
			}
			else {
				print1l("%s =>> duplicate pairs %ld:\t%d\t%d\n", __func__, ++dpairsNum, j, neigh);
			}
		}
		for (i = 0; i < net->count[j]; ++i) {
			int neigh = net->edges[j][i];
			place[neigh] = -1;
		}
	}
	free(place);
	if (dpairsNum) {
		isError("%s =>> net has some duplicate pairs.\n", __func__);
	}
	print2l("%s =>> ......end.\n", __func__);
	fflush(stdout);
}

void verify_fullyConnected_iiNet(struct iiNet *net) {
	print2l("%s =>> begin......\n", __func__);
	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (net->count[i]) break;
	}
	int conn = extract_backbone_iiNet(i, net, fg, left, right);
	free(fg);
	free(left);
	free(right);
	if (conn != net->idNum) {
		isError("%s =>> not connectedness.\n", __func__);
	}
	print2l("%s =>> ......end.\n", __func__);
}
*/