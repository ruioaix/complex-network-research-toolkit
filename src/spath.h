#ifndef CNRT_SPATH_H
#define CNRT_SPATH_H

#include "net.h"
double *dijkstra_1A_undirected_weighted_Net(struct Net *net, int nid);
double dijkstra_avesp_undirected_weighted_Net(struct Net *net);

vertex_t *spath_1A_undirect_unweight_Net(struct Net *net, vertex_t originId);
void spath_1A_step_undirect_unweight_Net(struct Net *net, vertex_t originId, vertex_t step, vertex_t *Num, vertex_t **ret);
void spath_avesp_undirect_unweight_Net(struct Net *net, double *avesp);

void spath_avesp_coupling_undirect_unweight_2_Net(struct Net *base, struct Net *air, double *avesp, double *coupling);

/*
 * standard shortest path algorithm.
 */
//double *dijkstra_1A_iidNet(struct iidNet *net, int nid);

/*
 * following is some shortest-path algorithms for two very restrictive network.
 * with following prerequisite:
 * 1, base is iinet.
 * 2, air can be iinet or iidnet.
 * 3, air's nodes is a subset of base's nodes.
 * 4, in iinet network, egdes don't have weight, or we can think all weight is 1.
 * 5, but in iidnet network, edges has weight which has to be not less than 1.
 * 6, base is fullly contected.
 *
 */
/*
 * spath01_1A_iiNet return a array which contains all possible shortest pathes.
 * let's say the returned array is sp, then sp's length is "base->maxId + 1". the initial value of each element is -1.
 * I mean, if the length of shortest path between node A and node B is 5, then sp[5] is -1.
 *         if the length of shortest path between node C and node B is 7, then 7 is in the returned array.
 *         ...
 */
//int *spath01_step_1A_iiNet(struct iiNet *net, int originId, int step, int *Num);
//void avesp_spath01_iiNet(struct iiNet *net, double *avesp);

//this spath02 is FW algorithm for unweighted and undirected net.
//int **spath02_AA_iiNet(struct iiNet *net);

//this spath03 is for unweighted_undirected base and weighted_undirected air network.
//the weight of each link in air network is >= 1.0.
//void avesp_spath03_Net(struct iiNet *net, struct iidNet *air, double *avesp);

//this spath04 is for unweighted and undirected net.
//to find how many shortest pathes use only base or only air or both.
//void useRate_spath04_iiNet(struct iiNet *net, struct iiNet *air, double *useRate, double *sameRate, double *cleanRate, double *avesp);

//this spath05 is for unweighted and undirected net.
//to find coupling of two net: base and air.
//void coupling_spath05_iiNet(struct iiNet *net, struct iiNet *air, double *coupling, double *avesp);

//this spath06 is for unweighted_undirected base and weighted_undirected XE network.
//to find gini of two net: base and XE.
//void gini_spath06_Net(struct iiNet *base, struct iidNet *XE, double *avesp, double *gini);
#endif
