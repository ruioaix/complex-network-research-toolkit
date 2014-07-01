#include "bip.h"
#include "base.h"
#include "mtprand.h"
#include "utilities.h"
#include <math.h>
#include <stdlib.h>
//#include <assert.h>
#include <string.h>
//#include <stdio.h>
#include <limits.h>

void free_Bip(struct Bip *bip) {
	int i=0;
	for(i=0; i<bip->left->maxId+1; ++i) {
		free(bip->left->edges[i]);
	}
	for(i=0; i<bip->right->maxId+1; ++i) {
		free(bip->right->edges[i]);
	}
	free(bip->left->degree);
	free(bip->right->degree);
	free(bip->left);
	free(bip->right);
	free(bip);
}

static void set_half_Bip(long linesNum, int *half, int *other_half, struct HalfBip *hbip) {
	long i;
	int minId = half[0];
	int maxId = half[0];
	for(i=0; i<linesNum; ++i) {
		maxId = maxId > half[i] ? maxId : half[i];
		minId = minId < half[i] ? minId : half[i];
	}
	int *degree = scalloc(maxId+1, sizeof(int));
	int **edges = smalloc((maxId+1)*sizeof(int *));

	for(i=0; i<linesNum; ++i) {
		++degree[half[i]];
	}

	int idNum = 0;
	int j;
	int degreeMax = -1;
	int degreeMin = INT_MAX;
	for(j=0; j<maxId+1; ++j) {
		if (degree[j]>0) {
			degreeMax = degreeMax>degree[j]?degreeMax:degree[j];
			degreeMin = degreeMin<degree[j]?degreeMin:degree[j];
			++idNum;
			edges[j]=smalloc(degree[j]*sizeof(int));
		}
		else {
			edges[j] = NULL;
		}
	}

	int *temp = scalloc(maxId+1, sizeof(int));
	for(i=0; i<linesNum; ++i) {
		int id = half[i];
		edges[id][temp[id]++]=other_half[i];
	}
	free(temp);

	hbip->maxId = maxId;
	hbip->minId = minId;
	hbip->idNum = idNum;
	hbip->degreeMax = degreeMax;
	hbip->degreeMin = degreeMin;
	hbip->degree = degree;
	hbip->edges = edges;
}

struct Bip *create_Bip(const struct LineFile * const lf) {
	printgfb();
	if (lf == NULL || lf->i1 == NULL || lf->i2 == NULL || lf->linesNum < 1) {
		isError("LineFile is not valid.");
	}

	//all elements of struct Bip.
	struct Bip *bip = smalloc(sizeof(struct Bip));
	bip->left = smalloc(sizeof(struct HalfBip));
	bip->right = smalloc(sizeof(struct HalfBip));

	bip->edgesNum = lf->linesNum;
	set_half_Bip(lf->linesNum, lf->i1, lf->i2, bip->left);
	set_half_Bip(lf->linesNum, lf->i2, lf->i1, bip->right);

	//fill Bip and return.
	printgf("left : Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d, edgesNum: %5ld\n", bip->left->maxId, bip->left->minId, bip->left->idNum, bip->left->degreeMax, bip->left->degreeMin, bip->edgesNum);
	printgf("right: Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d, edgesNum: %5ld\n", bip->right->maxId, bip->right->minId, bip->right->idNum, bip->right->degreeMax, bip->right->degreeMin, bip->edgesNum);

	printgfe();
	return bip;
}

void sort_desc_half_Bip(struct HalfBip *hbip) {
	int i;
	for (i = 0; i < hbip->maxId + 1; ++i) {
		if (hbip->degree[i]) {
			qsort_i_desc(hbip->edges[i], 0, hbip->degree[i] - 1);
		}
	}
}

static struct HalfBip *clone_half_Bip(struct HalfBip *hbip) {
	struct HalfBip *hnew = smalloc(sizeof(struct HalfBip));
	hnew->maxId = hbip->maxId;
	hnew->minId = hbip->minId;
	hnew->idNum = hbip->idNum;
	hnew->degreeMax = hbip->degreeMax;
	hnew->degreeMin = hbip->degreeMin;

	hnew->degree = smalloc((hbip->maxId + 1)*sizeof(int));
	memcpy(hnew->degree, hbip->degree, (hbip->maxId + 1)*sizeof(int));
	hnew->edges = smalloc((hbip->maxId + 1)*sizeof(int *));
	int i;
	for (i=0; i<hbip->maxId +1 ; ++i) {
		if (hnew->degree[i]) {
			hnew->edges[i] = smalloc(hnew->degree[i]*sizeof(int));
			memcpy(hnew->edges[i], hbip->edges[i], hnew->degree[i]*sizeof(int));
		}
		else {
			hnew->edges[i] = NULL;
		}
	}
	return hnew;
}

