//
//  Graph.cpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#include "Graph.hpp"
#include <queue>
#include <stack>

/**
 The index of virtual edge
 */
unsigned int g_uiVirtualEdgeIndex = 0;

Graph::Graph()
{
	
}

Graph::Graph(Graph* const graph)
{
	for(Map::iterator mapIter = graph->mMap.begin(); mapIter != graph->mMap.end(); mapIter++)
	{
		Vertex vertex1 = (*mapIter).first;
		ElementSet *elements = &((*mapIter).second);
		
		for(ElementSet::iterator elemIter = elements->begin(); elemIter != elements->end(); elemIter++)
		{
			Vertex vertex2 = (*elemIter).second;
			Edge *edge = (*elemIter).first;
			
			AddEdge(edge->vertex_pair.first, edge->vertex_pair.second, edge);
		}
	}
}

Graph::~Graph()
{
	for(EdgeSet::iterator edgeIter = mEdges.begin(); edgeIter != mEdges.end(); edgeIter++)
		delete (*edgeIter);
}

//============================================================
// Manage the edges and vertices
//============================================================
void Graph::AddEdge(const Vertex &vertex1, const Vertex &vertex2, const Edge &edge)
{
	Edge *newEdge = new Edge(edge);
	newEdge->vertex_pair = VertexPair(vertex1, vertex2);
	
	mMap[vertex1].insert(Element(newEdge, vertex2));
	mMap[vertex2].insert(Element(newEdge, vertex1));
	
	mEdges.insert(newEdge);
}

void Graph::AddEdge(const Vertex &vertex1, const Vertex &vertex2, Edge* edge)
{
	for(EdgeSet::iterator edgeIter = mEdges.begin(); edgeIter != mEdges.end(); edgeIter++)
		if((*edgeIter)->id == edge->id)
			return;
	
	Edge *newEdge = new Edge(*edge);
	newEdge->vertex_pair = VertexPair(vertex1, vertex2);
	
	mMap[vertex1].insert(Element(newEdge, vertex2));
	mMap[vertex2].insert(Element(newEdge, vertex1));
	
	mEdges.insert(newEdge);
}

void Graph::RemoveEdge(const Edge* const edge)
{
	Map::iterator mapIter = mMap.begin();
	while(mapIter != mMap.end())
	{
		ElementSet::iterator elemIter = (*mapIter).second.begin();
		while(elemIter != (*mapIter).second.end())
		{
			if((*elemIter).first->id == edge->id)
				elemIter = (*mapIter).second.erase(elemIter);
			else
				elemIter++;
		}
		if((*mapIter).second.size() == 0)
			mapIter = mMap.erase(mapIter);
		else
			mapIter++;
	}
}

void Graph::RemoveVertex(const Vertex &vertex)
{
	mMap.erase(vertex);
	
	Map::iterator mapIter = mMap.begin();
	while(mapIter != mMap.end())
	{
		ElementSet::iterator elemIter = (*mapIter).second.begin();
		while(elemIter != (*mapIter).second.end())
		{
			if((*elemIter).second == vertex)
				elemIter = (*mapIter).second.erase(elemIter);
			else
				elemIter++;
		}
		mapIter++;
	}
}

void Graph::GetVertices(VertexSet* vertices)
{
	for(Map::iterator mapIter = mMap.begin(); mapIter != mMap.end(); mapIter++)
		vertices->insert((*mapIter).first);
}

void Graph::GetEdges(EdgeSet* edges)
{
	for(Map::iterator mapIter = mMap.begin(); mapIter != mMap.end(); mapIter++)
		for(ElementSet::iterator elemIter = (*mapIter).second.begin(); elemIter != (*mapIter).second.end(); elemIter++)
			edges->insert((*elemIter).first);
}

//============================================================
// Check path
//============================================================
bool Graph::HasPath(const Vertex &startVertex, const Vertex &endVertex)
{
	map<Vertex, bool> visited;
	queue<Vertex> vqueue;
	
	vqueue.push(startVertex);
	visited[startVertex] = true;
	
	while(vqueue.size() > 0)
	{
		Vertex temp = vqueue.front();
		vqueue.pop();
		
		for(Map::iterator mapIter = mMap.begin(); mapIter != mMap.end(); mapIter++)
		{
			if(IsAdjacent(temp, (*mapIter).first) && !visited[(*mapIter).first])
			{
				if((*mapIter).first == endVertex)
					return true;
				
				visited[(*mapIter).first] = true;
				vqueue.push((*mapIter).first);
			}
		}
	}
	
	return false;
}

