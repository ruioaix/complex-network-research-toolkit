#include "base.h"
#include "net.h"
#include "mtprand.h"
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "spath.h"

void free_Net(struct Net *net) {
	printgfb();
	//free degree&indegree.
	free(net->degree); 
	free(net->indegree);

	//free edges&inedges.
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		free(net->edges[i]);
	}
	free(net->edges);
	if (net->inedges != NULL) {
		for(i=0; i<net->maxId+1; ++i) {
			free(net->inedges[i]);
		}
		free(net->inedges);
	}
	
	//free weight&inweight;
	if (net->weight != NULL) {
		for(i=0; i<net->maxId+1; ++i) {
			free(net->weight[i]);
		}
		free(net->weight);
		if (net->inweight != NULL) {
			for(i=0; i<net->maxId+1; ++i) {
				free(net->inweight[i]);
			}
			free(net->inweight);
		}
	}

	//free edgesAttr&inedgesAttr
	if (net->edgesAttr != NULL) {
		for(i=0; i<net->maxId+1; ++i) {
			free(net->edgesAttr[i]);
		}
		free(net->edgesAttr);
		if (net->inedgesAttr != NULL) {
			for(i=0; i<net->maxId+1; ++i) {
				free(net->inedgesAttr[i]);
			}
			free(net->inedgesAttr);
		}
	}

	//free edgesMatrix
	if (net->edgesMatrix.sign == NS_VALID) {
		free(net->edgesMatrix.pp[0]);
		free(net->edgesMatrix.pp);
	}

	//free net
	free(net); 
	net = NULL;
	printgfe();
}

static struct Net *create_Net_init(long edgesNum, \
		int maxId, int minId, int idNum, \
		int *degree, int *indegree, \
		int **edges, int **inedges, \
		double **weight, double **inweight, \
		double **edgesAttr, double **inedgesAttr, \
		enum NET_STATUS dmax, int degreeMax, \
		enum NET_STATUS dmin, int degreeMin, \
		enum NET_STATUS idmax, int indegreeMax, \
		enum NET_STATUS idmin, int indegreeMin, \
		enum NET_STATUS connectnessStatus, \
		enum NET_STATUS duplicatepairsStatus) 
{ 
	struct Net *net = smalloc(sizeof(struct Net));
	net->edgesNum=edgesNum;
	net->maxId=maxId;
	net->minId=minId;
	net->idNum=idNum;
	net->degree=degree;
	net->indegree=indegree;
	net->edges=edges;
	net->inedges = inedges;
	net->weight = weight;
	net->inweight = inweight;
	net->edgesAttr = edgesAttr;
	net->inedgesAttr = inedgesAttr;

	net->degreeMax.sign = dmax;
	net->degreeMax.value = degreeMax;
	net->degreeMin.sign = dmin;
	net->degreeMin.value = degreeMin;
	net->indegreeMax.sign = idmax;
	net->indegreeMax.value = indegreeMax;
	net->indegreeMin.sign = idmin;
	net->indegreeMin.value = indegreeMin;

	net->avesp.sign = NS_NON_VALID;
	net->connectnessStatus = connectnessStatus;
	net->duplicatepairsStatus = duplicatepairsStatus;

	net->edgesMatrix.sign = NS_NON_VALID;
	return net;
}

static void create_Net_basic_lf_check(struct LineFile *lf) {
	if (lf->i1 == NULL || lf->i2 == NULL || lf->linesNum < 1) {
		isError("i1 or/and i2 is NULL, or/and linesNum is 0.\n");
	}
}

