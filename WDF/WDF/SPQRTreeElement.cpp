//
//  SPQRTreeElement.cpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#include "SPQRTreeElement.hpp"
#include "WDFDiode.hpp"
#include "WDFTriode.hpp"
#include "WDFTransistor.hpp"

typedef vector<SPQRTreeElement*>	ElementVector;

//============================================================
// SPQR Tree Element
//============================================================
SPQRTreeElement::SPQRTreeElement()
{
	id = "";
	parent = NULL;
	vertex_pair.first = "";
	vertex_pair.second = "";
	
	for(int i=0; i<NUM_OF_PROPS; i++)
		values[i] = 0.0;
	
	for(int i=0; i<NUM_OF_OPTS; i++)
		options[i] = false;
}

SPQRTreeElement::SPQRTreeElement(string id)
{
	this->id = id;
	parent = NULL;
	vertex_pair.first = "";
	vertex_pair.second = "";
	
	for(int i=0; i<NUM_OF_PROPS; i++)
		values[i] = 0.0;
	
	for(int i=0; i<NUM_OF_OPTS; i++)
		options[i] = false;
}

SPQRTreeElement::~SPQRTreeElement()
{
	
}

string SPQRTreeElement::GetPrefix()
{
	string prefix = "";
	int charIndex = 0;
	
	// Get the type of WDF leaf from the prefix of id
	while(charIndex < id.length())
	{
		char c = id.at(charIndex++);
		if(c >= 'A' && c <= 'Z')
			prefix += c;
		else
			break;
	}
	
	return prefix;
}

void SPQRTreeElement::MergeVertexPair(VertexPair pair)
{
	if(vertex_pair.first == "")
		vertex_pair = pair;
	else if(pair.first == "" || vertex_pair == pair)
		return;
	else if(vertex_pair.first == pair.second && vertex_pair.second == pair.first)
		return;
	else
	{
		if(vertex_pair.first == pair.first)
			vertex_pair.first = pair.second;
		else if(vertex_pair.second == pair.first)
			vertex_pair.second = pair.second;
		else if(vertex_pair.first == pair.second)
			vertex_pair.first = pair.first;
		else if(vertex_pair.second == pair.second)
			vertex_pair.second = pair.first;
	}
//	cout << "merged vertex: (" << vertex_pair.first << "," << vertex_pair.second << ")" << endl;
}

//============================================================
// SPQR Tree Node
//============================================================
SPQRTreeNode::SPQRTreeNode() : SPQRTreeElement()
{
	type = SPQRTreeNodeType::SERIES;
	nNonlinearCount = 0;
}

SPQRTreeNode::SPQRTreeNode(string id, const SPQRTreeNodeType type) : SPQRTreeElement(id)
{
	this->type = type;
	nNonlinearCount = 0;
}

SPQRTreeNode::~SPQRTreeNode()
{
	
}

void SPQRTreeNode::SetType(const SPQRTreeNodeType type)
{
	this->type = type;
}

SPQRTreeNodeType SPQRTreeNode::GetType()
{
	return type;
}

void SPQRTreeNode::SetNonlinearElementCount(unsigned int nNonlinear)
{
	nNonlinearCount = nNonlinear;
}

unsigned int SPQRTreeNode::GetNonlinearElementCount()
{
	return nNonlinearCount;
}

void SPQRTreeNode::SortNonlinearElements(bool ascending)
{
	sort(children.begin(), children.begin() + nNonlinearCount, Compare);
}

bool SPQRTreeNode::Compare(SPQRTreeElement* elem1, SPQRTreeElement* elem2)
{
	if(elem1->id.length() < elem2->id.length())
		return true;
	else if(elem1->id.length() > elem2->id.length())
		return false;
	else
		return elem1->id.compare(elem2->id) < 0;
}

void SPQRTreeNode::SortChildren(bool ascending)
{
	QuickSort(children, 0, (unsigned int)children.size()-1, ascending);
}

string SPQRTreeNode::GetUnmatchedPairId()
{
	for(map<string, VertexPair>::iterator pairIter = rigidVertexTable.begin(); pairIter != rigidVertexTable.end(); pairIter++)
	{
		bool found = false;
		for(ElementVector::iterator childIter = children.begin(); childIter != children.end(); childIter++)
		{
			if(pairIter->first == (*childIter)->id)
			{
				found = true;
				break;
			}
		}
		
		if(!found)
			return pairIter->first;
	}
	
	return "";
}