struct Bip * clone_Bip(struct Bip *bip) {
	printgfb();
	if (bip->edgesNum < 1) {
		isError("when clone, the origin bip should be valid");
	}

	struct Bip *new = smalloc(sizeof(struct Bip));
	new->left = clone_half_Bip(bip->left);
	new->right = clone_half_Bip(bip->right);
	new->edgesNum = bip->edgesNum;

	return new;
}

void print_Bip(struct Bip *bip, char *filename) {
	FILE *fp = sfopen(filename, "w");
	int i,j;
	for (i = 0; i < bip->left->maxId + 1; ++i) {
		for (j = 0; j < bip->left->degree[i]; ++j) {
			fprintf(fp, "%d\t%d\n", i, bip->left->edges[i][j]);
		}
	}
	fclose(fp);
}

/*
 * divide Bip into two parts.
 * return two struct LineFile. 
 * the first one is always the small one.
 * the second is always the large one.
*/
void divide_Bip(struct Bip *bip, double rate, struct LineFile **small, struct LineFile **big) {
	printgfb();
	if (rate <=0 || rate >= 1) {
		isError("wrong rate: %f.\n", rate);
	}
	rate = rate>0.5?1-rate:rate;
	long l1, l2;
	if (bip->edgesNum > 100000) {
		l1 = (int)(bip->edgesNum*(rate+0.1));
		l2 = (int)(bip->edgesNum*(1-rate+0.1));
	}
	else {
		l2 = l1 = bip->edgesNum;
	}

	*small = create_LineFile(NULL);
	*big = create_LineFile(NULL);

	(*small)->i1 = malloc(l1*sizeof(int));
	assert((*small)->i1 != NULL);
	(*small)->i2 = malloc(l1*sizeof(int));
	assert((*small)->i2 != NULL);
	(*big)->i1 = malloc(l2*sizeof(int));
	assert((*big)->i1 != NULL);
	(*big)->i2 = malloc(l2*sizeof(int));
	assert((*big)->i2 != NULL);
	
	long line1=0, line2=0;

	char *i1sign = scalloc(bip->left->maxId + 1, sizeof(char));
	char *i2sign = scalloc(bip->right->maxId + 1, sizeof(char));

	int *counti1 = smalloc((bip->left->maxId + 1)*sizeof(int));
	memcpy(counti1, bip->left->degree, (bip->left->maxId + 1)*sizeof(int));
	int *counti2 = smalloc((bip->right->maxId + 1)*sizeof(int));
	memcpy(counti2, bip->right->degree, (bip->right->maxId + 1)*sizeof(int));

	int i, j, neigh;
	for (i=0; i<bip->left->maxId + 1; ++i) {
		for (j=0; j<bip->left->degree[i]; ++j) {
			neigh = bip->left->edges[i][j];
			if (get_d01_MTPR() < rate) {
				if ((counti1[i] == 1 && i1sign[i] == 0) || (counti2[neigh] == 1 && i2sign[neigh] == 0)) {
					(*big)->i1[line2] = i;	
					(*big)->i2[line2] = neigh;	
					--counti1[i];
					--counti2[neigh];
					i1sign[i] = 1;
					i2sign[neigh] = 1;
					++line2;
					continue;
				}
				(*small)->i1[line1] = i;	
				(*small)->i2[line1] = neigh;	
				--counti1[i];
				--counti2[neigh];
				++line1;
			}
			else {
				(*big)->i1[line2] = i;	
				(*big)->i2[line2] = neigh;	
				i1sign[i] = 1;
				i2sign[neigh] = 1;
				--counti1[i];
				--counti2[neigh];
				++line2;
			}
		}
	}
	if ((line1 > l1) || (line2 > l2)) {
		isError("truly bad luck...l1 and l2 two small.\n");
	}

	free(i1sign);
	free(i2sign);
	free(counti1);
	free(counti2);

	(*big)->linesNum = line2;
	(*small)->linesNum = line1;

	(*big)->filename = "trainset";
	(*small)->filename = "testset";

	printgf("rate: %.3f, big file's linesNum: %ld, small file's linesNum: %ld.\n", rate, line2, line1);
	printgfe();
}

struct LineFile *similarity_CF_HalfBip(struct HalfBip *hbip, int other_half_maxId) {
	int maxId = hbip->maxId;
	int _maxId = other_half_maxId;
	int *degree = hbip->degree;
	int **edges = hbip->edges;

	int *sign = scalloc((_maxId+ 1),sizeof(int));
	struct LineFile *simfile = create_LineFile(NULL);

