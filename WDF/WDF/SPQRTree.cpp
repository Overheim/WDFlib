//
//  SPQRTree.cpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#include "SPQRTree.hpp"

typedef vector<SPQRTreeElement*>	ElementVector;
typedef vector<SPQRTreeLeaf*>		LeafVector;

/**
 the index of node. It's used as id.
 */
unsigned int g_uiNodeIndex = 0;

SPQRTree::SPQRTree()
{
	fInputVoltage = 0.0;
	fInputFrequency = 0.0;
}

SPQRTree::~SPQRTree()
{
	// Clear all elements of SPQR tree
	for(ElementVector::iterator iter = vecElements.begin(); iter != vecElements.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
	
	for(LeafVector::iterator iter = vecNonlinearElements.begin(); iter != vecNonlinearElements.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
}

//============================================================
// Manage the nodes & leaves
//============================================================
void SPQRTree::AddRootLeaf(const Edge &edge)
{
	// Create a leaf
	SPQRTreeLeaf *leaf = new SPQRTreeLeaf(&edge);
	
	if(vecElements.size() != 0)
	{
		// Find a root
		SPQRTreeElement* root = GetRoot();
		
		// Set this leaf as parent
		Couple(leaf, root);
	}
	
	// Add to the list of tree elements
	vecElements.push_back(leaf);
}

void SPQRTree::AddRootLeaf(const Edge* const edge)
{
	// Create a leaf
	SPQRTreeLeaf *leaf = new SPQRTreeLeaf(edge);
	
	if(vecElements.size() != 0)
	{
		// Find a root
		SPQRTreeElement* root = GetRoot();
		
		// Set this leaf as parent
		Couple(leaf, root);
	}
	
	// Add to the list of tree elements
	vecElements.push_back(leaf);
}

void SPQRTree::AddRootNode(const SPQRTreeNodeType type)
{
	// Create a node
	SPQRTreeNode *node = new SPQRTreeNode(CreateNodeId(type), type);
	
	if(vecElements.size() != 0)
	{
		// Find a root
		SPQRTreeElement* root = GetRoot();
		
		// Set this node as parent
		Couple(root, node);
	}
	
	// Add to the list of tree elements
	vecElements.push_back(node);
}

void SPQRTree::AddLeaf(const Edge* const edge, const SPQRTreeNodeType type)
{
	// Create a leaf
	SPQRTreeLeaf *leaf = new SPQRTreeLeaf(edge);
	
	if(vecElements.size() != 0)
	{
		// Find or create a root-node
		SPQRTreeNode *rootNode = GetRootNode();
		if(!rootNode)
		{
			AddRootNode(type);
			rootNode = GetRootNode();
		}
		
		// Couple the new leaf and the root-node
		Couple(rootNode, leaf);
	}
	
	// add to the tree-objects list
	vecElements.push_back(leaf);
}

//============================================================
// Properties of tree
//============================================================
void SPQRTree::Print()
{
	for(ElementVector::iterator parentIter = vecElements.begin(); parentIter != vecElements.end(); parentIter++)
	{
		cout << (*parentIter)->id << ": ";
		for(ElementVector::iterator childIter = (*parentIter)->children.begin(); childIter != (*parentIter)->children.end(); childIter++)
		{
			if((*childIter)->parent == *parentIter)
				cout << (*childIter)->id << " ";
			else
				cout << (*childIter)->id << "(x) ";
		}
		cout << endl;
	}
	
	cout << "NLs: ";
	for(vector<SPQRTreeLeaf*>::iterator nlIter = vecNonlinearElements.begin(); nlIter != vecNonlinearElements.end(); nlIter++)
		cout << (*nlIter)->id << " ";
	cout << endl;
}

SPQRTreeElement* SPQRTree::GetRoot()
{
	// In case that there is no object
	if(vecElements.size() == 0)
		return NULL;
	
	SPQRTreeElement *root = vecElements[0];
	while(root->parent)
		root = root->parent;
	
	return root;
}

SPQRTreeLeaf* SPQRTree::GetRootLeaf()
{
	return dynamic_cast<SPQRTreeLeaf*>(GetRoot());
}

SPQRTreeNode* SPQRTree::GetRootNode()
{
	SPQRTreeElement* root = GetRoot();
	if(!root)
		return NULL;
	
	SPQRTreeNode* rootNode = dynamic_cast<SPQRTreeNode*>(root);
	if(rootNode)
		return rootNode;
	
	if(root->children.size() > 0)
		return dynamic_cast<SPQRTreeNode*>(root->children[0]);
	
	return NULL;
}

SPQRTreeElement* SPQRTree::GetChildOnSameLevel(const SPQRTreeElement* child)
{
	SPQRTreeElement* parent = child->parent;
	if(!parent || parent->children.size() <= 1)
		return NULL;
	
	for(ElementVector::iterator childIter = parent->children.begin(); childIter != parent->children.end(); childIter++)
	{
		if(*childIter != child)
			return *childIter;
	}
	
	return NULL;
}

int SPQRTree::GetIndexInChildren(const SPQRTreeElement* child)
{
	SPQRTreeElement* parent = child->parent;
	if(parent)
	{
		for(int i=0; i<parent->children.size(); i++)
			if(parent->children[i] == child)
				return i;
	}
	
	return -1;
}

//============================================================
// etc.
//============================================================
string SPQRTree::CreateNodeId(const SPQRTreeNodeType type)
{
	string prefix = "";
	switch(type)
	{
		case SPQRTreeNodeType::SERIES:
			prefix = "Snode";
			break;
		case SPQRTreeNodeType::PARALLEL:
			prefix = "Pnode";
			break;
		case SPQRTreeNodeType::RIGID:
			prefix = "Rnode";
			break;
	}
	
	return prefix + to_string(++g_uiNodeIndex);
}

//============================================================
// Edit the tree
//============================================================
void SPQRTree::JoinSubTree(SPQRTree &subTree)
{
	// Get the root of subtree
	SPQRTreeElement* subRoot = subTree.GetRoot();
	
	// Get the root-node of the subtree
	SPQRTreeNode* subRootNode = subTree.GetRootNode();
	
	if(!subRoot || !subRootNode)
		return;
	
	SPQRTreeElement* sameElement = NULL;
	
	// Find the same element with the subroot using the id
	for(ElementVector::iterator iter = vecElements.begin(); iter != vecElements.end(); iter++)
	{
		if((*iter)->id == subRoot->id)
		{
			sameElement = *iter;
			
			// remove from the list
			vecElements.erase(iter);
			break;
		}
	}
	
	if(!sameElement)
		return;
	
	// Get the parent of same element
	SPQRTreeNode* parent = dynamic_cast<SPQRTreeNode*>(sameElement->parent);
	if(!parent)
		return;
	
	// Check whether the type is rigid
	bool isRigidType = parent->GetType() == SPQRTreeNodeType::RIGID;
	
	// Remove the same element
	for(ElementVector::iterator iter = parent->children.begin(); iter != parent->children.end(); iter++)
	{
		if((*iter)->id == sameElement->id)
		{
			parent->children.erase(iter);
			break;
		}
	}
	
	// Check the type
	if(parent->GetType() == subRootNode->GetType() || subRootNode->children.size() == 1)
	{
		// Copy all the elements of subtree to this tree
		for(ElementVector::iterator elemIter = subTree.vecElements.begin(); elemIter != subTree.vecElements.end();)
		{
			if((*elemIter)->id != subRoot->id && (*elemIter)->id != subRootNode->id)
			{
				// Add to the list of this tree
				vecElements.push_back(*elemIter);
				
				// Remove the element from the list of subtree
				elemIter = subTree.vecElements.erase(elemIter);
			}
			else
			{
				elemIter++;
			}
		}
		
		// Add the children of the root of subtree at the same element's position
		for(ElementVector::iterator childIter = subRootNode->children.begin(); childIter != subRootNode->children.end(); childIter++)
		{
			Couple(parent, *childIter);
			
			// rigid type
			if(isRigidType)
				parent->ReplaceVertexPair(sameElement->id, (*childIter)->id);
		}
		
		// Clear all children
		subRootNode->children.clear();
	}
	else
	{
		// Remove all elements except the root from the element list of subtree
		for(ElementVector::iterator elemIter = subTree.vecElements.begin(); elemIter != subTree.vecElements.end();)
		{
			if((*elemIter)->id != subRoot->id)
			{
				// Add to the element list of this tree
				vecElements.push_back(*elemIter);
				
				// Remove from the element list of subtree
				elemIter = subTree.vecElements.erase(elemIter);
			}
			else
			{
				elemIter++;
			}
		}
		
		// Add the root-node of subtree to the parent as child
		Couple(parent, subRootNode);
		
		// rigid type
		if(isRigidType)
			parent->ReplaceVertexPair(sameElement->id, subRootNode->id);
		
		// Clear all children
		subRoot->children.clear();
	}
	
	// delete same root object
	delete sameElement;
	sameElement = NULL;
}

void SPQRTree::ConvertToBinaryTree()
{
	unsigned int nElements = (unsigned int)vecElements.size();
	unsigned int index = -1;
	
	while(++index < nElements)
	{
		SPQRTreeElement* element = vecElements[index];
		const unsigned int nChildren = (unsigned int)element->children.size();
		
		if(nChildren > 2)
		{
			// Cast as node
			SPQRTreeNode* parentNode = dynamic_cast<SPQRTreeNode*>(element);
			if(!parentNode || parentNode->GetType() == SPQRTreeNodeType::RIGID)
				continue;
			
			// Create new nodes
			SPQRTreeNode* newNodes[nChildren-2];
			for(int i=0; i < nChildren-2; i++)
			{
				newNodes[i] = new SPQRTreeNode(CreateNodeId(parentNode->GetType()), parentNode->GetType());
				vecElements.push_back(newNodes[i]);
				
				// Set parent of the node
				if(i == 0)
				{
					newNodes[i]->parent = parentNode;
				}
				else
				{
					newNodes[i]->parent = newNodes[i-1];
					newNodes[i-1]->children.push_back(newNodes[i]);
				}
			}
			
			// Add a child
			unsigned int nodeIndex = nChildren-3;
			while(parentNode->children.size() > 1)
			{
				newNodes[nodeIndex]->children.push_back(parentNode->children.back());
				parentNode->children.back()->parent = newNodes[nodeIndex];
				
				parentNode->children.pop_back();
				
				if(newNodes[nodeIndex]->children.size() == 2)
					nodeIndex--;
			}
			
			// Add the new node to the children list
			parentNode->children.push_back(newNodes[0]);
		}
	}
}

void SPQRTree::SetRigidNodeAsRoot()
{
	// Find a rigid node
	SPQRTreeNode* rigidNode = NULL;
	for(ElementVector::iterator elemIter = vecElements.begin(); elemIter != vecElements.end(); elemIter++)
	{
		SPQRTreeNode *node = dynamic_cast<SPQRTreeNode*>(*elemIter);
		if(node && node->GetType() == SPQRTreeNodeType::RIGID)
		{
			rigidNode = node;
			break;
		}
	}
	
	if(!rigidNode)
		return;
	
	rigidNode->ReplaceVertexPair(rigidNode->GetUnmatchedPairId(), rigidNode->parent->id);
	
	SetNodeAsRoot(rigidNode);
}

void SPQRTree::SplitNonlinearElements()
{
	// Find the nonlinear elements
	for(ElementVector::iterator elemIter = vecElements.begin(); elemIter != vecElements.end();)
	{
		SPQRTreeLeaf* leaf = dynamic_cast<SPQRTreeLeaf*>(*elemIter);
		if(leaf && leaf->options[OPT_NONLINEAR])
		{
			vecNonlinearElements.push_back(leaf);
			elemIter = vecElements.erase(elemIter);
		}
		else
		{
			elemIter++;
		}
	}
	
	if(vecNonlinearElements.size() == 0)
		return;
	
	for(vector<SPQRTreeLeaf*>::iterator nlIter = vecNonlinearElements.begin(); nlIter != vecNonlinearElements.end(); nlIter++)
	{
		SetLeafAsRoot(*nlIter);
		
		// Decouple the nonlinear element and the root node
		SPQRTreeElement* rootNode =(*nlIter)->children[0];
		Decouple(*nlIter, rootNode);
	}
}

void SPQRTree::SetNodeAsRoot(SPQRTreeNode* node)
{
	SPQRTreeElement* current = node;
	SPQRTreeElement* parent = current->parent;
	SPQRTreeElement* pparent = parent ? parent->parent : NULL;
	
	if(node == GetRoot())
		return;
	
	while(parent)
	{
		// Swap the parent-child relationship
		current->children.push_back(parent);
		parent->parent = current;
		for(ElementVector::iterator childIter = parent->children.begin(); childIter != parent->children.end(); childIter++)
		{
			if(*childIter == current)
			{
				parent->children.erase(childIter);
				break;
			}
		}
		
		// Update the variables
		current = parent;
		parent = pparent;
		pparent = parent ? parent->parent : NULL;
	}
	
	// Clear the parent of the root
	node->parent = NULL;
}

void SPQRTree::SetLeafAsRoot(SPQRTreeLeaf* leaf)
{
	SPQRTreeElement* current = leaf;
	SPQRTreeElement* parent = current->parent;
	SPQRTreeElement* pparent = parent ? parent->parent : NULL;
	
	if(leaf == GetRoot())
		return;
	
	// Decouple the current object and the parent object
	Decouple(parent, current);
	
	while(parent)
	{
		// Decouple the pparent object and the parent object
		if(pparent && parent)
			Decouple(pparent, parent);
		
		// Couple the current object(as parent) and parent object
		Couple(current, parent);
		
		// Update the object pointers
		current = parent;
		parent = pparent;
		pparent = parent ? parent->parent : NULL;
	}
}

void SPQRTree::Couple(SPQRTreeElement* parent, SPQRTreeElement* child, bool atFirst)
{
	for(ElementVector::iterator childIter = parent->children.begin(); childIter != parent->children.end(); childIter++)
		if(*childIter == child)
			return;
	
	if(atFirst)
		parent->children.insert(parent->children.begin(), 1, child);
	else
		parent->children.push_back(child);
	
	child->parent = parent;
}

void SPQRTree::Decouple(SPQRTreeElement* elem1, SPQRTreeElement* elem2)
{
	SPQRTreeElement* parent = NULL, *child = NULL;
	if(elem1->parent == elem2)
	{
		parent = elem2;
		child = elem1;
	}
	else if(elem2->parent == elem1)
	{
		parent = elem1;
		child = elem2;
	}
	else
	{
//		cout << "no parent-child relationship" << endl;
		return;
	}
	
	// Remove the parent relationshiop
	child->parent = NULL;
	
	// Remove the child relationship
	for(ElementVector::iterator childIter = parent->children.begin(); childIter != parent->children.end(); childIter++)
	{
		if(*childIter == child)
		{
			parent->children.erase(childIter);
			return;
		}
	}
}

void SPQRTree::Trim()
{
	for(ElementVector::iterator elemIter = vecElements.begin(); elemIter != vecElements.end(); elemIter++)
	{
		SPQRTreeNode* currentNode = dynamic_cast<SPQRTreeNode*>(*elemIter);
		if(!currentNode)
			continue;
		
		if((*elemIter)->children.size() == 1)
		{
			SPQRTreeElement* child = (*elemIter)->children[0];
			
			SPQRTreeElement* parent = (*elemIter)->parent;
			if(!parent)
			{
				Decouple(*elemIter, child);
			}
			else
			{
				SPQRTreeNode* parentNode = dynamic_cast<SPQRTreeNode*>(parent);
				if(!parentNode)
					continue;
				
				// Set the children of current node to the children of parent
				Decouple(parent, *elemIter);
				Decouple(*elemIter, child);
				Couple(parent, child);
				
				// In case of rigid type
				if(parentNode->GetType() == SPQRTreeNodeType::RIGID)
					parentNode->ReplaceVertexPair((*elemIter)->id, child->id);
			}
		}
		else if((*elemIter)->children.size() == 0)
		{
			SPQRTreeElement* anotherElement = GetChildOnSameLevel(*elemIter);
			if(!anotherElement)
				continue;
			
			SPQRTreeElement* parent = (*elemIter)->parent;
			if(!parent)
				continue;
			
			SPQRTreeElement* pparent = parent ? parent->parent : NULL;
			
			Decouple(parent, *elemIter);
			Decouple(parent, anotherElement);
			if(pparent)
			{
				Decouple(pparent, parent);
				Couple(pparent, anotherElement);
			}
		}
	}
	
	// Remove the disconnected nodes
	for(ElementVector::iterator elemIter = vecElements.begin(); elemIter != vecElements.end();)
	{
		SPQRTreeNode* currentNode = dynamic_cast<SPQRTreeNode*>(*elemIter);
		if(currentNode && !currentNode->parent && currentNode->children.size() == 0)
		{
			elemIter = vecElements.erase(elemIter);
			delete currentNode;
			currentNode = NULL;
		}
		else
		{
			elemIter++;
		}
	}
}

void SPQRTree::ChangeRootTypeRigid()
{
	SPQRTreeNode* rootNode = GetRootNode();
	if(!rootNode || rootNode->GetType() == SPQRTreeNodeType::RIGID)
		return;
	
	// Set the root-node as root
	SetNodeAsRoot(rootNode);
//	Print();
	
	// Change the type
	rootNode->SetType(SPQRTreeNodeType::RIGID);
	
	// Add the pairs
	for(ElementVector::iterator iter = rootNode->children.begin(); iter != rootNode->children.end(); iter++)
		rootNode->AddVertexPair((*iter)->id, (*iter)->vertex_pair);
}

void SPQRTree::RemoveDummyLeaves()
{
	for(ElementVector::iterator elemIter = vecElements.begin(); elemIter != vecElements.end();)
	{
		if((*elemIter)->options[OPT_DUMMY])
		{
			SPQRTreeLeaf* dummyLeaf = dynamic_cast<SPQRTreeLeaf*>(*elemIter);
			if(!dummyLeaf)
			{
				elemIter++;
				continue;
			}
			
			SPQRTreeNode* parent = dynamic_cast<SPQRTreeNode*>((*elemIter)->parent);
			
			if(parent)
			{
				if(parent->GetType() == SPQRTreeNodeType::RIGID)
					parent->RemovePair((*elemIter)->id);
				
				Decouple(parent, dummyLeaf);
			}
			else
			{
				// In cast the dummy element is the root
				SPQRTreeNode* rootNode = dynamic_cast<SPQRTreeNode*>(dummyLeaf->children[0]);
				if(rootNode)
					Decouple(dummyLeaf, rootNode);
			}
			
			// Remove the dummy element
			delete dummyLeaf;
			dummyLeaf = NULL;
			elemIter = vecElements.erase(elemIter);
		}
		else
		{
			elemIter++;
		}
	}
}

void SPQRTree::AddNonlinearElementsToRoot()
{
	SPQRTreeNode* rootNode = GetRootNode();
	if(!rootNode)
		return;
	
	for(unsigned int i=0; i<vecNonlinearElements.size(); i++)
		Couple(rootNode, vecNonlinearElements[i], true);
	
	rootNode->SetNonlinearElementCount((unsigned int)(vecNonlinearElements.size()));
	rootNode->SortNonlinearElements();
}

void SPQRTree::MergeChildrenInRoot()
{
	// Get the root-node
	SPQRTreeNode* root = GetRootNode();
	unsigned int nChildren;
	if(!root || (nChildren = (unsigned int)(root->children.size() - root->GetNonlinearElementCount())) <= 2)
		return;
	
	// Check whether there is duplicated vertex pair in children
	vector<ElementVector> underElements;
	vector<VertexPair> underVertexPairs;
	ElementVector::iterator childIter1 = root->children.begin();
	while(childIter1 != root->children.end())
	{
		// Do not merge in case of nonlinear elements
		if((*childIter1)->options[OPT_NONLINEAR])
		{
			childIter1++;
			continue;
		}
		
		ElementVector::iterator childIter2 = childIter1+1;
		ElementVector tempElements;
		while(childIter2 != root->children.end())
		{
			// Do not merge in case of nonlinear elements
			if((*childIter2)->options[OPT_NONLINEAR])
			{
				childIter2++;
				continue;
			}
			
			if(((*childIter1)->vertex_pair.first == (*childIter2)->vertex_pair.first && (*childIter1)->vertex_pair.second == (*childIter2)->vertex_pair.second) ||
			   ((*childIter1)->vertex_pair.first == (*childIter2)->vertex_pair.second && (*childIter1)->vertex_pair.second == (*childIter2)->vertex_pair.first))
			{
				// Store in the array
				tempElements.push_back(*childIter2);
				underVertexPairs.push_back(root->GetVertexPair((*childIter2)->id));
				
				// Decouple them
				(*childIter2)->parent = NULL;
				root->RemovePair((*childIter2)->id);
				childIter2 = root->children.erase(childIter2);
			}
			else
			{
				childIter2++;
			}
		}
		if(tempElements.size() > 0)
		{
			tempElements.push_back(*childIter1);
			
			(*childIter1)->parent = NULL;
			root->RemovePair((*childIter1)->id);
			childIter1 = root->children.erase(childIter1);
			
			underElements.push_back(tempElements);
			
		}
		else
		{
			childIter1++;
		}
	}
	
	// Couple the chilren, which owns same vertex pair, each other as parallel
	unsigned int vertexPairIndex=0;
	for(vector<ElementVector>::iterator iter = underElements.begin(); iter != underElements.end(); iter++)
	{
		// Can create one node because there is minimum two elements
		SPQRTreeNode* node = new SPQRTreeNode(CreateNodeId(SPQRTreeNodeType::PARALLEL), SPQRTreeNodeType::PARALLEL);
		vecElements.push_back(node);
		Couple(node, (*iter)[0]);
		Couple(node, (*iter)[1]);
		
		// a node to be added in the children list of root
		SPQRTreeNode* parent = node;
		
		// Create one node for each of the remaining children to create a tree structure.
		for(unsigned int i=2; i<(*iter).size(); i++)
		{
			parent = new SPQRTreeNode(CreateNodeId(SPQRTreeNodeType::PARALLEL), SPQRTreeNodeType::PARALLEL);
			vecElements.push_back(parent);
			Couple(parent, node);
			Couple(parent, (*iter)[i]);
		}
		
		// Add to the root
		Couple(root, parent);
		
		// Modify the rigid-table
		root->AddVertexPair(parent->id, underVertexPairs[vertexPairIndex++]);
	}
}

void SPQRTree::SetResistanceOfVoltageSource()
{
	vector<SPQRTreeElement*> elementsToDelete;
	
	for(vector<SPQRTreeElement*>::iterator elemIter = vecElements.begin(); elemIter != vecElements.end(); elemIter++)
	{
		string prefix = (*elemIter)->GetPrefix();
		
		// In case that the element is the voltage source
		if(prefix == "AC" || prefix == "DC")
		{
			SPQRTreeElement* vs = *elemIter;
			SPQRTreeElement* anotherChild = GetChildOnSameLevel(vs);
			SPQRTreeLeaf* resistor = dynamic_cast<SPQRTreeLeaf*>(anotherChild);
			
			// In case that the another child is the resistor
			if(resistor && resistor->GetPrefix() == "R" && resistor->vertex_pair.second != "grounding")
			{
				// Set the resistance of voltage source
				vs->values[VAL_RESISTANCE] = resistor->values[VAL_RESISTANCE];
				
				// Merge the vertex pair
				vs->MergeVertexPair(resistor->vertex_pair);
				
				// Couple the VS to the pparent
				SPQRTreeElement* parent = vs->parent;
				SPQRTreeElement* pparent = parent->parent;
				if(pparent)
				{
					// In case of rigid, update the table
					SPQRTreeNode* rigid = dynamic_cast<SPQRTreeNode*>(pparent);
					if(rigid && rigid->GetType() == SPQRTreeNodeType::RIGID)
						rigid->ReplaceVertexPair(parent->id, vs->id);
					
					Decouple(pparent, parent);
					Couple(pparent, vs);
				}
				
				// Add to the list for deleting
				elementsToDelete.push_back(parent);
				elementsToDelete.push_back(anotherChild);
			}
		}
	}
	
	// Delete the elements in the list
	for(vector<SPQRTreeElement*>::iterator elemDelIter = elementsToDelete.begin(); elemDelIter != elementsToDelete.end();)
	{
		bool removed = false;
		for(vector<SPQRTreeElement*>::iterator elemIter = vecElements.begin(); elemIter != vecElements.end(); elemIter++)
		{
			if(*elemDelIter == *elemIter)
			{
				delete *elemIter;
				vecElements.erase(elemIter);
				elemDelIter = elementsToDelete.erase(elemDelIter);
				removed = true;
				break;
			}
		}
		
		if(!removed)
			elemDelIter++;
	}
}

//============================================================
// etc.
//============================================================
void SPQRTree::SetPoles()
{
	SPQRTreeElement* root = GetRoot();
	vector<VertexPair> pairs;
	root->SetVertexPair(&pairs);
}

void SPQRTree::SetVertexPair()
{
	vector<VertexPair> pairs;
	GetRoot()->SetVertexPair(&pairs);
}

//============================================================
// WDF Tree
//============================================================
void SPQRTree::SetInputVoltage(float fInputVoltage)
{
	this->fInputVoltage = fInputVoltage;
}

void SPQRTree::SetInputFrequency(float fInputFrequency)
{
	this->fInputFrequency = fInputFrequency;
}

WDFTree* SPQRTree::CreateWDFTree(float fSamplingTime)
{
	WDFTree* wdfTree = new WDFTree(fSamplingTime, fInputVoltage, fInputFrequency);
	GetRoot()->CreateWDFObject(wdfTree);
	
	return wdfTree;
}
