/**
 * recommandation is carried on between user and item, always two part.
 * maybe a biparte can contain the third part, like score that a user give to a item. but the third part (even another forth part) can be treated as a param.
 * this is more flexible.
 *
 * for bip:
 * 		the origin bipartite is coming from a struct LineFile with using create_Bip(struct LineFile) function.
 * 		(BTW, the origin LineFile is getting from a file which contain at least two int integer in an line, 
 * 		to a line containing three or more integers, only fetch two.  using create_LineFile(char *filename, 1, 1, -1))
 *
 * struct Metrics_Bip contains the results(all metrics) for all kinds of recommendation algorithm.
 *
 */
#ifndef CN_BIP_H
#define CN_BIP_H

#include "linefile.h"
#include "net.h"

//this struct is just one way for describing Bipartite.
//other ways like (int ***xx) is also usefully.
struct HalfBip {
	int maxId, minId, idNum;
	int degreeMax, degreeMin;
	int *degree, **edges;
};
struct Bip {
	long edgesNum;
	struct HalfBip *left;
	struct HalfBip *right;
};

void free_Bip(struct Bip *bip);
struct Bip *create_Bip(const struct LineFile * const file);
void sort_desc_half_Bip(struct HalfBip *hbip);

struct Bip * clone_Bip(struct Bip *bip);
//void verify_Bip(struct Bip *bipi1, struct Bip *bipi2);
void print_Bip(struct Bip *bip, char *filename);

//the dividation will guarantee that: 
//	for each available user(degree of this user is at least one), at least there will be one edge in big part.
//	for each available item(degree of this item is at least one), at least there will be one edge in big part.
//so maybe some users/items are not existed in the small part.
void divide_Bip(struct Bip *bip, double rate, struct LineFile **small_part, struct LineFile **big_part);

//if target == 1, then calculate i1(mostly could be user)'s similarity.
//if target == 2, then calculate i2(mostly could be item)'s similarity.
struct LineFile *similarity_CF_HalfBip(struct HalfBip *hbip, int other_half_maxId);
//struct LineFile *mass_similarity_Bip(struct Bip *bipi1, struct Bip *bipi2);


struct MetricsBip {
	double R;
	double PL;
	double HL;
    double IL;
    double NL;
	int *topL;
	int L;
};
struct MetricsBip *create_MetricsBip(void);
void clean_MetricsBip(struct MetricsBip *m);
void free_MetricsBip(struct MetricsBip *m);

//recommend methods

struct MetricsBip *mass_Bip(struct Bip *train, struct Bip *test, struct Net *trainSim);

struct MetricsBip *heats_Bip(struct Bip *train, struct Bip *test, struct Net *trainSim);

struct MetricsBip *HNBI_Bip(struct Bip *train, struct Bip *test, struct Net *trainSim, double HNBI_param);

struct MetricsBip *RENBI_Bip(struct Bip *train, struct Bip *test, struct Net *trainSim, double RENBI_param);

struct MetricsBip *hybrid_Bip(struct Bip *train, struct Bip *test, struct Net *trainSim, double hybrid_param);

//int *mass_getBK_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, double rate);

//struct Metrics_Bip *mass_topk_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk);
//struct Metrics_Bip *mass_hs_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk);

//score bip
//struct Metrics_Bip *mass_score_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, int maxscore, double mass_score);
//struct Metrics_Bip *mass_scoret3step_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double mass_score);
//struct Metrics_Bip *mass_degree_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim,  double mass_score);

//rank
//double *mass_score_rank_Bip(struct Bip *traini1, struct Bip *traini2, int maxscore, double mass_score);
//double *mass_scoret3step_rank_Bip(struct Bip *traini1, struct Bip *traini2, double mass_score);
//double *mass_degree_rank_Bip(struct Bip *traini1, struct Bip *traini2, double mass_score);

#endif