	int con = 1000000;
	int *i1 = smalloc(con*sizeof(int));
	int *i2 = smalloc(con*sizeof(int));
	double *d1 = smalloc(con*sizeof(double));

	long linesNum = 0;

	int Sij;
	double soij;
	int i,j,k;
	for (i=0; i<maxId; ++i) {
		if (degree[i]) {
			for (k=0; k<degree[i]; ++k) {
				sign[edges[i][k]] = 1;
			}
			for (j = i+1; j<maxId+ 1; ++j) {
				if (degree[j]) {
					Sij = 0;
					for (k=0; k<degree[j]; ++k) {
						Sij += sign[edges[j][k]];
					}
					if (Sij) {
						soij = (double)Sij/pow(degree[i] * degree[j], 0.5);
						i1[linesNum] = i;
						i2[linesNum] = j;
						d1[linesNum] = soij;
						++linesNum;
						if (linesNum == con) {
							con += 1000000;
							i1 = srealloc(i1, con*sizeof(int));
							i2 = srealloc(i2, con*sizeof(int));
							d1 = realloc(d1, con*sizeof(double));
						}
					}
				}
			}
			for (k = 0; k < degree[i]; ++k) {
				sign[edges[i][k]] = 0;
			}
		}
	}
	free(sign);

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;
	simfile->filename = "bip_similarity";
	printgf("calculate similarity done, linesNum: %ld.\n", linesNum);

	return simfile;
}

struct MetricsBip *create_MetricsBip(void) {
	struct MetricsBip *lp = smalloc(sizeof(struct MetricsBip));
	lp->R = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->L = 0;
	lp->topL = NULL;
	return lp;
}

void clean_MetricsBip(struct MetricsBip *lp) {
	lp->R = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->L = 0;
	free(lp->topL);
	lp->topL = NULL;
}

void free_MetricsBip(struct MetricsBip *lp) {
	free(lp->topL);
	free(lp);
}

//following is for recommendation.
//R is rankscore.
//PL is precision
//Warning: about unselected_list_length, I use bipii->idNum, not bipii->maxId. 
//	but I believe in linyuan's paper, she use the bipii->maxId. 
//	I think bipii->idNum make more sence.
static void metrics_R_PL_Bip(int i1, int *i1count, int i2idNum, struct HalfBip *ltest, int L, int *rank, double *R, double *PL) {
	if (i1<ltest->maxId + 1 &&  ltest->degree[i1]) {
		int unselected_list_length = i2idNum - i1count[i1];
		int rank_i1_j = 0;
		int DiL = 0;
		int j, id;
		for (j=0; j<ltest->degree[i1]; ++j) {
			id = ltest->edges[i1][j];
			rank_i1_j += rank[id];
			if (rank[id] <= L) {
				++DiL;
			}
		}
		*R += (double)rank_i1_j/(double)unselected_list_length;
		*PL += (double)DiL/(double)L;
	}
}
//HL is hamming distance.
static double metrics_HL_Bip(int i1maxId, int *i1count, int i2maxId, int L, int *Hij) {
	int *sign = scalloc((i2maxId + 1), sizeof(int));
	int i, j, k;
	int cou = 0;
	int Cij = 0;
	double HL = 0;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1count[i]) {
			for (k=i*L; k<i*L+L; ++k) {
				sign[Hij[k]] = 1;
			}
			for (j=i+1; j<i1maxId + 1; ++j) {
				if (i1count[j]) {
					Cij = 0;
					for (k=j*L; k<j*L+L; ++k) {
						if (sign[Hij[k]]) {
							++Cij;
						}
					}
					HL += 1 - ((double)Cij)/(double)L;
					//printf("%d %d %d\n", i, j, Cij);
					++cou;
				}
			}
			for (k=i*L; k<i*L+L; ++k) {
				sign[Hij[k]] = 0;
			}
		}
	}
	free(sign);
	return HL/cou;
}
//IL is intrasimilarity
static double metrics_IL_Bip(int i1maxId, int *i1count, int i1idNum, int i2maxId, int L, int *Hij, struct Net *sim) {
	if (sim == NULL) return -1;
	double *sign = scalloc((i2maxId + 1), sizeof(double));
	int i, j, k;
	double IL = 0;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1count[i]) {
			int *tmp = Hij + i*L;
			for (j=0; j<L; ++j) {
				int id = tmp[j];
				for (k=0; k<sim->degree[id]; ++k) {
					sign[sim->edges[id][k]] = sim->weight[id][k];
				}
				for (k=j+1; k<L; ++k) {
					id = tmp[k];
					IL += sign[id];
				}
				for (k=0; k<sim->degree[id]; ++k) {
					sign[sim->edges[id][k]] = 0;
				}
			}
		}
	}
	free(sign);
	IL /= L*(L-1)*i1idNum;
	return 2*IL;
}
//NL is popularity.
static double metrics_NL_Bip(int i1maxId, int *i1count, int i1idNum, int *i2count, int L, int *Hij) {
	int i,j;
	double NL = 0;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1count[i]) {
			int *tmp = Hij + i*L;
			for (j=0; j<L; ++j) {
				NL += i2count[tmp[j]];
			}
		}
	}
	NL /= L*i1idNum;
	return NL;
}



