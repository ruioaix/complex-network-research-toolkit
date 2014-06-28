#ifndef CNRT_NET_H
#define CNRT_NET_H

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

struct netOption_i{
	enum NET_STATUS sign;
	int value;
};

struct netOption_ipp{
	enum NET_STATUS sign;
	int **pp;
};

/*
 * there are a lot of options, some optins may seem important and should not be only an option.
 * the reason I put them in options is :
 * 		may be not existed by default.
 * 		may be memory-greedy 
 * 		may be time-greedy(computing-complex)
 * 		may be difficult to maintain when modifying the net.
 * 		or they will not used by me frequently.
 *
 * if net is directed, 
 * 		diffence between degree&indegree, edges&inedges, weight&inweight is obvious.
 * if net is undirected,
 * 		NULL == indegree == inedges == inweight.
 *
 * when net is undirected, each edge is stored twice.
 * 		an edge between 34 and 22, there are edges[34][x] = 22 and edges[22][y] = 34. 
 *
 * when net is directed, each edge is stored only once in edges, and  stored another time in inedges.
 * 		an edge frome 34 to 22, there is edges[34][x] = 22, but edges[22][y] = 34 maybe not existed(depends on whether there is an edge from 22 to 34.
 * 		but inedge[22][z] = 34 is existed.
 *
 * if weight != NULL, then the Net is weighted.
 * if weight == NULL, then the Net is unweighted.
 * it's quite clear, so I don't need to set a weightStatus.
 * but this is not suit for directStatus.
 * because even in a directed net, edges element can store all the informations.
 * so inedges element is not essential, thus I can not check the value of inedges to decide whether the net is directed or not.
 *
 */
struct Net{
	//the number of vertices.
	int idNum; //essential
	//the maximum and minimum id of the vertices.
	//the scope of ids is [0, ∞), but the minId doesn't have to be 0.
	//"maxId - minId + 1" doen't have to be "idNum", hole is acceptable.
	int maxId; //essential
	int minId; //essential
	//the number of edges.
	long edgesNum; //essential
	//degree[11] means the degree of the vertex whose id is 11.
	int *degree; //essential
	//indegree maybe NULL even in a directed net, it's not essential
	int *indegree;
	//edges[23][2] means the id of the third neighbour of the vertex whose id is 23.
	//edges[23] doesn't have to be a sorted array.
	int **edges; //essential
	//inedges maybe NULL even in a directed net, it's not essential
	int **inedges;
	//same structure to edges.
	//the values are got from d1 of LineFile.
	double **weight;
	double **inweight;
	//same structure to edges.
	//this will not be used often, but when an edge has some extra attribute, this is the place storing that.
	//the values are got from d2 of LineFile.
	double **edgesAttr;
	double **inedgesAttr;
	
	//directStatus == NS_UNDIRECTED, undirect.
	//directStatus == NS_DIRECTED, direct.
	enum NET_STATUS directStatus;
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
	struct netOption_i degreeMax;
	struct netOption_i degreeMin;
	struct netOption_i indegreeMax;
	struct netOption_i indegreeMin;
	//average shortest path.
	struct netOption_d avesp;

	//be careful with this option. it's a matrix: (maxId + 1)^2 * sizeof(int) BYTE.
	//this option should be used when you have quite sufficient memory space and you want to find a edge ASAP.
	//e.g. if you want to find a edge's weight, the edge is from 23 to 77.
	//		then the value of edgesMatrix.pp[23][77] is 55; 
	//		then the value of weight[23][55] is 43.23;
	//		then 43.23 is the weight of the edge.
	//		if edgesMatrix.pp[23][77] is -1, it means there is no edge from 23 to 77.
	struct netOption_ipp edgesMatrix;

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
#include "linefile.h"
struct Net *create_Net(struct LineFile * file);
struct Net *create_directed_Net(struct LineFile * file);
struct Net *create_weighted_Net(struct LineFile * file);
struct Net *create_directed_weighted_Net(struct LineFile * file);

//generate the Matrix according to edges element.
//may be there will be a inedgesMatrix element later.
void set_option_edgesMatrix_Net(struct Net *net);
//test duplicatepairs only according to edges element.
void set_status_connectness_Net(struct Net *net);
//test duplicatepairs only according to edges element.
//if there is no duplicatepairs in edges element, then no duplicatepairs
//in the entire net.
void set_status_duplicatepairs_Net(struct Net *net);

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