static void create_Net_maxId_minId(struct LineFile *lf, \
		int *maxId, int *minId) {
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

static void create_Net_undirect_degree_idNum(struct LineFile *lf, int maxId, \
		int **degree, int *idNum) {
	*degree=calloc(maxId+1, sizeof(int)); assert(degree!=NULL);

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

static void create_Net_direct_degree_idNum(struct LineFile *lf, int maxId, \
		int **degree, int **indegree ,int *idNum) {
	*degree=scalloc(maxId+1, sizeof(int)); 
	*indegree=scalloc(maxId+1, sizeof(int));

	long i;
	for(i=0; i<lf->linesNum; ++i) {
		++(*degree)[lf->i1[i]];
		++(*indegree)[lf->i2[i]];
	}
	int j;
	*idNum = 0;
	for(j=0; j<maxId+1; ++j) {
		if ((*degree)[j]>0 || (*indegree)[j]>0) {
			++(*idNum);
		}
	}
}

static void create_Net_directed_edges_weight_edgesAttr(struct LineFile *lf, int maxId, int *degree, int *indegree, double *wgt, double *eda, \
		int ***edges, int ***inedges, \
		double ***weight, double ***inweight, \
		double ***edgesAttr, double ***inedgesAttr, \
		int *degreeMax, int *degreeMin, int *indegreeMax, int *indegreeMin) {
	*edges=smalloc((maxId+1)*sizeof(int *));
	*inedges=smalloc((maxId+1)*sizeof(int *));
	if (wgt != NULL) {
		*weight = smalloc((maxId+1)*sizeof(double *));
		*inweight = smalloc((maxId+1)*sizeof(double *));
	}
	if (eda != NULL) {
		*edgesAttr = smalloc((maxId+1)*sizeof(double *));
		*inedgesAttr = smalloc((maxId+1)*sizeof(double *));
	}
	int i;
	int dmi=INT_MAX, idmi=INT_MAX; 
	int dma=0, idma=0;
	for(i=0; i<maxId+1; ++i) {
		if (degree[i]!=0) {
			dma = dma > degree[i] ? dma : degree[i];
			dmi = dmi < degree[i] ? dmi : degree[i];
			(*edges)[i]=smalloc(degree[i]*sizeof(int));
			if (wgt != NULL) {
				(*weight)[i]=smalloc(degree[i]*sizeof(double));
			}
			if (eda != NULL) {
				(*edgesAttr)[i]=smalloc(degree[i]*sizeof(double));
			}
		}
		else {
			(*edges)[i] = NULL;
			if (wgt != NULL) {
				(*weight)[i] = NULL;
			}
			if (eda != NULL) {
				(*edgesAttr)[i] = NULL;
			}
		}
		
		if (indegree[i]!=0) {
			idma = idma > indegree[i] ? idma : indegree[i];
			idmi = idmi < indegree[i] ? idmi : indegree[i];
			(*inedges)[i]=smalloc(indegree[i]*sizeof(int));
			if (wgt != NULL) {
				(*inweight)[i]=smalloc(indegree[i]*sizeof(double));
			}
			if (eda != NULL) {
				(*inedgesAttr)[i]=smalloc(indegree[i]*sizeof(double));
			}
		}
		else {
			(*inedges)[i] = NULL;
			if (wgt != NULL) {
				(*inweight)[i] = NULL;
			}
			if (eda != NULL) {
				(*inedgesAttr)[i] = NULL;
			}
		}
	}
	*degreeMax = dma;
	*degreeMin = dmi;
	*indegreeMax = idma;
	*indegreeMin = idmi;

	int *out_count=scalloc(maxId+1, sizeof(int));
	int *in_count=scalloc(maxId+1, sizeof(int));
	long j;
	for(j=0; j<lf->linesNum; ++j) {
		int i1 =lf->i1[j];
		int i2 =lf->i2[j];
		(*edges)[i1][out_count[i1]] = i2;
		(*inedges)[i2][in_count[i2]] = i1;
		if (wgt != NULL) {
			double w = wgt[j];
			(*weight)[i1][out_count[i1]] = w;
			(*inweight)[i2][in_count[i2]] = w;
		}
		if (eda != NULL) {
			double ea = eda[j];
			(*edgesAttr)[i1][out_count[i1]] = ea;
			(*inedgesAttr)[i2][in_count[i2]] = ea;
		}
		++out_count[i1];
		++in_count[i2];
	}
	free(out_count);
	free(in_count);
}

static void create_Net_undirected_edges_weight_edgesAttr(struct LineFile *lf, int maxId, int *degree, double *wgt, double *eda, \
		int ***edges, double ***weight, double ***edgesAttr, int *degreeMax, int *degreeMin) {
	*edges=smalloc((maxId+1)*sizeof(int *));
	if (wgt != NULL) {
		*weight=smalloc((maxId+1)*sizeof(double *));
	}
	if (eda != NULL) {
		*edgesAttr =smalloc((maxId+1)*sizeof(double *));
	}
	int i;
	int dmi=INT_MAX;
	int dma=0;
	for(i=0; i<maxId+1; ++i) {
		if (degree[i]!=0) {
			dma = dma > degree[i] ? dma : degree[i];
			dmi = dmi < degree[i] ? dmi : degree[i];
			(*edges)[i]=smalloc(degree[i]*sizeof(int));
			if (wgt != NULL) {
				(*weight)[i]=smalloc(degree[i]*sizeof(double));
			}
			if (eda != NULL) {
				(*edgesAttr)[i]=smalloc(degree[i]*sizeof(double));
			}
		}
		else {
			(*edges)[i] = NULL;
			if (wgt != NULL) {
				(*weight)[i] = NULL;
			}
			if (eda != NULL) {
				(*edgesAttr)[i] = NULL;
			}
		}
	}
	*degreeMax = dma;
	*degreeMin = dmi;

	int *count=scalloc(maxId+1, sizeof(int));
	long j;
	for(j=0; j<lf->linesNum; ++j) {
		int i1 =lf->i1[j];
		int i2 =lf->i2[j];
		(*edges)[i1][count[i1]] = i2;
		(*edges)[i2][count[i2]] = i1;
		if (wgt != NULL) {
			double w = wgt[j];
			(*weight)[i1][count[i1]] = w;
			(*weight)[i2][count[i2]] = w;
		}
		if (eda != NULL) {
			double ea = eda[j];
			(*edgesAttr)[i1][count[i1]] = ea;
			(*edgesAttr)[i2][count[i2]] = ea;
		}
		++count[i1];
		++count[i2];
	}
	free(count);
}

struct Net *create_Net(struct LineFile * lf) {
	printgfb();
	create_Net_basic_lf_check(lf);

	int maxId, minId;
	create_Net_maxId_minId(lf, &maxId, &minId);
	printgf("get maxId: %d, minId: %d\n", maxId, minId);

	int idNum;
	int *degree;
	create_Net_undirect_degree_idNum(lf, maxId, &degree, &idNum);
	printgf("get degree, idNum: %d.\n", idNum);

	int degreeMax, degreeMin;
	int **edges;
	double **weight = NULL;
	double **edgesAttr = NULL;
	double *wgt = lf->d1;
	double *eda = lf->d2;
	create_Net_undirected_edges_weight_edgesAttr(lf, maxId, degree, wgt, eda, &edges, &weight, &edgesAttr, &degreeMax, &degreeMin);
	printgf("get degreeMax: %d, degreeMin: %d; and alloc memory and fill edges.\n", degreeMax, degreeMin);

	struct Net *net = create_Net_init(lf->linesNum, \
			maxId, minId, idNum, \
			degree, NULL, \
			edges, NULL, \
			weight, NULL, \
			edgesAttr, NULL, 
			NS_VALID, degreeMax, \
			NS_VALID, degreeMin, \
			NS_NON_VALID, -1, \
			NS_NON_VALID, -1, \
			NS_NOTSURE, \
			NS_NOTSURE); 
	printgf("return Net.\n");
	printgfe();
	return net;
}

struct Net *create_directed_Net(struct LineFile * lf) {
	printgfb();
	create_Net_basic_lf_check(lf);

	printgf("get edgesNum: %ld\n", lf->linesNum);
	int maxId, minId;
	create_Net_maxId_minId(lf, &maxId, &minId);
	printgf("get maxId: %d, minId: %d\n", maxId, minId);

	int idNum;
	int *degree, *indegree;
	create_Net_direct_degree_idNum(lf, maxId, &degree, &indegree, &idNum);
	printgf("get degree, idNum: %d.\n", idNum);

	int degreeMax, degreeMin;
	int indegreeMax, indegreeMin;
	int **edges, **inedges;
	double *wgt = lf->d1;
	double **weight = NULL, **inweight= NULL;
	double *eda = lf->d2;
	double **edgesAttr = NULL, **inedgesAttr = NULL;
	create_Net_directed_edges_weight_edgesAttr(lf, maxId, degree, indegree, wgt, eda, \
			&edges, &inedges, &weight, &inweight, &edgesAttr, &inedgesAttr, &degreeMax, &degreeMin, &indegreeMax, &indegreeMin);
	printgf("get degreeMax: %d, degreeMin: %d, indegreeMax: %d, indegreeMin: %d; and alloc memory and fill edges.\n", degreeMax, degreeMin, indegreeMax, indegreeMin);

	struct Net *net = create_Net_init(lf->linesNum, \
			maxId, minId, idNum, \
			degree, indegree, \
			edges, inedges, \
			weight, inweight, \
			edgesAttr, inedgesAttr, \
			NS_VALID, degreeMax, \
			NS_VALID, degreeMin, \
			NS_VALID, indegreeMax, \
			NS_VALID, indegreeMin, \
			NS_NOTSURE, \
			NS_NOTSURE); 

	printgf("return Net.\n");
	printgfe();
	return net;
}

#if 0
struct Net *create_weighted_Net(struct LineFile * lf) {
	printgfb();
	create_Net_basic_lf_check(lf);
	if (lf->d1 == NULL) isError("%s =>> weight is NULL.\n", __func__);
	double *wgt = lf->d1;

	int maxId, minId;
	create_Net_maxId_minId(lf, &maxId, &minId);
	printgf("get maxId: %d, minId: %d\n", maxId, minId);

	int idNum;
	int *degree;
	create_Net_undirect_degree_idNum(lf, maxId, &degree, &idNum);
	printgf("get degree, idNum: %d.\n", idNum);

	int degreeMax, degreeMin;
	int **edges;
	double **weight;
	double *eda = lf->d2;
	double **edgesAttr=NULL;
	create_Net_undirected_edges_weight_edgesAttr(lf, maxId, degree, wgt, eda, &edges, &weight, &edgesAttr, &degreeMax, &degreeMin);
	printgf("get degreeMax: %d, degreeMin: %d; and alloc memory and fill edges.\n", degreeMax, degreeMin);

	struct Net *net = create_Net_init(lf->linesNum, \
			maxId, minId, idNum, \
			degree, NULL, \
			edges, NULL, \
			weight, NULL, \
			edgesAttr, NULL, \
			NS_VALID, degreeMax, \
			NS_VALID, degreeMin, \
			NS_NON_VALID, -1, \
			NS_NON_VALID, -1, \
			NS_NOTSURE, \
			NS_NOTSURE); 
	printgf("return Net.\n");
	printgfe();
	return net;
}

struct Net *create_directed_weighted_Net(struct LineFile * lf) {
	printgfe();
	create_Net_basic_lf_check(lf);
	if (lf->d1 == NULL) isError("%s =>> weight is NULL.\n", __func__);
	double *wgt = lf->d1;

	int maxId, minId;
	create_Net_maxId_minId(lf, &maxId, &minId);
	printgf("get maxId: %d, minId: %d\n", maxId, minId);

	int idNum;
	int *degree, *indegree;
	create_Net_direct_degree_idNum(lf, maxId, &degree, &indegree, &idNum);
	printgf("get degree, idNum: %d.\n", idNum);

	int degreeMax, degreeMin;
	int indegreeMax, indegreeMin;
	int **edges, **inedges;
	double **weight, **inweight;
	double *eda = lf->d2;
	double **edgesAttr=NULL, **inedgesAttr=NULL;
	create_Net_directed_edges_weight_edgesAttr(lf, maxId, degree, indegree, wgt, eda, \
			&edges, &inedges, &weight, &inweight, &edgesAttr, &inedgesAttr, &degreeMax, &degreeMin, &indegreeMax, &indegreeMin);
	printgf("get degreeMax: %d, degreeMin: %d, indegreeMax: %d, indegreeMin: %d; and alloc memory and fill edges.\n", degreeMax, degreeMin, indegreeMax, indegreeMin);

	struct Net *net = create_Net_init(lf->linesNum, \
			maxId, minId, idNum, \
			degree, indegree, \
			edges, inedges, \
			weight, inweight, \
			edgesAttr, inedgesAttr, \
			NS_VALID, degreeMax, \
			NS_VALID, degreeMin, \
			NS_VALID, indegreeMax, \
			NS_VALID, indegreeMin, \
			NS_NOTSURE, \
			NS_NOTSURE); 
	printgf("return Net.\n");
	printgfe();
	return net;
}
#endif

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

void set_option_edgesMatrix_Net(struct Net *net) {
	int maxId = net->maxId;
	int *em_onetime = scalloc((maxId + 1) * (maxId + 1), sizeof(int));
	int **em = smalloc((maxId + 1) * sizeof(int *));
	int i;
	em[0] = em_onetime;
	for (i = 1; i < maxId + 1; ++i) {
		em[i] = em_onetime + (maxId + 1)*i;
	}
	long j;
	for (j = 0; j < (maxId + 1)*(maxId + 1); ++j) {
		em_onetime[j] = -1;
	}
	for (i = 0; i < maxId + 1; ++i) {
		for (j = 0; j < net->degree[i]; ++j) {
			int x = i;
			int y = net->edges[i][j];
			em[x][y] = j;
		}
	}
	net->edgesMatrix.pp = em;
	net->edgesMatrix.sign = NS_VALID;
}

void set_status_connectness_Net(struct Net *net) {
	printgfb();
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (net->degree[i] != 0) {
			break;
		}
	}
	int * sp = spath_1A_undirect_unweight_Net(net, i);
	for (i = 0; i < net->maxId + 1; ++i) {
		if(net->degree[i] != 0 && sp[i] == 0) {
			net->connectnessStatus = NS_NON_CNNTNESS;
			free(sp);
			return;
		}
	}
	net->connectnessStatus = NS_CNNTNESS;
	free(sp);
	printgfe();
}

void set_status_duplicatepairs_Net(struct Net *net) {
	printgfb();
	int i,j,k;
	int *place = malloc((net->maxId+1)*sizeof(int));
	for (k=0; k<net->maxId + 1; ++k) {
		place[k] = -1;
	}
	long dpairsNum=0;
	FILE *fp = NULL;
	char *tmpdpfile = "/tmp/duplicatepairs";
	for (j=0; j<net->maxId+1; ++j) {
		for (i=0; i < net->degree[j]; ++i) {
			int neigh = net->edges[j][i];
			if (place[neigh] == -1) {
				place[neigh] = 1;
			}
			else {
				if (fp == NULL) {
					fp = fopen(tmpdpfile, "w");
					if (fp == NULL)  isError("can not open %s file.\n", tmpdpfile);
				}
				fprintf(fp, "duplicate pairs %ld:\t%d\t%d\n", ++dpairsNum, j, neigh);
			}
		}
		for (i = 0; i < net->degree[j]; ++i) {
			int neigh = net->edges[j][i];
			place[neigh] = -1;
		}
	}
	if (fp != NULL) fclose(fp);
	free(place);
	if (dpairsNum) {
		printm("the net has some duplicate pairs which have been output to %s file.\nyou can check that file.\n", tmpdpfile);
		net->duplicatepairsStatus = NS_DUPPAIRS;
	}
	else {
		net->duplicatepairsStatus = NS_NON_DUPPAIRS;
	}
	printgfe();
}

void clean_duplicatepairs_Net(struct Net *net, char *cleanfilename, char *duplicatefilename) {
	printgfb();
	int i,j,k;
	int *place = smalloc((net->maxId+1)*sizeof(int));
	for (k=0; k<net->maxId + 1; ++k) {
		place[k] = -1;
	}
	long dpairsNum=0;
	FILE *fpc = NULL, *fpd = NULL;
	for (j=0; j<net->maxId+1; ++j) {
		for (i=0; i < net->degree[j]; ++i) {
			int neigh = net->edges[j][i];
			if (place[neigh] == -1) {
				place[neigh] = 1;
				if (fpc == NULL) {
					fpc = sfopen(cleanfilename, "w");
				}
				if (neigh > j) {
					fprintf(fpc, "%d\t%d\n", j, neigh);
				}
			}
			else {
				if (fpd == NULL) {
					fpd = sfopen(duplicatefilename, "w");
				}
				fprintf(fpd, "duplicate pairs %ld:\t%d\t%d\n", ++dpairsNum, j, neigh);
			}
		}
		for (i = 0; i < net->degree[j]; ++i) {
			int neigh = net->edges[j][i];
			place[neigh] = -1;
		}
	}
	if (fpc != NULL) fclose(fpc);
	if (fpd != NULL) fclose(fpd);

	free(place);
	if (dpairsNum) {
		printf("the net has some duplicate pairs which have been output to %s file.\nyou can check that file.\nclean net file has been output to %s.\n", duplicatefilename, cleanfilename);
	}
	else {
		printf("perfect network");
	}
	printgfe();
}

void delete_duplicatepairs_Net(struct Net *net) {
	printgfb();
	if (net->inedges != NULL) isError("net should be undirected.");
	int i,j,k;
	int *place = smalloc((net->maxId+1)*sizeof(int));
	for (k=0; k<net->maxId + 1; ++k) {
		place[k] = -1;
	}
	for (j=0; j<net->maxId+1; ++j) {
		for (i=0; i < net->degree[j]; ) {
			int neigh = net->edges[j][i];
			if (place[neigh] == -1) {
				place[neigh] = 1;
				++i;
			}
			else {
				net->edges[j][i] = net->edges[j][--(net->degree[j])];
				if (net->weight != NULL) {
					net->weight[j][i] = net->weight[j][net->degree[j]];
				}
				if (net->edgesAttr != NULL) {
					net->edgesAttr[j][i] = net->edgesAttr[j][net->degree[j]];
				}
			}
		}
		for (i = 0; i < net->degree[j]; ++i) {
			int neigh = net->edges[j][i];
			place[neigh] = -1;
		}
	}
	free(place);

	int degreeMax = 0, degreeMin = INT_MAX;
	long old = net->edgesNum;
	net->edgesNum = 0;
	for (i = 0; i < net->maxId + 1; ++i) {
		int degree = net->degree[i];
		net->edgesNum += degree;
		degreeMax = degreeMax > degree ? degreeMax : degree;
		degreeMin = degreeMin < degree ? degreeMin : degree;
	}
	net->edgesNum /= 2;
	printgf("old edgesNum: %ld, new edgesNum: %ld.\n", old, net->edgesNum);
	net->degreeMax.sign = NS_VALID;
	net->degreeMax.value = degreeMax;
	net->degreeMin.sign = NS_VALID;
	net->degreeMin.value = degreeMin;
	net->duplicatepairsStatus = NS_NON_DUPPAIRS;
	printgfe();
}


struct LineFile *similarity_linkout_CN_directed_Net(struct Net *net) {
	printgfb();
	if (net->inedges == NULL) isError("net should be directed.");
	int MSTEP = 100000;
	int *id1 = smalloc(MSTEP*sizeof(int));
	int *id2 = smalloc(MSTEP*sizeof(int));
	double *sim = smalloc(MSTEP*sizeof(double));
	long linesNum = 0, memNum=MSTEP;
	char *sign = scalloc(net->maxId + 1, sizeof(char));

	int i,j,k;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (net->degree[i] == 0) continue;
		for (j = 0; j < net->degree[i]; ++j) {
			int neigh = net->edges[i][j];
			sign[neigh] = 1;
		}
		for (j = 0; j < net->degree[i]; ++j) {
			int id = net->edges[i][j];
			if (net->degree[id] == 0) continue;
			int cn = 0;
			int an = net->degree[i];
			for (k = 0; k < net->degree[id]; ++k) {
				int neigh = net->edges[id][k];
				if (sign[neigh] == 1) ++cn;
				else ++an;
			}
			if (linesNum == memNum) {
				id1 = srealloc(id1, (memNum+MSTEP)*sizeof(int));
				id2 = srealloc(id2, (memNum+MSTEP)*sizeof(int));
				sim = srealloc(sim, (memNum+MSTEP)*sizeof(double));
				memNum += MSTEP;
			}
			if (cn != 0) {
				id1[linesNum] = i;
				id2[linesNum] = id;
				sim[linesNum] = (double)cn/an;
				++linesNum;
			}
		}
		for (j = 0; j < net->degree[i]; ++j) {
			int neigh = net->edges[i][j];
			sign[neigh] = 0;
		}
	}
	free(sign);

	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->d1 = sim;
	lf->linesNum = linesNum;
	lf->memNum = memNum;
	lf->filename = "similarity_CN_Net";
	printgfe();
	return lf;
}