//============================================================
// Properties of graph
//============================================================
bool Graph::IsAdjacent(const Vertex &vertex1, const Vertex &vertex2)
{
	for(ElementSet::iterator elemIter = mMap[vertex1].begin(); elemIter != mMap[vertex1].end(); elemIter++)
		if((*elemIter).second == vertex2)
			return true;
	
	return false;
}

bool Graph::IsBiconnected()
{
	if(mMap.size() <= 2)
		return false;
	
	for(Map::iterator mapIter = mMap.begin(); mapIter != mMap.end(); mapIter++)
	{
		Graph copiedGraph(this);
		copiedGraph.RemoveVertex((*mapIter).first);
		
		for(Map::iterator startMapIter = copiedGraph.mMap.begin(); startMapIter != copiedGraph.mMap.end(); startMapIter++)
		{
			for(Map::iterator depMapIter = copiedGraph.mMap.begin(); depMapIter != copiedGraph.mMap.end(); depMapIter++)
			{
				if((*startMapIter).first == (*depMapIter).first)
					continue;
				
				if(!copiedGraph.HasPath((*startMapIter).first, (*depMapIter).first))
					return false;
			}
		}
	}
	
	return true;
}

void Graph::FindReferenceEdgeWithVertexPair(Edge* referenceEdge, VertexPair* referenceVertexPair)
{
	unsigned int prevCutsCount = 0;
	EdgeSet visited;
	
	// Return any edge if the graph has only 2 vertices
	if(mMap.size() == 2)
	{
		Vertex vertex1 = (*mMap.begin()).first;
		Vertex vertex2 = (*(*mMap.begin()).second.begin()).second;
		
		referenceEdge = (*(*mMap.begin()).second.begin()).first;
		referenceVertexPair->first = vertex1;
		referenceVertexPair->second = vertex2;
	}
	
	// In case of having 3 or more vertices
	for(Map::iterator mapIter = mMap.begin(); mapIter != mMap.end(); mapIter++)
	{
		if((*mapIter).second.size() == 2)
		{
			for(ElementSet::iterator elemIter = (*mapIter).second.begin(); elemIter != (*mapIter).second.end(); elemIter++)
			{
				Edge* tempRefEdge = (*elemIter).first;
				Vertex adjVertex = (*elemIter).second;
				
				// The nonlinear edge cannot be the reference edge.
				if(tempRefEdge->options[OPT_NONLINEAR])
					continue;
				
				// Go to the next element if alreday visited
				if(visited.find(tempRefEdge) != visited.end())
					continue;
				
				// Set the element visited
				visited.insert(tempRefEdge);
				
				Graph tempGraph(this);
				
				// Get the cut-vertex list
				tempGraph.RemoveEdge(tempRefEdge);
				VertexSet tempCuts;
				tempGraph.GetCutset(&tempCuts);
				
				// If new reference edge make more cut-vertices, set the new one to the reference edge
				if(tempCuts.size() > prevCutsCount)
				{
					prevCutsCount = (unsigned int)tempCuts.size();
					*referenceEdge = *((*elemIter).first);
					referenceVertexPair->first = (*mapIter).first;
					referenceVertexPair->second = (*elemIter).second;
				}
			}
		}
	}
}

unsigned int Graph::GetSeparationPairs(vector<VertexPair> * const separationPair)
{
	for(Map::iterator mapIter1 = mMap.begin(); mapIter1 != mMap.end(); mapIter1++)
	{
		for(Map::iterator mapIter2 = mMap.begin(); mapIter2 != mMap.end(); mapIter2++)
		{
			if(*mapIter1 >= *mapIter2)
				continue;
			
			// Get the vertices
			Vertex vertex1 = (*mapIter1).first;
			Vertex vertex2 = (*mapIter2).first;
			
			// Remove vertex pair
			Graph tempGraph(this);
			tempGraph.RemoveVertex(vertex1);
			tempGraph.RemoveVertex(vertex2);
			
			// Check the paths
			bool noPath = false;
			for(Map::iterator startMapIter = tempGraph.mMap.begin(); startMapIter != tempGraph.mMap.end(); startMapIter++)
			{
				for(Map::iterator depMapIter = tempGraph.mMap.begin(); depMapIter != tempGraph.mMap.end(); depMapIter++)
				{
					if((*startMapIter).first == (*depMapIter).first)
						continue;
					
					if(!tempGraph.HasPath((*startMapIter).first, (*depMapIter).first))
					{
						noPath = true;
						break;
					}
				}
				
				if(noPath)
					break;
			}
			
			if(noPath)
				separationPair->push_back(VertexPair(vertex1, vertex2));
		}
	}
	
	return (unsigned int)separationPair->size();
}