VertexPair SPQRTreeNode::GetVertexPair(const string edgeId)
{
	return rigidVertexTable[edgeId];
}

void SPQRTreeNode::AddVertexPair(const string edgeId, const VertexPair &pair)
{
	rigidVertexTable[edgeId] = pair;
	
//	cout << "added a vertex pair: " << edgeId << "(" << pair.first << "," << pair.second << ")" << endl;
//	for(map<string, VertexPair>::iterator iter = rigidVertexTable.begin(); iter != rigidVertexTable.end(); iter++)
//		cout << iter->first << "(" << iter->second.first << "," << iter->second.second << ") ";
//	cout << endl;
}

void SPQRTreeNode::ReplaceVertexPair(const string existingId, const string newId)
{
	// Add a new one
	rigidVertexTable[newId] = rigidVertexTable[existingId];
	
	// Remove an old one
	for(map<string, VertexPair>::iterator iter = rigidVertexTable.begin(); iter != rigidVertexTable.end(); iter++)
	{
		if(iter->first == existingId)
		{
			rigidVertexTable.erase(iter);
			break;
		}
	}
	
//	cout << "replaced a vertex pair: " << existingId << "->" << newId << endl;
//	for(map<string, VertexPair>::iterator iter = rigidVertexTable.begin(); iter != rigidVertexTable.end(); iter++)
//		cout << iter->first << "(" << iter->second.first << "," << iter->second.second << ") ";
//	cout << endl;
}

void SPQRTreeNode::RemovePair(const string edgeId)
{
	for(map<string, VertexPair>::iterator iter = rigidVertexTable.begin(); iter != rigidVertexTable.end(); iter++)
	{
		if(iter->first == edgeId)
		{
			rigidVertexTable.erase(iter);
			break;
		}
	}
	
//	cout << "removed a vertex pair: " << edgeId << endl;
//	for(map<string, VertexPair>::iterator iter = rigidVertexTable.begin(); iter != rigidVertexTable.end(); iter++)
//		cout << iter->first << "(" << iter->second.first << "," << iter->second.second << ") ";
//	cout << endl;
}