struct LineFile *similarity_linkin_CN_directed_Net(struct Net *net) {
	printgfb();
	if (net->inedges == NULL) isError("net should be directed.");
	int MSTEP = 100000;
	int *id1 = smalloc(MSTEP*sizeof(int));
	int *id2 = smalloc(MSTEP*sizeof(int));
	double *sim = smalloc(MSTEP*sizeof(double));
	long linesNum = 0, memNum=MSTEP;
	char *sign = scalloc(net->maxId + 1, sizeof(char));

	int i,j,k;
	for (i = 0; i < net->maxId + 1; ++i) {
		//I will calculate the similarity of the two nodes which are in a link.
		//if A and B is not connected directly, I will not calculate similarity of A&B.
		//i is one vertex of edges.
		//id is the other vertex of edges.
		if (net->indegree[i] == 0) continue;
		for (j = 0; j < net->indegree[i]; ++j) {
			int neigh = net->inedges[i][j];
			sign[neigh] = 1;
		}
		for (j = 0; j < net->indegree[i]; ++j) {
			int id = net->inedges[i][j];
			if (net->indegree[id] == 0) continue;
			int cn = 0;
			int an = net->indegree[i];
			for (k = 0; k < net->indegree[id]; ++k) {
				int neigh = net->inedges[id][k];
				if (sign[neigh] == 1) ++cn;
				else  ++an;
			}
			if (linesNum == memNum) {
				id1 = srealloc(id1, (memNum+MSTEP)*sizeof(int));
				id2 = srealloc(id2, (memNum+MSTEP)*sizeof(int));
				sim = srealloc(sim, (memNum+MSTEP)*sizeof(double));
				memNum += MSTEP;
			}
			if (cn != 0) {
				id1[linesNum] = i;
				id2[linesNum] = id;
				sim[linesNum] = (double)cn/an;
				++linesNum;
			}
		}
		for (j = 0; j < net->indegree[i]; ++j) {
			int neigh = net->inedges[i][j];
			sign[neigh] = 0;
		}
	}
	free(sign);

	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->d1 = sim;
	lf->linesNum = linesNum;
	lf->memNum = memNum;
	lf->filename = "similarity_CN_Net";
	printgfe();
	return lf;
}