unsigned int Graph::GetSplitPairs(vector<VertexPair> * const splitPair)
{
	for(Map::iterator mapIter1 = mMap.begin(); mapIter1 != mMap.end(); mapIter1++)
	{
		for(Map::iterator mapIter2 = mMap.begin(); mapIter2 != mMap.end(); mapIter2++)
		{
			if(*mapIter1 >= *mapIter2)
				continue;
			
			// Get the vertices
			Vertex vertex1 = (*mapIter1).first;
			Vertex vertex2 = (*mapIter2).first;
			
			// Remove the vertex pair
			Graph tempGraph(this);
			tempGraph.RemoveVertex(vertex1);
			tempGraph.RemoveVertex(vertex2);
			
			// Check adjacence
			if(IsAdjacent(vertex1, vertex2))
			{
				splitPair->push_back(VertexPair(vertex1, vertex2));
				continue;
			}
			
			// Check paths
			bool noPath = false;
			for(Map::iterator startMapIter = tempGraph.mMap.begin(); startMapIter != tempGraph.mMap.end(); startMapIter++)
			{
				for(Map::iterator depMapIter = tempGraph.mMap.begin(); depMapIter != tempGraph.mMap.end(); depMapIter++)
				{
					if((*startMapIter).first == (*depMapIter).first)
						continue;
					
					if(!tempGraph.HasPath((*startMapIter).first, (*depMapIter).first))
					{
						noPath = true;
						break;
					}
				}
				
				if(noPath)
					break;
			}
			
			if(noPath)
				splitPair->push_back(VertexPair(vertex1, vertex2));
		}
	}
	
	return (unsigned int)splitPair->size();
}

unsigned int Graph::GetCutset(VertexSet* const cutset)
{
	for(Map::iterator mapIter = mMap.begin(); mapIter != mMap.end(); mapIter++)
	{
		Graph copiedGraph(this);
		copiedGraph.RemoveVertex((*mapIter).first);
		
		for(Map::iterator startMapIter = copiedGraph.mMap.begin(); startMapIter != copiedGraph.mMap.end(); startMapIter++)
		{
			for(Map::iterator depMapIter = copiedGraph.mMap.begin(); depMapIter != copiedGraph.mMap.end(); depMapIter++)
			{
				if((*startMapIter).first == (*depMapIter).first)
					continue;
				
				if(!copiedGraph.HasPath((*startMapIter).first, (*depMapIter).first))
					cutset->insert((*mapIter).first);
			}
		}
	}
	
	return (unsigned int)cutset->size();
}

GraphType Graph::GetType(const VertexPair &referenceVertexPair)
{
	VertexSet vertices;
	EdgeSet edges;
	
	GetVertices(&vertices);
	GetEdges(&edges);
	
	if(vertices.size() == 2 && edges.size() == 1)
	{
		return GraphType::TRIVIAL;
	}
	else
	{
		// Get the split pair
		vector<VertexPair> sp;
		GetSplitPairs(&sp);
		
		// Check whether the reference vertex pair is included in the split pairs
		for(vector<VertexPair>::iterator iter = sp.begin(); iter != sp.end(); iter++)
		{
			if(((*iter).first == referenceVertexPair.first && (*iter).second == referenceVertexPair.second) || ((*iter).second == referenceVertexPair.first && (*iter).first == referenceVertexPair.second))
			{
				return GraphType::PARALLEL;
			}
		}
		
		if(!IsBiconnected())
			return GraphType::SERIES;
		
		return GraphType::RIGID;
	}
}

