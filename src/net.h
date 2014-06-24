#ifndef CNRT_NET_H
#define CNRT_NET_H

#include "linefile.h"

typedef int vertex_t;
#define VERTEX_T_MAX INT_MAX 
typedef int edge_t;
#define EDGE_T_MAX INT_MAX


enum NET_STATUS {
	NS_NON_VALID, NS_VALID,
	NS_DIRECTED, NS_UNDIRECTED,
	NS_WEIGHTED, NS_UNWEIGHTED,
	NS_DUPPAIRS, NS_NON_DUPPAIRS, 
	NS_CNNTNESS, NS_NON_CNNTNESS, 
	NS_NOTSURE
};

struct netOption_d{
	enum NET_STATUS sign;
	double value;
};

struct netOption_edge{
	enum NET_STATUS sign;
	edge_t value;
};

struct netOption_edge_pp{
	enum NET_STATUS sign;
	edge_t **pp;
};

/*
 * there are a lot of options, some optins may seem important and should not be only an option.
 * the reason I put them in options is :
 * 		may be not existed by default.
 * 		may be memory-greedy 
 * 		may be time-greedy(computing-complex)
 * 		may be difficult to maintain when modifying the net.
 * and they will not used by me frequently.
 *
 * if net is directed, 
 * 		diffence between degree&indegree, edges&inedges, weight&inweight is obvious.
 * if net is undirected,
 * 		degree == indegree, edges == inedges, weight == inweight.
 *
 * when net is undirected, each edge is stored twice, but edges&inedges point to the same address.
 * 		an edge between 34 and 22, there are edges[34][x] = 22 and edges[22][y] = 34. 
 * 		but inedges is point to the same memory block of edges.
 *
 * when net is directed, each edge is stored only once in edges, and  stored another time in inedges.
 * 		an edge frome 34 to 22, there is edges[34][x] = 22, but edges[22][y] = 34 maybe not existed.
 * 		but inedge[22][z] = 34 must be existed.
 *
 * so actually, the memory consumed by directed net is not quite different from that by undirected net.
 * 		
 */
struct Net{
	//the number of vertices.
	vertex_t idNum;
	//the maximum and minimum id of the vertices.
	//the scope of ids is [0, ∞), but the minId doesn't have to be 0.
	//"maxId - minId + 1" doen't have to be "idNum", hole is acceptable.
	vertex_t maxId;
	vertex_t minId;
	//the number of edges.
	edge_t edgesNum;
	//degree[11] means the degree of the vertex whose id is 11.
	edge_t *degree;
	edge_t *indegree;
	//edges[23][2] means the id of the third neighbour of the vertex whose id is 23.
	//edges[23] doesn't have to be a sorted array.
	vertex_t **edges;
	vertex_t **inedges;
	//same structure to edges.
	double **weight;
	double **inweight;
	
	//directStatus == NS_UNDIRECTED, undirect.
	//directStatus == NS_DIRECTED, direct.
	enum NET_STATUS directStatus;
	//weightStatus == NS_UNWEIGHTED, unweighted.
	//weightStatus == NS_WEIGHTED, weighted.
	enum NET_STATUS weightStatus;
	//connectnessStatus == NS_NON_CNNTNESS, not fully connected.
	//connectnessStatus == NS_CNNTNESS, fully connected.
	//connectnessStatus == NS_NOTSURE, not sure whether the net is fully connected or not.
	enum NET_STATUS connectnessStatus;
	//duplicatepairsStatus == NS_NON_DUPPAIRS, no duplicatepairs.
	//duplicatepairsStatus == NS_DUPPAIRS, has duplicatepairs.
	//duplicatepairsStatus == NS_NOTSURE, not sure whether there are any duplicate pairs or not.
	enum NET_STATUS duplicatepairsStatus;

	//if degreeMax.sign == NS_VALID, then degreeMax.value is valid.
	//if degreeMax.sign == NS_NON_VALID, then degreeMax.value can not be used.
	struct netOption_edge degreeMax;
	struct netOption_edge degreeMin;
	struct netOption_edge indegreeMax;
	struct netOption_edge indegreeMin;
	//average shortest path.
	struct netOption_d avesp;

	//be careful with this option. it's a matrix: (maxId + 1)^2 * sizeof(edge_t) BYTE.
	//this option should be used when you have quite sufficient memory space and you want to find a edge ASAP.
	//e.g. if you want to find a edge's weight, the edge is from 23 to 77.
	//		then the value of edgesMatrix.pp[23][77] is 55; 
	//		then the value of weight.pp[23][55] is 43.23;
	//		then 43.23 is the weight of the edge.
	//		if edgesMatrix.pp[23][77] is -1, it means there is no edge from 23 to 77.
	struct netOption_edge_pp edgesMatrix;
};

//free net, net itself is dynamically allocated.
void free_Net(struct Net *net);
//LineFile is another core structure, more basic.
/*
 * create_Net is undirected and unweighted.
 * create_directed_Net is directed and unweighted.
 * create_weighted_Net is undirected and weighted.
 * create_directed_weighted_Net is directed and weighted.
 */
struct Net *create_Net(struct LineFile * file);
struct Net *create_directed_Net(struct LineFile * file);
struct Net *create_weighted_Net(struct LineFile * file, double *weight);
struct Net *create_directed_weighted_Net(struct LineFile * file, double *weight);

void set_edgesMatrix_Net(struct Net *net);
void check_connectness_Net(struct Net *net);

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
