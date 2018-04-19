//
//  SPQRTree.hpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#ifndef SPQRTree_hpp
#define SPQRTree_hpp

#include "SPQRTreeElement.hpp"

/**
 A class that shows the SPQR tree. It's composed of leaves, which show an electric circuit components, and nodes, whih show an electric circuit adaptors. It should be set binary tree before converting to the WDFTree.
 */
class SPQRTree {
public:
	SPQRTree();
	~SPQRTree();
	
	//============================================================
	// Manage the nodes & leaves
	//============================================================
	/**
	 Add a leaf to the tree as root(root-leaf)

	 @param edge The edge is converted to the leaf
	 */
	void AddRootLeaf(const Edge &edge);
	
	/**
	 Add a leaf to the tree as root(root-leaf)

	 @param edge The edge is converted to the leaf
	 */
	void AddRootLeaf(const Edge* const edge);
	
	/**
	 Add a node as child of the root. If there is no root, the new node is set to the root(root-node)

	 @param type the type of node
	 */
	void AddRootNode(const SPQRTreeNodeType type);
	
	/**
	 Add a leaf to the tree at the child of root node

	 @param edge The edge is converted to the leaf
	 @param type Create a new node if there is no root-node with this type
	 */
	void AddLeaf(const Edge* const edge, const SPQRTreeNodeType type);
	
	//============================================================
	// Properties of tree
	//============================================================
	/**
	 Print a tree
	 */
	void Print();
	
	/**
	 Get the root of tree. It's a leaf or node

	 @return the root element
	 */
	SPQRTreeElement* GetRoot();
	
	/**
	 Get the root-leaf of tree

	 @return the leaf at root position.
	 */
	SPQRTreeLeaf* GetRootLeaf();
	
	/**
	 Get the roo-node of tree

	 @return the node ar root position
	 */
	SPQRTreeNode* GetRootNode();
	
	/**
	 Get other child on same level(having same parent). This works on the binary tree only.

	 @param child a tree element to be searched
	 @return the pointer to the other child
	 */
	SPQRTreeElement* GetChildOnSameLevel(const SPQRTreeElement* child);
	
	/**
	 Get the index of tree element in the children array

	 @param child a tree element to be searched
	 @return the index of that element
	 */
	int GetIndexInChildren(const SPQRTreeElement* child);
	
	/**
	 Create the id of tree node

	 @param type the type of node
	 @return the new id
	 */
	string CreateNodeId(const SPQRTreeNodeType type);
	
	//============================================================
	// Edit the tree
	//============================================================
	/**
	 Join the tree as sub

	 @param subTree the tree to be subbed
	 */
	void JoinSubTree(SPQRTree &subTree);
	
	/**
	 Convert to the binary tree
	 */
	void ConvertToBinaryTree();
	
	/**
	 Set a rigid node as root
	 */
	void SetRigidNodeAsRoot();
	
	/**
	 Split all nonlinear elements from the tree then store in the array
	 */
	void SplitNonlinearElements();
	
	/**
	 Set the node as root

	 @param node a new root
	 */
	void SetNodeAsRoot(SPQRTreeNode* node);
	
	/**
	 Set the leaf as root

	 @param leaf a new root
	 */
	void SetLeafAsRoot(SPQRTreeLeaf* leaf);
	
	/**
	 Couple the two tree elements to parent-child relationship

	 @param parent the tree element to be set as parent
	 @param child the tree element to be set as child
	 @param atFirst Set the index of this child 0
	 */
	void Couple(SPQRTreeElement* parent, SPQRTreeElement* child, bool atFirst=false);
	
	/**
	 Decouple the two tree elements

	 @param elem1 a tree element
	 @param elem2 another tree element
	 */
	void Decouple(SPQRTreeElement* elem1, SPQRTreeElement* elem2);
	
	/**
	 Remove the node if having no child, merge the node to the parent if having one child. This function works only in binary tree
	 */
	void Trim();
	
	/**
	 Change the type of root-node as rigid
	 */
	void ChangeRootTypeRigid();
	
	/**
	 Remove all dummy leaves
	 */
	void RemoveDummyLeaves();
	
	/**
	 Add the tree elements in nonlinear-array to the children of root
	 */
	void AddNonlinearElementsToRoot();
	
	/**
	 Merge the children which have same vertex pair.
	 */
	void MergeChildrenInRoot();
	
	/**
	 Set the resistance of voltage source by merging the voltage source and the resistor if they are serialized
	 */
	void SetResistanceOfVoltageSource();
	
	//============================================================
	// etc.
	//============================================================
	/**
	 Set the poles of electric circuit.
	 */
	void SetPoles();
	
	/**
	 Set the pair of vertex of all elements
	 */
	void SetVertexPair();
	
	//============================================================
	// WDF Tree
	//============================================================
	/**
	 Set the value of input voltage

	 @param fInputVoltage input voltage
	 */
	void SetInputVoltage(float fInputVoltage);
	
	/**
	 Set the value of input frequency

	 @param fInputFrequency input frequency
	 */
	void SetInputFrequency(float fInputFrequency);
	
	/**
	 Create the WDF tree

	 @param fSamplingTime sampling period
	 @return a new tree
	 */
	WDFTree* CreateWDFTree(float fSamplingTime);
	
protected:
	/**
	 an array that stores the all tree elements of the tree
	 */
	vector<SPQRTreeElement*> vecElements;
	
	/**
	 an array that stores the nonlinear elements of the tree
	 */
	vector<SPQRTreeLeaf*> vecNonlinearElements;
	
	/**
	 an input voltage of the electric circuit
	 */
	float fInputVoltage;
	
	/**
	 an input frequency of the electric circuit
	 */
	float fInputFrequency;
};

#endif /* SPQRTree_hpp */