bool Graph::Contains(Graph &graph)
{
	EdgeSet edges, tempEdges;
	GetEdges(&edges);
	graph.GetEdges(&tempEdges);
	
	for(EdgeSet::iterator tempEdgeIter = tempEdges.begin(); tempEdgeIter != tempEdges.end(); tempEdgeIter++)
	{
		bool edgeFound = false;
		for(EdgeSet::iterator edgeIter = edges.begin(); edgeIter != edges.end(); edgeIter++)
		{
			if((*tempEdgeIter)->id == (*edgeIter)->id)
			{
				edgeFound = true;
				break;
			}
		}
		if(!edgeFound)
			return false;
	}
	
	return true;
}

void Graph::Print()
{
	for(Map::iterator mapIter = mMap.begin(); mapIter != mMap.end(); mapIter++)
	{
		// print vertex
		cout << (*mapIter).first << ": ";
		
		// print edges and vertex
		for(ElementSet::iterator elementIter = (*mapIter).second.begin(); elementIter != (*mapIter).second.end(); elementIter++)
		{
			cout << "(" << (*elementIter).first->id << "," << (*elementIter).second << ") ";
		}
		
		cout << endl;
	}
}

//============================================================
// Manages the graph
//============================================================
void Graph::SplitByVertexPair(const VertexPair &splitPair, vector<Graph*> * splitComponents)
{
	// The visited list of vertices
	VertexSet visitedVertex;
	visitedVertex.insert(splitPair.first);
	visitedVertex.insert(splitPair.second);
	
	// In case of adjacent
	if(IsAdjacent(splitPair.first, splitPair.second))
	{
		Graph* tempGraph = NULL;
		ElementSet* elements = &mMap[splitPair.first];
		for(ElementSet::iterator elemIter = elements->begin(); elemIter != elements->end(); elemIter++)
		{
			if((*elemIter).second == splitPair.second)
			{
				tempGraph = new Graph();
				
				tempGraph->AddEdge((*(*elemIter).first).vertex_pair.first, (*(*elemIter).first).vertex_pair.second, *(*elemIter).first);
				splitComponents->push_back(tempGraph);
			}
		}
	}
	
	for(Map::iterator tempMapIter = mMap.begin(); tempMapIter != mMap.end(); tempMapIter++)
	{
		Vertex startVertex = (*tempMapIter).first;
		
		// Return if already visited
		if(visitedVertex.find(startVertex) != visitedVertex.end())
			continue;
		
		EdgeSet visitedEdges;
		stack<Vertex> vstack;
		
		// Add the start vertex
		vstack.push(startVertex);
		
		Graph* subGraph = NULL;
		do
		{
			// Pop from the stack
			Vertex vertex = vstack.top();
			vstack.pop();
			visitedVertex.insert(vertex);
			
			// Get the connected vertices and edges
			ElementSet* elements = &(mMap[vertex]);
			
			for(ElementSet::iterator elemIter = elements->begin(); elemIter != elements->end(); elemIter++)
			{
				Edge* edge = (*elemIter).first;
				Vertex adjVertex = (*elemIter).second;
				
				if(visitedEdges.find(edge) == visitedEdges.end())
				{
					if(!subGraph)
						subGraph = new Graph();
					
					// Copy the connected elements
					subGraph->AddEdge(edge->vertex_pair.first, edge->vertex_pair.second, edge);
					
					// Add to the visited list
					visitedEdges.insert(edge);
					
					// Push the adjacent vertices to the stack
					if(adjVertex != splitPair.first && adjVertex != splitPair.second)
						vstack.push(adjVertex);
				}
			}
		} while(vstack.size() != 0);
		
		// Add to the graph list
		splitComponents->push_back(subGraph);
	}
}

void Graph::SplitByCutVertex(const Vertex &cutVertex, vector<Graph*> * splitComponents)
{
	// Visited list of vertices
	VertexSet visitedVertex;
	visitedVertex.insert(cutVertex);
	
	for(Map::iterator tempMapIter = mMap.begin(); tempMapIter != mMap.end(); tempMapIter++)
	{
		Vertex startVertex = (*tempMapIter).first;
		
		// Return if already visited
		if(visitedVertex.find(startVertex) != visitedVertex.end())
			continue;
		
		EdgeSet visitedEdges;
		stack<Vertex> vstack;
		
		// Add the start vertex to the stack
		vstack.push(startVertex);
		
		Graph *subGraph = NULL;
		do
		{
			// Pop from the stack
			Vertex vertex = vstack.top();
			vstack.pop();
			visitedVertex.insert(vertex);
			
			// Get the connected vertices and edges
			ElementSet *elements = &(mMap[vertex]);
			
			for(ElementSet::iterator elemIter = elements->begin(); elemIter != elements->end(); elemIter++)
			{
				Edge* edge = (*elemIter).first;
				Vertex adjVertex = (*elemIter).second;
				
				if(visitedEdges.find(edge) == visitedEdges.end())
				{
					if(!subGraph)
						subGraph = new Graph();
					
					// Copy the connected elements
					subGraph->AddEdge(edge->vertex_pair.first, edge->vertex_pair.second, edge);
					
					// Add to the visited list
					visitedEdges.insert(edge);
					
					// Push the adjacent vertices to the stack
					if(adjVertex != cutVertex)
						vstack.push(adjVertex);
				}
			}
		} while(vstack.size() != 0);
		
		// Add to the graph list
		splitComponents->push_back(subGraph);
	}
}