struct Bip_recommend_param{
	//param for recommend.
	double HNBI_param;
	double RENBI_param;
	double hybrid_param;

	struct Net *leftSim;
	struct Net *rightSim;

	struct Bip *train;
	struct Bip *test;

	int source;

	//param for efficient.
	double *i1source;
	double *i2source;
	double *i1sourceA;
	double *i2sourceA;
	int *i1id;
	int *i2id;
};

static inline void Bip_core_common_part(struct Bip_recommend_param *args, int *i2id, int *rank, int *topL_i1L, int L) {
	
	int uid = args->source;
	long uidCount = args->train->left->degree[uid];
	int *uidId = args->train->left->edges[uid];
	int i2maxId = args->train->right->maxId;
	int *i2count = args->train->right->degree;
	double *i2source = args->i2source;

	long i;
	//set selected item's source to -1
	for (i=0; i<uidCount; ++i) {
		i2source[uidId[i]] = -1;
		//i2source[uidId[i]] = 0;
	}
	//set i2id and rank.
	int j=0;
	double dtmp;
	for (i=0; i<i2maxId + 1; ++i) {
		i2id[i] = i;
		//set unexisted item's source to -2.
		if (!i2count[i]) {
			i2source[i] = -2;
		}

		if (i2source[i] > 0 ) {
			dtmp = i2source[j];
			i2source[j] = i2source[i];
			i2source[i] = dtmp;

			i2id[i] = i2id[j];
			i2id[j] = i;

			++j;
			
		}
	}
	//to this step, i2source contains four parts: 
	//1, nomral i2source[x] , which <0, 1].
	//2, i2source[x] = 0, which x isn't selected by anyone has similarity.
	//3, i2source[x] = -1, which x has been selected by i1.
	//4, i2source[x] = -2, which x is the hole, x isn't selected by anyone.
	//
	//after qsort_di_desc, the id of the item with most source will be in i2id[0];
	//qsort_di_desc(i2source, 0, i2maxId, i2id);
	//printf("%d\t%d\n", j, i2maxId);
	qsort_di_desc(i2source, 0, j-1, i2id);
	//copy the top L itemid into topL.
	memcpy(topL_i1L, i2id, L*sizeof(int));
	//get rank;
	for (i=0; i<i2maxId + 1; ++i) {
		rank[i2id[i]] = i+1;
	}
}

//three-step random walk of Probs
static void mass_recommend_Bip(struct Bip_recommend_param *args) {

	int i1 = args->source;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->train->left->edges;
	int **i2ids = args->train->right->edges; 
	int i1maxId = args->train->left->maxId;
	int i2maxId = args->train->right->maxId;
	int *i1count = args->train->left->degree;
	int *i2count = args->train->right->degree;

	int i, j, neigh;
	long degree;
	double source;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
}

/** 
 * core function of recommendation.
 * type :
 *   -- mass (NONE arg)
 *   -- heats (NONE arg)
 *   -- HNBI  (double HNBI_param)
 *   -- RENBI  (RENBI_param)
 *   -- hybrid (hybrid_param)
 *   -- topk 
 *   -- hs
 *
 * all L is from this function. if you want to change, change the L below.
 */
static struct MetricsBip *recommend_Bip(void (*recommend_core)(struct Bip_recommend_param *), struct Bip_recommend_param *args) {

	int i1maxId      = args->train->left->maxId;
	int i1idNum      = args->train->left->idNum;
	int *i1count     = args->train->left->degree;

	int i2maxId      = args->train->right->maxId;
	int i2idNum      = args->train->right->idNum;
	int *i2count     = args->train->right->degree;

 	// all L is from this function. if you want to change, change the L below.
	int L = 50;

	double R, PL, HL, IL, NL;
	R=PL=HL=IL=NL=0;

	double *i1source = smalloc((i1maxId + 1)*sizeof(double));
	double *i2source = smalloc((i2maxId + 1)*sizeof(double));
	args->i1source = i1source;
	args->i2source = i2source;

