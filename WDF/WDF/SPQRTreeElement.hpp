//
//  SPQRTreeElement.hpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#ifndef SPQRTreeElement_hpp
#define SPQRTreeElement_hpp

#include "WDFTree.hpp"
#include "GraphElement.hpp"

/**
 the number of properties of the electric circuit component
 */
#define NUM_OF_PROPS	8

/**
 the number of options of the electric circuit component
 */
#define NUM_OF_OPTS		8

typedef string					Vertex;
typedef pair<Vertex, Vertex>	VertexPair;

/**
 An enumeration for the type of the node of spqr tree
 */
enum class SPQRTreeNodeType
{
	SERIES,
	PARALLEL,
	RIGID
};

/**
 A class that indicate the basic element of SPQR tree
 */
class SPQRTreeElement
{
public:
	/**
	 the id of element
	 */
	string id;
	
	/**
	 the parent element of this
	 */
	SPQRTreeElement* parent;
	
	/**
	 the child elements of this
	 */
	vector<SPQRTreeElement*> children;
	
	/**
	 a pair of vertices which are adjacent in the Graph
	 */
	VertexPair vertex_pair;
	
	/**
	 An array that contains the properties of WDF object
	 */
	float	values[NUM_OF_PROPS];
	
	/**
	 An array that contains the options, used in the WDF tree, of WDF object
	 */
	bool	options[NUM_OF_OPTS];
	
	/**
	 Create an empty element
	 */
	SPQRTreeElement();
	
	/**
	 Create an element with id
	 */
	SPQRTreeElement(string id);
	
	virtual ~SPQRTreeElement();
	
	/**
	 Create an WDF object then add to the WDF tree. This function starts to perform from the children recursively, so will be used at the root once.

	 @param wdfTree the WDF tree to which the created WDF object is added
	 */
	virtual void CreateWDFObject(WDFTree* wdfTree) = 0;
	
	/**
	 Set the pair of vertices. The leaf is has own vertex pair, though the node create the pair from the children.
	 */
	virtual void SetVertexPair(vector<VertexPair> * remainedVertexPairs) = 0;
	
	/**
	 Get the prefix of id
	 */
	string GetPrefix();
	
	/**
	 Merge two vertex paris. This function stores a new vertex pair to this element

	 @param pair a vertex pair to be merged
	 */
	void MergeVertexPair(VertexPair pair);
};

/**
 A class that indicates a node in the SPQR tree. There are three kinds of types, leaves or nodes as children/parent, and can be converted to an WDF adaptor.
 */
class SPQRTreeNode: public SPQRTreeElement {
public:
	/**
	 Create an empty instance
	 */
	SPQRTreeNode();
	
	/**
	 Create an instance with id and type
	 
	 @param id the id of node
	 @param type the type of node
	 */
	SPQRTreeNode(string id, const SPQRTreeNodeType type);
	
	~SPQRTreeNode();
	
	/**
	 Set the type of node

	 @param type new type of node
	 */
	void SetType(const SPQRTreeNodeType type);
	
	/**
	 Get the type of node

	 @return the type of node
	 */
	SPQRTreeNodeType GetType();
	
	/**
	 Set the number of nonlinear elements connected to this

	 @param nNonlinear the number of nonlinear elements
	 */
	void SetNonlinearElementCount(unsigned int nNonlinear);
	
	/**
	 Get the number of nonlinear elements connected to this

	 @return the number of nonlinear elements
	 */
	unsigned int GetNonlinearElementCount();
	
	/**
	 Sort the nonlinear elements of children

	 @param ascending true if ascending
	 */
	void SortNonlinearElements(bool ascending=true);
	
	/**
	 Compare the two SPQR tree elements using id
	 
	 @param elem1 an element to be compared
	 @param elem2 an element to be compared
	 @return True if the id is lesser
	 */
	static bool Compare(SPQRTreeElement* elem1, SPQRTreeElement* elem2);
	
	/**
	 Sort the children

	 @param ascending true if ascending
	 */
	void SortChildren(bool ascending=true);
	
	/**
	 Get the vertex "unmatched", which means it exists in the table but no child includes it.
	 
	 @return an unmatched vertex
	 */
	string GetUnmatchedPairId();
	
	/**
	 Get the pair of vertices in the table using the id of edge
	 
	 @param edgeId the id of edge which connects two vertices
	 @return the pair of vertices
	 */
	VertexPair GetVertexPair(const string edgeId);
	
	/**
	 Add a pair of vertices
	 
	 @param edgeId the id of edge which connects the two vertices
	 @param pair a pair of vertices
	 */
	void AddVertexPair(const string edgeId, const VertexPair &pair);
	
	/**
	 Replace a existing pair of vertices to new one
	 
	 @param existingId the id of existing edge
	 @param newId the id of new edge
	 */
	void ReplaceVertexPair(const string existingId, const string newId);
	
	/**
	 Remove a pair of vertices
	 
	 @param edgeId the id of edge to be deleted
	 */
	void RemovePair(const string edgeId);
	
	// Virtual functions
	void CreateWDFObject(WDFTree* wdfTree);
	void SetVertexPair(vector<VertexPair> * remainedVertexPairs);
	
protected:
	/**
	 Sort the array using quicksort algorithm

	 @param array an array to be sorted
	 @param left left index
	 @param right right index
	 @param ascending option for the compare. Set true if ascending.
	 */
	void QuickSort(vector<SPQRTreeElement*> array, unsigned int left, unsigned int right, bool ascending);
	
	/**
	 the type of this node
	 */
	SPQRTreeNodeType type;
	
	/**
	 the number of nonlinear elements which are connected to this node
	 */
	unsigned int nNonlinearCount;
	
	/**
	 the map of vertex pairs adjacent each other and the edge id which connects the vertex pair
	 */
	map<string, VertexPair> rigidVertexTable;
	
	/**
	 a table that converts the vertex to the index of child in rigid node.
	 */
	map<Vertex, int> rigidIndexTable;
};

/**
 A class that indicates a leaf of SPQR tree. There are many kinds of types that can be converted to the WDF leaf.
 */
class SPQRTreeLeaf: public SPQRTreeElement {
public:
	SPQRTreeLeaf();
	
	/**
	 Create an instance of the leaf of SPQR tree with edge. Take the properties, options and vertex-pair from the edge.
	 */
	SPQRTreeLeaf(const Edge* const edge);
	
	~SPQRTreeLeaf();
	
	// Virtual functions
	void CreateWDFObject(WDFTree* wdfTree);
	void SetVertexPair(vector<VertexPair> * remainedVertexPairs);
};

#endif /* SPQRTreeElement_hpp */