void Graph::SplitMaximal(const Edge &wdfReferenceEdge, pair<Graph*, vector<Graph*>> * splitComponents, vector<VertexPair> * splitPairs)
{
//	cout << "========================================" << endl;
//	cout << "Split a graph by maximal" << endl;
//	cout << "========================================" << endl;
	
	// Get separation pair
	vector<VertexPair> separationPair;
	if(GetSeparationPairs(&separationPair) == 0)
	{
		Graph* tempGraph = new Graph(this);
		splitComponents->first = tempGraph;
		
//		cout << "no separation paris" << endl;
//		cout << "split compoenent: " << endl;
//		tempGraph->Print();
	}
	else
	{
//		cout << "separation pairs: ";
//		for(vector<VertexPair>::iterator pairIter = separationPair.begin(); pairIter != separationPair.end(); pairIter++)
//			cout << "(" << (*pairIter).first << "," << (*pairIter).second << ") ";
//		cout << endl;
		
		bool isFirstIter = true;
		
		// Split the graph by each separation pair
		for(vector<VertexPair>::iterator spIter = separationPair.begin(); spIter != separationPair.end(); spIter++)
		{
//			cout << "split by " << "(" << (*spIter).first << "," << (*spIter).second << ") :" << endl;
			
			vector<Graph*> splits;
			SplitByVertexPair(*spIter, &splits);
			
			vector<Graph*> otherGraphs;
			Graph* rigidGraph = NULL;
			
//			int iGraphIndex = 1;
			for(vector<Graph*>::iterator graphIter = splits.begin(); graphIter != splits.end(); graphIter++)
			{
//				cout << "graph index: " << iGraphIndex << endl;
//				(*graphIter)->Print();
				
				bool refEdgeFound = false;
				
				// Find the split component which includes the reference edge
				for(Map::iterator mapIter = (*graphIter)->mMap.begin(); mapIter != (*graphIter)->mMap.end(); mapIter++)
				{
					ElementSet *elements = &((*mapIter).second);
					for(ElementSet::iterator elemIter = elements->begin(); elemIter != elements->end(); elemIter++)
					{
						if((*elemIter).first->id == wdfReferenceEdge.id)
						{
							refEdgeFound = true;
							break;
						}
					}
					
					if(refEdgeFound)
						break;
				}
//				cout << "refEdgeFound: " << (refEdgeFound ? "true" : "false") << endl;
				
				// At the first split pair, just get all graphs
				if(isFirstIter)
				{
					if(refEdgeFound)
					{
						splitComponents->first = *graphIter;
//						cout << "added the graph to the splitComponents->first" << endl;
					}
					else
					{
						splitComponents->second.push_back(*graphIter);
						splitPairs->push_back(*spIter);
//						cout << "added the graph to the splitComponents->second" << endl;
					}
				}
				else
				{
					if(refEdgeFound)
					{
						rigidGraph = *graphIter;
//						cout << "set to the rigidGraph variable" << endl;
					}
					else
					{
						otherGraphs.push_back(*graphIter);
//						cout << "added to the otherGraphs" << endl;
					}
				}
			}		// end of graph iteration by separation pair
			
			// Compare with previous split-components
			if(!isFirstIter)
			{
//				cout << "\nrigidGraph and splitComponents->first" << endl;
				EdgeSet prevEdges, newEdges;
				splitComponents->first->GetEdges(&prevEdges);
				rigidGraph->GetEdges(&newEdges);
				
				for(EdgeSet::iterator edgeIter1 = prevEdges.begin(); edgeIter1 != prevEdges.end(); edgeIter1++)
				{
					EdgeSet::iterator edgeIter2;
					for(edgeIter2 = newEdges.begin(); edgeIter2 != newEdges.end(); edgeIter2++)
						if((*edgeIter1)->id == (*edgeIter2)->id)
							break;
					
					// No edge in the new components
					if(edgeIter2 == newEdges.end())
						splitComponents->first->RemoveEdge(*edgeIter1);
				}
//				cout << "end of edge removing" << endl;
//				cout << "the edges of splitComponents.first: ";
				prevEdges.clear();
				splitComponents->first->GetEdges(&prevEdges);
//				for(EdgeSet::iterator prevEdgeIter = prevEdges.begin(); prevEdgeIter != prevEdges.end(); prevEdgeIter++)
//					cout << (*prevEdgeIter)->id << " ";
//				cout << endl;
				
				vector<Graph*> graphsToAdd;
				vector<VertexPair> VertexPairsToAdd;
				
//				cout << "\notherGraphs and splitComponents->second" << endl;
				for(vector<Graph*>::iterator newGraphIter = otherGraphs.begin(); newGraphIter != otherGraphs.end(); newGraphIter++)
				{
//					cout << "new one:" << endl;
//					(*newGraphIter)->Print();
					
					// Check whether the new split graph includes the old split graphs
					vector<Graph*>::iterator oldGraphIter = splitComponents->second.begin();
					vector<VertexPair>::iterator oldVertexPairIter = splitPairs->begin();
					bool added = false;
					
					while(oldGraphIter != splitComponents->second.end())
					{
//						cout << "old one:" << endl;
//						(*oldGraphIter)->Print();
						
						if((*oldGraphIter)->Contains(**newGraphIter))
						{
							// Remove the new one if the old graph includes it.
//							cout << "the old one includes includes the new one" << endl;
							delete *newGraphIter;
							break;
						}
						else if((*newGraphIter)->Contains(**oldGraphIter))
						{
							// Remove the old graph then add the new graph to the list of split graphs if the new one includes the old one
//							cout << "the new one includes the old one" << endl;
							delete *oldGraphIter;
							oldGraphIter = splitComponents->second.erase(oldGraphIter);
							oldVertexPairIter = splitPairs->erase(oldVertexPairIter);
							
							// Add the new graph
							if(!added)
							{
								graphsToAdd.push_back(*newGraphIter);
								VertexPairsToAdd.push_back(*spIter);
								added = true;
							}
						}
						else
						{
							// Add the new graph to the list of split graph
//							cout << "no inclusion" << endl;
							if(!added)
							{
								graphsToAdd.push_back(*newGraphIter);
								VertexPairsToAdd.push_back(*spIter);
								added = true;
							}
							
							oldGraphIter++;
							oldVertexPairIter++;
						}
					}
				}	// end of new graphs
				
				// Add to the list
				for(unsigned int i=0; i<graphsToAdd.size(); i++)
				{
					splitComponents->second.push_back(graphsToAdd[i]);
					splitPairs->push_back(VertexPairsToAdd[i]);
				}
				
				delete rigidGraph;
			}
			else
			{
				isFirstIter = false;
//				cout << "end of first iteration\n" << endl;
			}
		}
	}
}

