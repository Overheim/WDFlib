//
//  Graph.hpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#ifndef Graph_hpp
#define Graph_hpp

#include "SPQRTree.hpp"
#include <set>

typedef set<Vertex>				VertexSet;

typedef set<Edge*>				EdgeSet;

typedef pair<Edge*, Vertex>		Element;
typedef set<Element>			ElementSet;
typedef map<Vertex, ElementSet>	Map;

/**
 A type of graph
 */
enum class GraphType
{
	TRIVIAL = 0,
	SERIES,
	PARALLEL,
	RIGID
};

/**
 A class that creates the graph with edge and vertex and convert a graph to the SPQR tree.
 */
class Graph
{
public:
	/**
	 Create an empty instance of graph
	 */
	Graph();
	
	/**
	 Duplicate a graph
	 */
	Graph(Graph* const graph);
	
	~Graph();
	
	//============================================================
	// Manage the edges and vertices
	//============================================================
	/**
	 Add an edge between two vertices

	 @param vertex1 a vertex
	 @param vertex2 another vertex
	 @param wdfEdge an edge connects two vertices
	 */
	void AddEdge(const Vertex &vertex1, const Vertex &vertex2, const Edge &wdfEdge);
	
	/**
	 Add an edge between two vertices

	 @param vertex1 a vertex
	 @param vertex2 another vertex
	 @param edge a pointer to the edge which connects two vertices
	 */
	void AddEdge(const Vertex &vertex1, const Vertex &vertex2, Edge* edge);
	
	/**
	 Remove an edge

	 @param edge an edge to be removed
	 */
	void RemoveEdge(const Edge* const edge);
	
	/**
	 Remove a vertex from the graph

	 @param vertex a vertex to be removed
	 */
	void RemoveVertex(const Vertex &vertex);
	
	/**
	 Get all vertices in the graph
	 
	 @param vertices a pointer to the set of vertices
	 */
	void GetVertices(VertexSet* vertices);
	
	/**
	 Get all edges in the graph
	 
	 @param edges a pointer to the set of edges
	 */
	void GetEdges(EdgeSet* edges);
	
	//============================================================
	// Check path
	//============================================================
	/**
	 Check whether a route exists from a vertex to another vertex

	 @param startVertex a vertex at startpoint
	 @param endVertex a vertex at endpoint
	 @return true if existing route, or false
	 */
	bool HasPath(const Vertex &startVertex, const Vertex &endVertex);
	
	//============================================================
	// Properties of graph
	//============================================================
	/**
	 Check the two vertices is adjacent, which means there is an edge connects the vertices

	 @param vertex1 a vertex
	 @param vertex2 another vertex
	 @return true if adjacent, or false
	 */
	bool IsAdjacent(const Vertex &vertex1, const Vertex &vertex2);
	
	/**
	 Check the graph is biconnected

	 @return true if biconnected, or false
	 */
	bool IsBiconnected();
	
	/**
	 Find a reference edge

	 @param referenceEdge a reference edge
	 @param referenceVertexPair a vertex pair which the reference edge connects
	 */
	void FindReferenceEdgeWithVertexPair(Edge* referenceEdge, VertexPair* referenceVertexPair);
	
	/**
	 Get the separation pairs

	 @param separationPair a pointer to the vector which stores the separation pairs
	 @return the number of separation pairs
	 */
	unsigned int GetSeparationPairs(vector<VertexPair> * const separationPair);
	
	/**
	 Get the split pairs

	 @param splitPair a pointer to the vector which stores the split pairs
	 @return the number of split pairs
	 */
	unsigned int GetSplitPairs(vector<VertexPair> * const splitPair);
	
	/**
	 Get the cutsets

	 @param cutset a pointer to the set of vertices which stores the cutsets
	 @return the number of cutsets
	 */
	unsigned int GetCutset(VertexSet* const cutset);
	
	/**
	 Get the type of graph

	 @param referenceVertexPair the reference edge of the graph
	 @return the type of graph
	 */
	GraphType GetType(const VertexPair &referenceVertexPair);
	
	/**
	 Check the inclustion relationship

	 @param graph another graph
	 @return True if this graph includes another graph, or false
	 */
	bool Contains(Graph &graph);
	
	/**
	 Print a graph
	 */
	void Print();
	
	//============================================================
	// Manages the graph
	//============================================================
	/**
	 Split the graph with the vertex pair

	 @param splitPair The graph is split by this pair
	 @param splitComponents a vector of graphs which is split by the pair of vertices
	 */
	void SplitByVertexPair(const VertexPair &splitPair, vector<Graph*> * splitComponents);
	
	/**
	 Split the graph with the vertex

	 @param cutVertex The graph is split by this vertex
	 @param splitComponents a vector of graphs which is split by the pair of vertices
	 */
	void SplitByCutVertex(const Vertex &cutVertex, vector<Graph*> * splitComponents);
	
	/**
	 Split the rigid graph by maximal pairs

	 @param wdfReferenceEdge the reference edge of this graph
	 @param splitComponents a pointer to the vector of graphs which is split by maximal pairs
	 @param splitPairs a pointer to the vector of the pairs of vectices which split the graph
	 */
	void SplitMaximal(const Edge &wdfReferenceEdge, pair<Graph*, vector<Graph*>> * splitComponents, vector<VertexPair> * splitPairs);
	
	//============================================================
	// Make SPQR tree
	//============================================================
	/**
	 Create a SPQR tree from this graph

	 @param tree a pointer to the SPQR tree to be created
	 */
	void MakeSPQRTree(SPQRTree *tree);
	
	/**
	 Create a SPQR tree with the reference edge

	 @param tree a pointer to the SPQR tree to be created
	 @param referenceEdge the reference of this graph
	 @param referenceVertices the pair of vertices that reference edge connects
	 */
	void MakeTree(SPQRTree *tree, const Edge &referenceEdge, const VertexPair &referenceVertices);
	
protected:
	/**
	 Shows a connections between vertices
	 */
	Map mMap;
	
	/**
	 A set of all edges in the graph
	 */
	EdgeSet mEdges;
};

#endif /* Graph_hpp */
