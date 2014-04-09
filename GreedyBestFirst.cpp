#include "GreedyBestFirst.h"
#include <iostream>

// comparison, not case sensitive.
bool CompareFunc(const Tree::Node* first, const Tree::Node* second)
{
	return first->vert->hVal < second->vert->hVal;
}

bool GreedyBestFirst::DoSearch(Tree::Node* currNode)
{
	if (currNode == NULL)
		currNode = m_searchTree->getHead();

	m_path.push_back(currNode->vert);
	currNode->path = m_path;

	// Add to closed list
	m_closedList.insert(make_pair(currNode->vert, currNode));

	// Return true if dest has been reached
	if (currNode->vert == m_end)
		return true;

	// fetch edges connected to current vertex
	list<Graph::Edge*> conns = m_searchSpace->getConnectedEdges(currNode->vert);

	// create a node for each connection if one of following is true:
	//	- The vertex has not been visited
	for (list<Graph::Edge*>::iterator itr = conns.begin(); itr != conns.end(); ++itr)
	{
		// Compute cost of this path
		int nextCost = currNode->cost + (*itr)->cost;

		Graph::Vertex* endVert;
		if ((*itr)->ends[0] == currNode->vert)
			endVert = (*itr)->ends[1];
		else
			endVert = (*itr)->ends[0];

		// Figure out which end is the start, add child accordingly if not already visited
		if (!endVert->visited)
			m_searchTree->addAsChild(nextCost, endVert, currNode, &m_path);
	}

	// fetch children of current node
	list<Tree::Node*> currChildren = currNode->getChildren();

	// if no children and not at dest, not on correct path
	if (currChildren.empty())
	{
		return false;
	}

	// append to open list
	m_openList.insert(m_openList.end(), currChildren.begin(), currChildren.end());
	sort(m_openList.begin(), m_openList.end(), CompareFunc);
	if( m_verbose )
		displayOpenList();

	// mark current vertex visited
	currNode->vert->visited = true;

	Tree::Node* nextNode;
	// get first entry in open list and remove from open list
	do
	{
		if (m_openList.empty())
			return false;
		nextNode = *m_openList.begin();
		m_openList.pop_front();
	}
	while( nextNode->vert->visited );

	// set path to next node's path
	m_path = nextNode->path;

	if (m_verbose)
	{
		displayPath(currNode);
		cout << "Expanding node " << nextNode->vert->id << endl;
	}

	if (DoSearch(nextNode))
		return true;
	

	// default case, no path found
	m_path.clear();
	return false;
}