//============================================================
// Make SPQR tree
//============================================================
void Graph::MakeSPQRTree(SPQRTree *tree)
{
	// Copy the graph
	Graph tempGraph(this);
	
	// Find reference edge
	Edge refEdge;
	VertexPair vertices;
	FindReferenceEdgeWithVertexPair(&refEdge, &vertices);
//	cout << "initial reference edge: " << refEdge.id << endl;
//	cout << "initial reference vertex: (" << vertices.first << "," << vertices.second << ")" << endl;
	
	tempGraph.MakeTree(tree, refEdge, vertices);
}

void Graph::MakeTree(SPQRTree *tree, const Edge &referenceEdge, const VertexPair &referenceVertices)
{
//	cout << "========================================" << endl;
//	cout << "Start to crate a tree" << endl;
//	cout << "========================================" << endl;
//	cout << "reference edge: " << referenceEdge.id << endl;
//	cout << "reference vertex pair: (" << referenceVertices.first << "," << referenceVertices.second << ")" << endl;
	
	// Remove the reference edge
	RemoveEdge(&referenceEdge);
//	cout << "removed reference edge" << endl;
//	Print();
	
	// Add the reference edge to the root of the tree
	tree->AddRootLeaf(referenceEdge);
//	cout << "added the leaf" << endl;
//	tree->Print();
	
	// Get the cutset
	VertexSet cutset;
	GetCutset(&cutset);
//	cout << "cutset:";
//	for(VertexSet::iterator iter = cutset.begin(); iter != cutset.end(); iter++)
//		cout << (*iter) << " ";
//	cout << endl;
	
	if(cutset.size() > 0)
	{
		//============================================================
		// Cutset exists
		//============================================================
		// Split the graph by first cur-vertex
		vector<Graph *> splits;
		SplitByCutVertex(*cutset.begin(), &splits);
		
		for(vector<Graph*>::iterator graphIter = splits.begin(); graphIter != splits.end(); graphIter++)
		{
//			cout << "split graph: " << endl;
//			(*graphIter)->Print();
			
			// Create the virtual edge
			Edge *virtualEdge = new Edge();
			virtualEdge->id = "VE" + to_string(++g_uiVirtualEdgeIndex);
//			cout << "created the virtual edge: " << virtualEdge->id << endl;
			
			// Get the reference vertex
			Vertex tempRefVertex;
			Map *tempMap = &((*graphIter)->mMap);
			if(tempMap->find(referenceVertices.first) != tempMap->end())
				tempRefVertex = referenceVertices.first;
			else
				tempRefVertex = referenceVertices.second;
			
			// Add a virtual edge to each split component
			(*graphIter)->AddEdge(*cutset.begin(), tempRefVertex, virtualEdge);
//			cout << "added a virtual edge:" << endl;
//			(*graphIter)->Print();
			
			// Add the each virtual edge to the root of the tree
			tree->AddLeaf(virtualEdge, SPQRTreeNodeType::SERIES);
//			cout << "added the virtual edge to the tree:" << endl;
//			tree->Print();
			
			// Process recursively
			SPQRTree subTree;
			(*graphIter)->MakeTree(&subTree, *virtualEdge, VertexPair(tempRefVertex, *cutset.begin()));
			
			// Join the subtree
			tree->JoinSubTree(subTree);
//			cout << "merged a sub tree:" << endl;
//			tree->Print();
			
			delete virtualEdge;
			delete *graphIter;
		}
	}
	else
	{
		//============================================================
		// No cuset: minimum size of graph(tree)
		//============================================================
//		cout << "no cutset" << endl;
		switch(GetType(referenceVertices))
		{
			case GraphType::TRIVIAL:
			{
				// Get the edge
				EdgeSet edges;
				GetEdges(&edges);
				
				// Add the leaf to the tree
				tree->AddLeaf(*edges.begin(), SPQRTreeNodeType::SERIES);
				
//				cout << "the type of tree is trivial:" << endl;
//				tree->Print();
				
				break;
			}
			case GraphType::SERIES:
			{
				// Get the edge
				EdgeSet edges;
				GetEdges(&edges);
				
				// Add the leaf to the tree
				for(EdgeSet::iterator edgeIter = edges.begin(); edgeIter != edges.end(); edgeIter++)
					tree->AddLeaf(*edgeIter, SPQRTreeNodeType::SERIES);
				
//				cout << "the type of tree is series:" << endl;
//				tree->Print();
				
				break;
			}
			case GraphType::PARALLEL:
			{
//				cout << "the type of tree is parallel" << endl;
				
				// Split the graph by split pair
				vector<Graph*> splits;
				SplitByVertexPair(referenceVertices, &splits);
//				cout << "split by reference vertex pair:" << endl;
				
//				int iGraphIndex = 1;
				for(vector<Graph*>::iterator graphIter = splits.begin(); graphIter != splits.end(); graphIter++)
				{
//					cout << "graph index: " << iGraphIndex << endl;
//					(*graphIter)->Print();
					
					// Create the virtual edge
					Edge *virtualEdge = new Edge();
					virtualEdge->id = "VE" + to_string(++g_uiVirtualEdgeIndex);
//					cout << "created a virtual edge: " << virtualEdge->id << endl;
					
					// Add the reference edge to the split graph
					(*graphIter)->AddEdge(referenceVertices.first, referenceVertices.second, virtualEdge);
//					cout << "added a virtual edge to the graph: " << endl;
//					(*graphIter)->Print();
					
					// Add each virtual edge to the root of tree
					tree->AddLeaf(virtualEdge, SPQRTreeNodeType::PARALLEL);
//					cout << "added a virtual edge to the tree:" << endl;
//					tree->Print();
					
					// Make the subtree
					SPQRTree subTree;
					(*graphIter)->MakeTree(&subTree, *virtualEdge, referenceVertices);
					
					// Join the subtree
					tree->JoinSubTree(subTree);
//					cout << "merged a subtree:" << endl;
//					tree->Print();
					
					delete virtualEdge;
					delete *graphIter;
				}
				break;
			}
			case GraphType::RIGID:
			{
				// Re-connect the reference edge
				AddEdge(referenceVertices.first, referenceVertices.second, referenceEdge);
//				cout << "added a reference edge: " << endl;
//				Print();
				
				// Split by maximal
				pair<Graph*, vector<Graph*>> splitComponents;
				vector<VertexPair> splitPairs;
				SplitMaximal(referenceEdge, &splitComponents, &splitPairs);
				
//				cout << "split by maximal:" << endl;
//				cout << "split pairs: ";
//				for(vector<VertexPair>::iterator spIter = splitPairs.begin(); spIter != splitPairs.end(); spIter++)
//					cout << "(" << (*spIter).first << "," << (*spIter).second << ") ";
//				cout << endl;
				
//				cout << "reference graph: " << endl;
//				if(splitComponents.first)
//					splitComponents.first->Print();
				
//				cout << "split graphs: " << endl;
//				for(vector<Graph*>::iterator iter = splitComponents.second.begin(); iter != splitComponents.second.end(); iter++)
//					(*iter)->Print();
//				cout << endl;
				
				// Add the edges to the tree
				set<string> addedEdges;
				for(Map::iterator mapIter = splitComponents.first->mMap.begin(); mapIter != splitComponents.first->mMap.end(); mapIter++)
				{
					for(ElementSet::iterator elemIter = (*mapIter).second.begin(); elemIter != (*mapIter).second.end(); elemIter++)
					{
						Edge* edge = (*elemIter).first;
						
						// New edge
						if(addedEdges.find(edge->id) == addedEdges.end() && edge->id != referenceEdge.id)
						{
							// Add the leaf to the tree
							tree->AddLeaf((*elemIter).first, SPQRTreeNodeType::RIGID);
//							cout << "added an edge to the tree: " << edge->id << endl;
							
							// Add the vertex-pair to the tree
							SPQRTreeNode* node = tree->GetRootNode();
							VertexPair pair((*mapIter).first, (*elemIter).second);
							node->AddVertexPair(edge->id, edge->vertex_pair);
							
							// Add to the list
							addedEdges.insert(edge->id);
						}
					}
				}
//				tree->Print();
				
				// Deallocate the graph
				delete splitComponents.first;
				
				for(unsigned int index=0; index < splitComponents.second.size(); index++)
				{
					// Create a virtual edge
					Edge *virtualEdge = new Edge();
					virtualEdge->id = "VE" + to_string(++g_uiVirtualEdgeIndex);
//					cout << "created a virtual edge: " << virtualEdge->id << endl;
					
					// Add the virtual edge to the tree
					tree->AddLeaf(virtualEdge, SPQRTreeNodeType::RIGID);
//					cout << "added a virtual edge to the tree:" << endl;
//					tree->Print();
					
					// Add the vertex pair
					SPQRTreeNode* node = tree->GetRootNode();
					node->AddVertexPair(virtualEdge->id, splitPairs[index]);
					
					// Add to the subgraph
					splitComponents.second[index]->AddEdge(splitPairs[index].first, splitPairs[index].second, virtualEdge);
//					cout << "added a virtual edge to the split graph:" << endl;
//					splitComponents.second[index]->Print();
					
					// Make the tree
					SPQRTree subTree;
					splitComponents.second[index]->MakeTree(&subTree, *virtualEdge, splitPairs[index]);
					
					// Join the subtree
					tree->JoinSubTree(subTree);
//					cout << "merged a subtree:" << endl;
//					tree->Print();
					
					delete virtualEdge;
					delete splitComponents.second[index];
				}
				
				// Add the vertex-pair of the reference edge
				SPQRTreeNode* node = tree->GetRootNode();
				VertexPair pair = referenceVertices;
				node->AddVertexPair(referenceEdge.id, pair);
			}
		}
	}
}