	double *i1sourceA = smalloc((i1maxId + 1)*sizeof(double));
	double *i2sourceA = smalloc((i2maxId + 1)*sizeof(double));
	args->i1sourceA = i1sourceA;
	args->i2sourceA = i2sourceA;

	int *i1id =  smalloc((i1maxId + 1)*sizeof(int));
	int *i2id =  smalloc((i2maxId + 1)*sizeof(int));
	args->i1id = i1id;
	args->i2id = i2id;

	int *rank =  smalloc((i2maxId + 1)*sizeof(int));
	int *topL = scalloc(L*(i1maxId + 1), sizeof(int));

	int i;
	for (i = 0; i<i1maxId + 1; ++i) { 
		if (i1count[i]) {
			//get rank
			args->source = i;
			recommend_core(args);
			Bip_core_common_part(args, i2id, rank, topL + i*L, L);
			metrics_R_PL_Bip(i, i1count, /*i2maxId*/i2idNum, args->test->left, L, rank, &R, &PL);
		}
	}

	R /= args->test->edgesNum;
	PL /= args->test->left->idNum;
	HL = metrics_HL_Bip(i1maxId, i1count, i2maxId, L, topL);
	IL = metrics_IL_Bip(i1maxId, i1count, i1idNum, i2maxId, L, topL, args->rightSim);
	NL = metrics_NL_Bip(i1maxId, i1count, i1idNum, i2count, L, topL);
	
	struct MetricsBip *retn = create_MetricsBip();
	retn->R = R;
	retn->PL = PL;
	retn->HL = HL;
	retn->IL = IL;
	retn->NL = NL;
	retn->topL = topL;
	retn->L = L;

	//printf("hybrid:\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", R, PL, IL, HL, NL);
	free(i1source);
	free(i2source);
	free(i1sourceA);
	free(i2sourceA);
	free(i1id);
	free(i2id);
	free(rank);
	return retn;
}

struct MetricsBip *mass_Bip(struct Bip *train, struct Bip *test, struct Net *rightSim) {
	struct Bip_recommend_param param;
	param.rightSim = rightSim;
	param.train = train;
	param.test = test;

	return recommend_Bip(mass_recommend_Bip, &param);
}


#if 0
//three-step random walk of heats
static void heats_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int neigh, i;
	long j;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1;
	}
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1count[i]) {
			for (j=0; j<i1count[i]; ++j) {
				neigh = i1ids[i][j];
				i1source[i] += i2source[neigh];
			}
			i1source[i] /= i1count[i];
		}
	}
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2count[i]) {
			for (j=0; j<i2count[i]; ++j) {
				neigh = i2ids[i][j];
				i2source[i] += i1source[neigh];
			}
			i2source[i] /= i2count[i];
		}
	}
}

//three-step random walk of HNBI
static void HNBI_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double HNBI_param = args->HNBI_param;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int i, j, neigh;
	long degree;
	double source;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0*pow(i2count[neigh], HNBI_param);
	}
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
}

//five-step random walk of RENBI
static void RENBI_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double RENBI_param = args->RENBI_param;
	double *i2sourceA = args->i2sourceA;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int i, j, neigh;
	long degree;
	double source;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	//one
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
	//save three steps result	
	memcpy(i2sourceA, i2source, (i2maxId + 1)*sizeof(double));
	//four
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//five
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
	//now, i2source save w*w result.
	//add: w+RENBI_param*w*w.
	for (i=0; i<i2maxId + 1; ++i) {
		i2sourceA[i] += RENBI_param*i2source[i];
	}

	memcpy(i2source, i2sourceA, (i2maxId + 1)*sizeof(double));
}

//three-step random walk of hybrid
static void hybrid_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double hybrid_param = args->hybrid_param;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int neigh, i;
	//double source;
	long j;
	//one
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			double powl = pow(i2count[i], hybrid_param);
			for (j=0; j<i2count[i]; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += i2source[i]/powl;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2count[i]) {
			double powl = pow(i2count[i], 1-hybrid_param);
			for (j=0; j<i2count[i]; ++j) {
				neigh = i2ids[i][j];
				i2source[i] += i1source[neigh]/i1count[neigh];
			}
			i2source[i] /= powl;
		}
	}
}