struct LineFile *similarity_linkboth_CN_directed_Net(struct Net *net) {
	printgfb();
	if (net->inedges == NULL) isError("net should be directed.");
	int MSTEP = 100000;
	int *id1 = smalloc(MSTEP*sizeof(int));
	int *id2 = smalloc(MSTEP*sizeof(int));
	double *sim = smalloc(MSTEP*sizeof(double));
	long linesNum = 0, memNum=MSTEP;
	char *sign = scalloc(net->maxId + 1, sizeof(char));

	int i,j,k;
	for (i = 0; i < net->maxId + 1; ++i) {
		//I will calculate the similarity of the two nodes which are in a link.
		//if A and B is not connected directly, I will not calculate similarity of A&B.
		//i is one vertex of edges.
		//id is the other vertex of edges.
		if (net->degree[i] == 0 && net->indegree[i] == 0) continue;
		for (j = 0; j < net->degree[i]; ++j) {
			int neigh = net->edges[i][j];
			sign[neigh] = 1;
		}
		for (j = 0; j < net->indegree[i]; ++j) {
			int neigh = net->inedges[i][j];
			sign[neigh] = 1;
		}
		for (j = 0; j < net->degree[i]; ++j) {
			int id = net->edges[i][j];
			if (net->degree[id] == 0 && net->indegree[id] == 0) continue;
			int cn = 0;
			int an = net->indegree[i] + net->degree[i];
			for (k = 0; k < net->indegree[id]; ++k) {
				int neigh = net->inedges[id][k];
				if (sign[neigh] == 1) ++cn;
				else ++an;
			}
			for (k = 0; k < net->degree[id]; ++k) {
				int neigh = net->edges[id][k];
				if (sign[neigh] == 1) ++cn;
				else  ++an;
			}
			if (linesNum == memNum) {
				id1 = srealloc(id1, (memNum+MSTEP)*sizeof(int));
				id2 = srealloc(id2, (memNum+MSTEP)*sizeof(int));
				sim = srealloc(sim, (memNum+MSTEP)*sizeof(double));
				memNum += MSTEP;
			}
			if (cn != 0) {
				id1[linesNum] = i;
				id2[linesNum] = id;
				sim[linesNum] = (double)cn/an;
				++linesNum;
			}
		}
		for (j = 0; j < net->degree[i]; ++j) {
			int neigh = net->edges[i][j];
			sign[neigh] = 0;
		}
		for (j = 0; j < net->indegree[i]; ++j) {
			int neigh = net->inedges[i][j];
			sign[neigh] = 0;
		}
	}
	free(sign);

	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->d1 = sim;
	lf->linesNum = linesNum;
	lf->memNum = memNum;
	lf->filename = "similarity_CN_Net";
	printgfe();
	return lf;
}