void SPQRTreeNode::CreateWDFObject(WDFTree* wdfTree)
{
	for(ElementVector::iterator iter = children.begin(); iter != children.end(); iter++)
		(*iter)->CreateWDFObject(wdfTree);
	
	switch(type)
	{
		case SPQRTreeNodeType::SERIES:
			wdfTree->AddObject(new WDFSeries(wdfTree->FindObject(children[0]->id), wdfTree->FindObject(children[1]->id), id));
			break;
		case SPQRTreeNodeType::PARALLEL:
			wdfTree->AddObject(new WDFParallel(wdfTree->FindObject(children[0]->id), wdfTree->FindObject(children[1]->id), id));
			break;
		case SPQRTreeNodeType::RIGID:
		{
//			cout << "========================================" << endl;
//			cout << "Create a rigid adaptor" << endl;
//			cout << "========================================" << endl;
			
			// Get the number of children
			unsigned int nChildrenPorts = 0;
			for(ElementVector::iterator childIter = children.begin(); childIter != children.end(); childIter++)
				nChildrenPorts += 1;
//			cout << "the cout of the ports of children: " << nChildrenPorts << endl;
			
			// Create the index table
			unsigned int tableIndex = nChildrenPorts;
			bool groundingFound = false;
			for(map<string, VertexPair>::iterator iter = rigidVertexTable.begin(); iter != rigidVertexTable.end(); iter++)
			{
				VertexPair pair = iter->second;
				
				// First vertex in the pair
				if(rigidIndexTable.find(pair.first) == rigidIndexTable.end())
				{
					if(pair.first == "grounding")
					{
						rigidIndexTable[pair.first] = -1;
						groundingFound = true;
					}
					else
					{
						rigidIndexTable[pair.first] = tableIndex++;
					}
				}
				
				// Second vertex in the pair
				if(rigidIndexTable.find(pair.second) == rigidIndexTable.end())
				{
					if(pair.second == "grounding")
					{
						rigidIndexTable[pair.second] = -1;
						groundingFound = true;
					}
					else
					{
						rigidIndexTable[pair.second] = tableIndex++;
					}
				}
			}
			
			// Set the maximum index of table to -1 if there is no grounding vertex
			if(!groundingFound)
			{
				for(map<string, int>::iterator iter = rigidIndexTable.begin(); iter != rigidIndexTable.end(); iter++)
				{
					if((*iter).second == (tableIndex-1))
					{
						(*iter).second = -1;
						tableIndex--;
					}
				}
			}
			
//			cout << "rigidVertexTable: ";
//			for(map<string, VertexPair>::iterator iter = rigidVertexTable.begin(); iter != rigidVertexTable.end(); iter++)
//				cout << (*iter).first << "(" << (*iter).second.first << "," << (*iter).second.second << ") ";
//			cout << endl;
			
//			cout << "rigidIndexTable: ";
//			for(map<string, int>::iterator iter = rigidIndexTable.begin(); iter != rigidIndexTable.end(); iter++)
//				cout << "(" << (*iter).first << ":" << (*iter).second << ") ";
//			cout << endl;
			
//			cout << "the count of nonlinear elements: " << nNonlinearCount << endl;
			if(nNonlinearCount == 0)
			{
				//============================================================
				// There is no nonlinear element
				//============================================================
				// Create the WDF rigid adaptor
				WDFRTypeAdaptor* rigidAdaptor = new WDFRTypeAdaptor(nChildrenPorts, (int)children.size(), tableIndex, "root");
				
				// Connect the children
//				cout << "connect children: ";
				for(ElementVector::iterator iter = children.begin(); iter != children.end(); iter++)
				{
					rigidAdaptor->Connect(rigidIndexTable[rigidVertexTable[(*iter)->id].first], rigidIndexTable[rigidVertexTable[(*iter)->id].second], wdfTree->FindObject((*iter)->id));
//					cout << (*iter)->id << "(" << rigidIndexTable[rigidVertexTable[(*iter)->id].first] << "," << rigidIndexTable[rigidVertexTable[(*iter)->id].second] << ") ";
				}
//				cout << endl;
				
				// Set ready
				rigidAdaptor->UpdateScatteringMatrix();
				
				// Add to the list
				wdfTree->AddObject(rigidAdaptor);
				
				// Set as root
				wdfTree->SetRoot(rigidAdaptor);
			}
			else
			{
				//============================================================
				// There are nonlinear elements
				//============================================================
				// Create the adaptor
				WDFRTypeAdaptorNL* rigidAdaptor = NULL;
				if(nNonlinearCount >= 1)
					rigidAdaptor = new WDFRTypeAdaptorNL(nNonlinearCount, nChildrenPorts - nNonlinearCount, tableIndex - nChildrenPorts);
				
				// Connect the children
//				cout << "connect children: ";
				for(unsigned int iChild=0; iChild < nNonlinearCount; iChild++)
				{
					Vertex v1 = children[iChild]->vertex_pair.first;
					Vertex v2 = children[iChild]->vertex_pair.second;
					rigidAdaptor->ConnectNL(rigidIndexTable[children[iChild]->vertex_pair.first], rigidIndexTable[children[iChild]->vertex_pair.second], wdfTree->FindObject(children[iChild]->id)->GetDecoupledPort());
//					cout << children[iChild]->id << "(" << rigidIndexTable[children[iChild]->vertex_pair.first] << "," << rigidIndexTable[children[iChild]->vertex_pair.second] << ") ";
				}
				for(unsigned int iChild = nNonlinearCount; iChild < children.size(); iChild++)
				{
					// Get the pair of index from the table
					pair<int, int> pair(rigidIndexTable[rigidVertexTable[children[iChild]->id].first], rigidIndexTable[rigidVertexTable[children[iChild]->id].second]);
					
					// Set the direction
					if(pair.first == -1)
					{
						int temp = pair.first;
						pair.first = pair.second;
						pair.second = temp;
					}
					
					rigidAdaptor->Connect(pair.first, pair.second, wdfTree->FindObject(children[iChild]->id));
//					cout << children[iChild]->id << "(" << pair.first << "," << pair.second << ") ";
				}
//				cout << endl;
				
				// Set ready
				rigidAdaptor->UpdateMatrices();
				
				// Add to the list
				wdfTree->AddObject(rigidAdaptor);
				
				// Set as root
				wdfTree->SetRoot(rigidAdaptor);
			}
			
			break;
		}
	}
}