static void mass_topk_recommend_Bip(struct Bip_recommend_param *args) {

	int i1 = args->i1;
    struct iidNet *userSim = args->userSim;
	int topk = args->mass_topk;

	double * i1source = args->i1source;
	double *i2source = args->i2source;

	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int i, j, neigh;
	long degree;
	double source;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	long k;
	for (k=0; k<userSim->count[i1]; ++k) {
		i = userSim->edges[i1][k];
		if (k < topk) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
}

static void mass_hs_recommend_Bip(struct Bip_recommend_param *args) {

	int i1 = args->i1;
    struct iidNet *userSim = args->userSim;
	int topk = args->mass_topk;

	double * i1source = args->i1source;
	double *i2source = args->i2source;

	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int i, j, neigh;
	long degree;
	double source;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	long k;
	for (k=0; k<userSim->count[i1]; ++k) {
		i = userSim->edges[i1][k];
		if (k < topk) {
			degree = i1count[i];
			source = (double)i1source[i]/((double)degree*k);
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
		else {
			break;
		}
	}
}

static void mass_score_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double *i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int maxscore = args->maxscore;
	double theta = args->mass_score;

	double *i1sourceA = args->i1sourceA;
	double *i2sourceA = args->i2sourceA;
	
	int **i1score = args->traini1->score;
	int **i2score = args->traini2->score;
	if (i1score == NULL || i2source == NULL) isError("mass_score_recommend_Bip");

	int *i2id = args->i2id;

	int i, j, neigh;
	long degree;
	double source;
	double totalsource = 0;
	//one 
	memset(i2id, 0, (i2maxId +1)*sizeof(int));
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2id[neigh] = i1score[i1][j];
		i2source[neigh] = pow(i1score[i1][j], theta);
		totalsource += i2source[neigh];	
	}
	for (j=0; j<i1count[i1]; ++j) {
		i2source[neigh] = i2source[neigh]*i1count[i1]/totalsource;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i];
			totalsource = 0;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				//i1sourceA[neigh] = pow(maxscore - fabs(0-i2id[i]), theta); 
				i1sourceA[neigh] = pow(maxscore - fabs(i2score[i][j]-i2id[i]), theta); 
				totalsource += i1sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source*i1sourceA[neigh]/totalsource;
			}
		}
	}
	//printf("xxxo\n");fflush(stdout);
	
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = i1count[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2sourceA[neigh] = pow((double)i1score[i][j]/(double)i2count[neigh], theta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
}

static void mass_scoret3step_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	double theta = args->mass_score;

	double *i2sourceA = args->i2sourceA;
	
	int **i1score = args->traini1->score;
	if (i1score == NULL) isError("mass_scoret3step_recommend_Bip");


	int i, j, neigh;
	long degree;
	double source;
	double totalsource = 0;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = i1count[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2sourceA[neigh] = pow((double)i1score[i][j]/(double)i2count[neigh], theta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
}

static void mass_degree_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	double theta = args->mass_score;

	double *i2sourceA = args->i2sourceA;
	
	int **score = args->traini1->score;
	if (score == NULL) isError("mass_degree_recommend_Bip");


	int i, j, neigh;
	long degree;
	double source;
	double totalsource = 0;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = i1count[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2sourceA[neigh] = pow(1.0/(double)i2count[neigh], theta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
}




struct Metrics_Bip *heats_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(heats_recommend_Bip, &param);
}

struct Metrics_Bip *HNBI_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double HNBI_param) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.HNBI_param = HNBI_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(HNBI_recommend_Bip, &param);
}

struct Metrics_Bip *RENBI_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double RENBI_param) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.RENBI_param = RENBI_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(RENBI_recommend_Bip, &param);
}

struct Metrics_Bip *hybrid_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double hybrid_param) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.hybrid_param = hybrid_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(hybrid_recommend_Bip, &param);
}


int *mass_getBK_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, double rate) {
	printf("calculate BK begin =>> ");
	print_time();

	double *i1source = malloc((traini1->maxId + 1)*sizeof(double));
	double *i2source = malloc((traini2->maxId + 1)*sizeof(double));
	int *rank = malloc((traini2->maxId + 1)*sizeof(int));
	int *i2id =  malloc((traini2->maxId + 1)*sizeof(int));

	double R, PL;

	struct Bip_recommend_param args;
	args.traini1 = traini1;
	args.traini2 = traini2;
	args.testi1 = testi1;
	args.i1source = i1source;
	args.i2source = i2source;
	args.userSim = userSim;

	int i;
    int j;
	int L = 50;
	int *topL = calloc(L*(traini1->maxId + 1), sizeof(int));
	double bR, bPL;
	int bi;
	int *best = malloc((traini1->maxId + 1)*sizeof(int));
	double total_bR = 0, total_R = 0;
	double total_bPL = 0, total_PL = 0;

	for (i = 0; i<traini1->maxId + 1; ++i) { //each user
		if (i<testi1->maxId + 1 && testi1->count[i]) {
			//just to make sure bestR is enough big.
			bR = 10000; //big enough.
			bPL = -1;
			bi = -1;
			//to make realR2 is same with mass.
			for (j=1; j<= userSim->count[i] || (j ==1 && userSim->count[i] == 0); ++j) {

				//probs_knn_Bip_core(i, &args, userSim, j);
				args.i1 = i;
				args.mass_topk = j;
				mass_topk_recommend_Bip(&args);

				
				Bip_core_common_part(&args, i2id, rank, topL + i*L, L);

				R=PL=0;
				metrics_R_PL_Bip(i, traini1->count, traini2->idNum, testi1, L, rank, &R, &PL);
				//R will never be 0, because i is in testi1.
				if (bR > R) {
					bR = R;
					bi = j;
				}
				if (bPL < PL) {
					bPL = PL;
				}
			}
			
			total_bR += bR;
			total_R += R;
			total_bPL += bPL;
			total_PL += PL;

			best[i] = bi;

		}
		else {
			best[i] = -1;
		}
	}
	printf("BR result =>> Rate: %f, Best R: %f, normal R: %f, Best PL: %f, normal PL: %f\n", rate, total_bR/testi1->edgesNum, total_R/testi1->edgesNum, total_bPL/testi1->edgesNum, total_PL/testi1->edgesNum);

	free(i1source);
	free(i2source);
	free(rank);
	free(i2id);
	free(topL);
	printf("calculate BK done =>> ");
	print_time();
	return best;
}

