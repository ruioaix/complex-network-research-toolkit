/**
 * this file is used to generate all kinds of data set.
 * the data set will be represented with struct LineFile or just print into file.
 */
#ifndef CN_DATASET_H
#define CN_DATASET_H

#include "linefile.h"

enum DS_STATUS {
	DS_CYCLE, DS_NON_CYCLE,
	DS_DIRECT, DS_NON_DIRECT
};

struct LineFile * lattice2d_DS(int L, enum DS_STATUS cycle, enum DS_STATUS direct);
struct LineFile * line1d_DS(int L, enum DS_STATUS cycle, enum DS_STATUS direct);

//ER random network
struct LineFile *ER_DS(int N, int seed);
//scare free network
struct LineFile *SF_DS(int N, int seed, int MM0);


#endif