void SPQRTreeNode::SetVertexPair(vector<VertexPair> * remainedVertexPairs)
{
	for(ElementVector::iterator childIter = children.begin(); childIter != children.end(); childIter++)
		(*childIter)->SetVertexPair(remainedVertexPairs);
	
	if(children.size() == 2)
	{
		if(type == SPQRTreeNodeType::PARALLEL)
		{
			// The pair of vertex is same of each children in case of parallel
			vertex_pair = children[0]->vertex_pair;
		}
		else if(type == SPQRTreeNodeType::SERIES)
		{
			if(children[0]->vertex_pair.first == "" || children[0]->vertex_pair.second == "" || children[1]->vertex_pair.first == "" || children[1]->vertex_pair.second == "")
			{
				if(children[0]->vertex_pair.first != "")
				{
					remainedVertexPairs->push_back(children[0]->vertex_pair);
				}
				
				if(children[1]->vertex_pair.first != "")
				{
					remainedVertexPairs->push_back(children[1]->vertex_pair);
				}
				
				if(remainedVertexPairs->size() > 0)
				{
					unsigned int i = 0;
					while(i < remainedVertexPairs->size())
					{
						bool currentValueChanged = false;
						
						for(vector<VertexPair>::iterator iter = remainedVertexPairs->begin()+i+1; iter != remainedVertexPairs->end();)
						{
							VertexPair vp1 = (*remainedVertexPairs)[i];
							VertexPair vp2 = (*iter);
							
							VertexPair temp_vp;
							if(vp1.first == vp2.first)
							{
								temp_vp.first = vp1.second;
								temp_vp.second = vp2.second;
								
								currentValueChanged = true;
							}
							else if(vp1.first == vp2.second)
							{
								temp_vp.first = vp2.first;
								temp_vp.second = vp1.second;
								
								currentValueChanged = true;
							}
							else if(vp1.second == vp2.first)
							{
								temp_vp.first = vp1.first;
								temp_vp.second = vp2.second;
								
								currentValueChanged = true;
							}
							else if(vp1.second == vp2.second)
							{
								temp_vp.first = vp1.first;
								temp_vp.second = vp2.first;
								
								currentValueChanged = true;
							}
							else
							{
								iter++;
								continue;
							}
							
							if(temp_vp.first == "grounding")
							{
								Vertex temp = temp_vp.first;
								temp_vp.first = temp_vp.second;
								temp_vp.second = temp;
							}
							
							iter = remainedVertexPairs->erase(iter);
							(*remainedVertexPairs)[i] = temp_vp;
						}
						
						if(!currentValueChanged)
							i++;
					}
					
					if(remainedVertexPairs->size() == 1)
						vertex_pair = (*remainedVertexPairs)[0];
				}
			}
			else
			{
				vertex_pair = children[0]->vertex_pair;
				
				// Find a common vertex: Set the other two vertices as pair
				if(vertex_pair.first == children[1]->vertex_pair.first)
				{
					vertex_pair.first = children[0]->vertex_pair.second;
					vertex_pair.second = children[1]->vertex_pair.second;
				}
				else if(vertex_pair.first == children[1]->vertex_pair.second)
				{
					vertex_pair.first = children[0]->vertex_pair.second;
					vertex_pair.second = children[1]->vertex_pair.first;
				}
				else if(vertex_pair.second == children[1]->vertex_pair.first)
				{
					vertex_pair.first = children[0]->vertex_pair.first;
					vertex_pair.second = children[1]->vertex_pair.second;
				}
				else if(vertex_pair.second == children[1]->vertex_pair.second)
				{
					vertex_pair.first = children[0]->vertex_pair.first;
					vertex_pair.second = children[1]->vertex_pair.first;
				}
				else
				{
//					cout << "CANNOT MERGE VERTEX PAIRS: ";
//					cout << children[0]->id << "(" << children[0]->vertex_pair.first << "," << children[0]->vertex_pair.second << ") ";
//					cout << children[1]->id << "(" << children[1]->vertex_pair.first << "," << children[1]->vertex_pair.second << ") " << endl;
					
					vertex_pair = VertexPair("", "");
					remainedVertexPairs->push_back(children[0]->vertex_pair);
					remainedVertexPairs->push_back(children[1]->vertex_pair);
					
//					cout << "REMAINED VERTEX PAIRS: ";
//					for(vector<VertexPair>::iterator elemIter = remainedVertexPairs->begin(); elemIter != remainedVertexPairs->end(); elemIter++)
//						cout << "(" << (*elemIter).first << "," << (*elemIter).second << ") ";
//					cout << endl;
					
					return;
				}
			}
		}
		
		// Set the "grounding" as negative pole
		if(vertex_pair.first == "grounding")
		{
			string temp = vertex_pair.first;
			vertex_pair.first = vertex_pair.second;
			vertex_pair.second = temp;
		}
	}
}