struct Metrics_Bip *mass_topk_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk) {
	struct Bip_recommend_param param;
	param.userSim = userSim;
	param.itemSim = itemSim;
	param.mass_topk = topk;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(mass_topk_recommend_Bip, &param);
}

struct Metrics_Bip *mass_hs_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk) {
	struct Bip_recommend_param param;
	param.userSim = userSim;
	param.itemSim = itemSim;
	param.mass_topk = topk;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(mass_hs_recommend_Bip, &param);
}

struct Metrics_Bip *mass_score_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, int maxscore, double mass_score) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.mass_score = mass_score;
	param.maxscore = maxscore;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	//return recommend_Bip(mass_recommend_Bip, &param);
	return recommend_Bip(mass_score_recommend_Bip, &param);
}

struct Metrics_Bip *mass_scoret3step_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double mass_score) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.mass_score = mass_score;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(mass_scoret3step_recommend_Bip, &param);
}

struct Metrics_Bip *mass_degree_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double mass_score) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.mass_score = mass_score;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(mass_degree_recommend_Bip, &param);
}

//just for rank
static double *rank_Bip(void (*recommend_core)(struct Bip_recommend_param *), struct Bip_recommend_param *args) {

	int i1maxId      = args->traini1->maxId;
	int i2maxId      = args->traini2->maxId;
	int i1idNum      = args->traini1->idNum;
	int i2idNum      = args->traini2->idNum;
	long *i1count    = args->traini1->count;
 	// all L is from this function. if you want to change, change the L below.
	int L = 50;

	double *i1source = malloc((i1maxId + 1)*sizeof(double));
	assert(i1source != NULL);
	double *i2source = malloc((i2maxId + 1)*sizeof(double));
	assert(i2source != NULL);
	args->i1source = i1source;
	args->i2source = i2source;

	double *i2sourceA = malloc((i2maxId + 1)*sizeof(double));
	assert(i2sourceA != NULL);
	args->i2sourceA = i2sourceA;
	double *i1sourceA = malloc((i1maxId + 1)*sizeof(double));
	assert(i1sourceA != NULL);
	args->i1sourceA = i1sourceA;

	int *rank = malloc((i2maxId + 1)*sizeof(int));
	assert(rank != NULL);
	int *i1id =  malloc((i1maxId + 1)*sizeof(int));
	assert(i1id != NULL);
	int *i2id =  malloc((i2maxId + 1)*sizeof(int));
	assert(i2id != NULL);
	args->i1id = i1id;
	args->i2id = i2id;

	int i, j;
	int *topL = calloc(L*(i1maxId + 1), sizeof(int));
	assert(topL != NULL);

	double *rankA = calloc((i2maxId + 1), sizeof(double));
	assert(rankA != NULL);

	for (i = 0; i<i1maxId + 1; ++i) { //each user
		//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
		if (i1count[i]) {
			//get rank
			args->i1 = i;
			recommend_core(args);
			Bip_core_common_part(args, i2id, rank, topL + i*L, L);
			for (j=0; j<i2maxId + 1; ++j) {
				rankA[j] += (double)rank[j]/((double)i2idNum*i1idNum);
			}
		}
	}

	free(i1source);
	free(i2source);
	free(i1sourceA);
	free(i2sourceA);
	free(i1id);
	free(i2id);
	free(rank);
	return rankA;
}