struct LineFile *similarity_CN_Net(struct Net *net, struct Net *acc) {
	if (net->inedges != NULL) isError("net should be undirected.");
	int MSTEP = 100000;
	int *id1 = smalloc(MSTEP*sizeof(int));
	int *id2 = smalloc(MSTEP*sizeof(int));
	double *sim = smalloc(MSTEP*sizeof(double));
	long linesNum = 0, memNum=MSTEP;
	char *sign = scalloc(net->maxId + 1, sizeof(char));

	int i,j,k;
	//I only need the similarity between nodes which are in acc and has output links.
	//acc is directed. net is undirected;
	for (i = 0; i < acc->maxId + 1; ++i) {
		if (acc->degree[i] == 0 || net->degree[i] == 0) continue;
		for (j = 0; j < net->degree[i]; ++j) {
			int neigh = net->edges[i][j];
			sign[neigh] = 1;
		}
		for (j = 0; j < acc->degree[i]; ++j) {
			int id = acc->edges[i][j];
			if (acc->degree[id] == 0 || net->degree[id] == 0) continue;
			int cn = 0;
			int an = net->degree[i];
			for (k = 0; k < net->degree[id]; ++k) {
				int neigh = net->edges[id][k];
				if (sign[neigh] == 1) ++cn;
				else ++an;
			}
			if (linesNum == memNum) {
				id1 = srealloc(id1, (memNum+MSTEP)*sizeof(int));
				id2 = srealloc(id2, (memNum+MSTEP)*sizeof(int));
				sim = srealloc(sim, (memNum+MSTEP)*sizeof(double));
				memNum += MSTEP;
			}
			if (cn != 0) {
				id1[linesNum] = i;
				id2[linesNum] = id;
				sim[linesNum] = (double)cn/an;
				++linesNum;
			}
		}
		for (j = 0; j < net->degree[i]; ++j) {
			int neigh = net->edges[i][j];
			sign[neigh] = 0;
		}
	}
	free(sign);

	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->d1 = sim;
	lf->linesNum = linesNum;
	lf->memNum = memNum;
	lf->filename = "similarity_CN_Net";
	printgfe();
	return lf;
}
