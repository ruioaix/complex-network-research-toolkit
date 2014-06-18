#ifndef CNRT_NET_H
#define CNRT_NET_H

#include "linefile.h"

enum NET_STATUS {
	NS_NON_VALID, NS_VALID,
	NS_DIRECTED, NS_UNDIRECTED,
	NS_DUPPAIRS, NS_NON_DUPPAIRS, NS_NOTSURE_DUPPAIRS,
	NS_CNNTNESS, NS_NON_CNNTNESS, NS_NOTSURE_CNNTNESS
};

struct netOption_i{
	enum NET_STATUS sign;
	int value;
};

struct netOption_d{
	enum NET_STATUS sign;
	double value;
};

struct netOption_l{
	enum NET_STATUS sign;
	long value;
};

struct netOption_lp{
	enum NET_STATUS sign;
	long *p;
};

struct netOption_ip{
	enum NET_STATUS sign;
	int *p;
};

struct netOption_dp{
	enum NET_STATUS sign;
	double *p;
};

struct netOption_dpp{
	enum NET_STATUS sign;
	double **pp;
};

/*
 * there are a lot of options, some optins may seem important and should not be only an option.
 * the reason I put them in options is :
 * 		may be memory-greedy 
 * 		may be time-greedy(computing-complex)
 * 		may be difficult to maintain in some cases.
 * and they will not used by me frequently.
 */
struct Net{
	//if degreeMax.sign == NATTR_VALID, then degreeMax.value is valid.
	//if degreeMax.sign == NATTR_NON_VALID, then degreeMax.value can not be used.
	struct netOption_l degreeMax;
	struct netOption_l degreeMin;
	struct netOption_l indegreeMax;
	struct netOption_l indegreeMin;
	struct netOption_d avesp;
	struct netOption_dpp weight;

	//the number of vertices.
	int idNum;
	//the maximum and minimum id of the vertices.
	//the scope of ids is [0, ∞), but the minId doesn't have to be 0.
	int maxId;
	int minId;
	//the number of edges.
	long edgesNum;
	//degree[11] means the degree of the vertex whose id is 11.
	long *degree;
	long *indegree;
	//edges[23][2] means the id of the third neighbour of the vertex whose id is 23.
	//edges[23] doesn't have to be a sorted array.
	int **edges;
	int **inedges;
	
	//direct == NATTR_UNDIRECTED, undirect.
	//direct == NATTR_DIRECTED, direct.
	enum NET_STATUS directStatus;
	//connectness == NATTR_NON_CNNTNESS, not fully connected.
	//connectness == NATTR_CNNTNESS, fully connected.
	//connectness == NATTR_NOTSURE_CNNTNESS, not sure whether the net is fully connected or not.
	enum NET_STATUS connectnessStatus;
	//duplicatepairs == NATTR_NON_DUPPAIRS, no duplicatepairs.
	//duplicatepairs == NATTR_DUPPAIRS, has duplicatepairs.
	//duplicatepairs == NATTR_NOTSURE_DUPPAIRS, not sure whether there are any duplicate pairs or not.
	enum NET_STATUS duplicatepairsStatus;
};

//free net, net itself is dynamically allocated.
void free_Net(struct Net *net);
//LineFile is another core structure, more basic.
struct Net *create_directed_Net(struct LineFile * file);
struct Net *create_undirected_Net(struct LineFile * file);
/*
struct Net *read_direct_Net(char *netfilename);
struct Net *read_undirect_Net(char *netfilename);
void write_Net(struct Net *net, char *filename);

int delete_vertex_Net(struct Net *net, int vid);
//if vid contain some vextices which are non-existed in net, these vextices are ignored.
int delete_vertices_Net(struct Net *net, int *vid, int number);
int delete_edge_Net(struct Net *net, int v1, int v2);
//if v1&v2 contain some edges which are not existed in net, then these edges are ignored.
//only delete available edges. so allow multi-same edges.
int delete_edges_Net(struct Net *net, int *v1, int *v2, int number);

//if you can not make sure whether there are any duplicatepairs or not, you can use this
//function to make sure net is clean.
void delete_duplicatepairs_Net(struct Net *net);
void check_connectness_Net(struct Net *net);
*/

#endif