void SPQRTreeNode::QuickSort(ElementVector array, unsigned int left, unsigned int right, bool ascending)
{
	unsigned int i=left, j=right;
	SPQRTreeElement* temp;
	SPQRTreeElement* pivot = children[(left + right)/2];
	
	while(i <= j)
	{
		if(ascending)
		{
			while(children[i]->id.compare(pivot->id) < 0)
				i++;
			
			while(children[j]->id.compare(pivot->id) > 0)
				j--;
		}
		else
		{
			while(children[i]->id.compare(pivot->id) > 0)
				i++;
			
			while(children[j]->id.compare(pivot->id) < 0)
				j--;
		}
		
		if(i <= j)
		{
			temp = children[i];
			children[i] = children[j];
			children[j] = temp;
			
			i++;
			j--;
		}
	}
	
	if(left < j)
		QuickSort(children, left, j, ascending);
	
	if(i < right)
		QuickSort(children, i, right, ascending);
}

SPQRTreeLeaf::SPQRTreeLeaf() : SPQRTreeElement()
{
	
}

SPQRTreeLeaf::SPQRTreeLeaf(const Edge* const edge) : SPQRTreeElement(edge->id)
{
	vertex_pair = edge->vertex_pair;
	
	for(int i=0; i<NUM_OF_PROPS; i++)
		values[i] = edge->values[i];
	
	for(int i=0; i<NUM_OF_OPTS; i++)
		options[i] = edge->options[i];
}

SPQRTreeLeaf::~SPQRTreeLeaf()
{
	
}

void SPQRTreeLeaf::CreateWDFObject(WDFTree* wdfTree)
{
	string prefix = GetPrefix();
	
	if(prefix == "R")			// resistor
	{
		WDFResistor* resistor = new WDFResistor(values[VAL_RESISTANCE], id);
		wdfTree->AddObject(resistor);
		if(options[OPT_OUTPUT])
			wdfTree->SetOutput(resistor);
	}
	else if(prefix == "C")		// capacitor
	{
		WDFCapacitor* capacitor = new WDFCapacitor(values[VAL_CAPACITANCE], wdfTree->GetSamplingTime(), id);
		wdfTree->AddObject(capacitor);
		if(options[OPT_OUTPUT])
			wdfTree->SetOutput(capacitor);
	}
	else if(prefix == "AC" || prefix == "DC")	// voltage source
	{
		double R = values[VAL_RESISTANCE] == 0.0 ? 1.0 : values[VAL_RESISTANCE];
		WDFVoltageSource* vs = new WDFVoltageSource(values[VAL_VOLTAGE], R, id);
		wdfTree->AddObject(vs);
		if(options[OPT_INPUT])
			wdfTree->SetInput(vs);
	}
	else if(prefix == "I")		// inductor
	{
		WDFInductor* inductor = new WDFInductor(values[VAL_INDUCTANCE], wdfTree->GetSamplingTime(), id);
		wdfTree->AddObject(inductor);
		if(options[OPT_OUTPUT])
			wdfTree->SetOutput(inductor);
	}
	else if(prefix == "D")		// diode
	{
		WDFRTypeDiode* diode = new WDFRTypeDiode(2.52e-9, 26e-3, 1.732, id);
		wdfTree->AddObject(diode);
	}
	else if(prefix == "VT")		// vacuum tube
	{
		WDFRTypeTriode* tube = new WDFRTypeTriode((DempwolfTriodeModel)values[VAL_VACUUM_TUBE_MODEL], id);
		wdfTree->AddObject(tube);
	}
	else if(prefix == "TR")		// transistor
	{
		WDFRTypeTransistor* bjt = new WDFRTypeTransistor((TransistorModel)values[VAL_TRANSISTOR_MODEL], id);
		wdfTree->AddObject(bjt);
	}
	else
	{
//		cout << "WRONG PREFIX: " << prefix << endl;
	}
}

void SPQRTreeLeaf::SetVertexPair(vector<VertexPair> * remainedVertexPairs)
{
	for(ElementVector::iterator childIter = children.begin(); childIter != children.end(); childIter++)
		(*childIter)->SetVertexPair(remainedVertexPairs);
}
