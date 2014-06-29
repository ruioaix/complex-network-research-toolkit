#ifndef CN_TDPOTN_H
#define CN_TDPOTN_H

#include "linefile.h"
#include "net.h"

void tdpotn_argcv(int argc, char **argv, int *D_12, int *N, int *seed, int *limitN, double *theta, double *lambda);
struct LineFile * tdpotn_lf(int D_12, int L);
struct LineFile *tdpotn_create_air(struct Net * net, double alpha, int limitN, double theta, double lambda);
void tdpotn_print(int d12, int N, int seed, int limitN, double theta, double lambda, double alpha, double avesp, double coupling, double gini);
//void tdpotn_writenettofile_ii_iid(struct iiNet *base, struct iidNet *air, char *filename, char *wfilename);

#endif
