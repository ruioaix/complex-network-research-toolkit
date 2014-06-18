#ifndef CNRT_NET_H
#define CNRT_NET_H

#include "linefile.h"

struct netAttr_i{
	int sign;
	int value;
};

struct netAttr_d{
	int sign;
	double value;
};

struct netAttr_l{
	int sign;
	long value;
};

struct netAttr_ip{
	int sign;
	int *p;
};

struct netAttr_dp{
	int sign;
	double *p;
};

struct Net{
	//if countMax.sign == 1, then countMax.value is valid.
	//if countMax.sign == 0, then countMax.value can not be used.
	//sign should only be 0 or 1, for now: 20140618. the other values are reserved.
	struct netAttr_l countMax;
	struct netAttr_l countMin;
	struct netAttr_d avesp;
	struct netAttr_dp weight;

	//the number of vertices.
	int idNum;
	//the maximum and minimum id of the vertices.
	//the scope of ids is [0, ∞), but the minId doesn't have to be 0.
	int maxId;
	int minId;
	//the number of edges.
	long edgesNum;
	//count[11] means the count of the vertex whose id is 11.
	long *degree;
	//edges[23][2] means the id of the third neighbour of the vertex whose id is 23.
	//edges[23] doesn't have to be a sorted array.
	int **edges;
	
	//direct == 0, undirect.
	//direct == 1, direct.
	//other reserved.
	int direct;
	//connectness == 0, not fully connected.
	//connectness == 1, fully connected.
	//connectness == 2, not sure whether the net is fully connected or not.
	//other reserved.
	int connectness;
	//duplicatepairs == 0, no duplicatepairs.
	//duplicatepairs == 1, has duplicatepairs.
	//duplicatepairs == 2, not sure whether there are any duplicate pairs or not.
	//other reserved.
	int duplicatepairs;
};

//free net, net itself is dynamically allocated.
void free_Net(struct Net *net);
//LineFile is another core structure, more basic.
struct Net *create_Net(const struct LineFile * const file);
struct Net *read_Net(char *netfilename);
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

#endif