double *mass_score_rank_Bip(struct Bip *traini1, struct Bip *traini2, int maxscore, double mass_score) {
	struct Bip_recommend_param param;
	param.mass_score = mass_score;
	param.maxscore = maxscore;

	param.traini1 = traini1;
	param.traini2 = traini2;

	//return recommend_Bip(mass_recommend_Bip, &param);
	return rank_Bip(mass_score_recommend_Bip, &param);
}

double *mass_scoret3step_rank_Bip(struct Bip *traini1, struct Bip *traini2, double mass_score) {
	struct Bip_recommend_param param;
	//param.itemSim = itemSim;
	param.mass_score = mass_score;

	param.traini1 = traini1;
	param.traini2 = traini2;

	return rank_Bip(mass_scoret3step_recommend_Bip, &param);
}

double *mass_degree_rank_Bip(struct Bip *traini1, struct Bip *traini2, double mass_score) {
	struct Bip_recommend_param param;
	param.mass_score = mass_score;

	param.traini1 = traini1;
	param.traini2 = traini2;

	return rank_Bip(mass_degree_recommend_Bip, &param);
}
void verify_Bip(struct Bip *bipi1, struct Bip *bipi2) {
	long i;
	int j,k;
	int *place = malloc((bipi2->maxId+1)*sizeof(int));
	assert(place != NULL);
	FILE *fp = sfopen("data/duplicatePairsinNet", "w");
	FILE *fp2 = sfopen("data/NoDuplicatePairsNetFile", "w");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;
	for (j=0; j<bipi1->maxId+1; ++j) {
		if (bipi1->count[j]>0) {
			for (k=0; k<bipi2->maxId + 1; ++k) {
				place[k] = -1;
			}
			for (i=0; i<bipi1->count[j]; ++i) {
				int origin = bipi1->edges[j][i];
				int next = place[origin];
				if (next == -1) {
					place[origin]=origin;
					fprintf(fp2, "%d\t%d\n", j,origin);
				}
				else {
					fprintf(fp, "%d\t%d\n", j, next);
					sign=1;
				}
			}
		}
		if (j%10000 == 0) {
			printf("%d\n", j);
			fflush(stdout);
		}
	}
	free(place);
	fclose(fp);
	fclose(fp2);
	if (sign == 1) {
		isError("the file has duplicate pairs, you can check data/duplicatePairsinNet.\nwe generate a net file named data/NoDuplicatePairsNetFile which doesn't contain any duplicate pairsr.\nyou should use this file instead the origin wrong one.\n");
	}
	else {
		printf("verifyBip: perfect network.\n");
	}
}

struct LineFile *mass_similarity_Bip(struct Bip *bipi1, struct Bip *bipi2) {


	struct LineFile *simfile = create_LineFile(NULL);

	int con = 1000000;
	int *i1 = malloc(con*sizeof(int));
	int *i2 = malloc(con*sizeof(int));
	double *d1 = malloc(con*sizeof(double));

	long linesNum = 0;

	double *i2source = malloc((bipi2->maxId + 1)*sizeof(double));
	double *i1source = malloc((bipi1->maxId + 1)*sizeof(double));

	int i, j, neigh;
	long degree;
	double source;

	int k;
	for (k=0; k<bipi1->maxId + 1; ++k) {
		if (bipi1->count[k]) {
			memset(i2source, 0, (bipi2->maxId+1)*sizeof(double));
			for (j=0; j<bipi1->count[k]; ++j) {
				neigh = bipi1->edges[k][j];
				i2source[neigh] = 1.0;
			}
			memset(i1source, 0, (bipi1->maxId+1)*sizeof(double));
			for (i=0; i<bipi2->maxId + 1; ++i) {
				if (i2source[i]) {
					degree = bipi2->count[i];
					source = i2source[i]/(double)degree;
					for (j=0; j<degree; ++j) {
						neigh = bipi2->edges[i][j];
						i1source[neigh] += source;
					}
				}
			}
			for (i=0; i<bipi1->maxId + 1; ++i) {
				if (i1source[i] && i!=k) {

					i1[linesNum] = k;
					i2[linesNum] = i;
					d1[linesNum] = i1source[i];
					++linesNum;

					if (linesNum == con) {
						con += 1000000;
						int *temp = realloc(i1, con*sizeof(int));
						assert(temp != NULL);
						i1 = temp;
						temp = realloc(i2, con*sizeof(int));
						assert(temp != NULL);
						i2 = temp;
						double *tmp = realloc(d1, con*sizeof(double));
						assert(tmp != NULL);
						d1 = tmp;
					}
				}
			}
		}
	}

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;
	printf("calculate mass similarity done.\n");
	free(i1source);
	free(i2source);
	return simfile;
}

#endif
