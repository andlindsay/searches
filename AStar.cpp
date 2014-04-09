#include "AStar.h"
#include <stack>
#include <iostream>
#include <functional>

// comparison, not case sensitive.
bool AStarCompareFunc(const Tree::Node* first, const Tree::Node* second)
{
	return (first->cost + first->vert->hVal) < (second->cost + second->vert->hVal);
}

deque<Tree::Node*>::iterator AStar::OpenListFind(Tree::Node* find)
{
	//scan deque for vertex
	deque<Tree::Node*>::iterator itr = m_openList.begin();
	while (itr != m_openList.end())
	{
		if ((*itr)->vert->id == find->vert->id)
			return itr;
		++itr;
	}
	return m_openList.end();
}

bool AStar::inPath(Graph::Vertex* vert, deque<Graph::Vertex*> path)
{
	if (path.size() == 0)
		return false;
	return find(path.begin(), path.end(), vert) != path.end();
}

bool AStar::DoSearch(Tree::Node* currNode)
{
	m_openList.push_back(m_searchTree->getHead());
	m_searchTree->getHead()->path.push_back(m_searchTree->getHead()->vert);
	while (!m_openList.empty())
	{
		currNode = m_openList.front();
		m_openList.pop_front();
		m_path = currNode->path;

		if (m_verbose)
		{
			displayPath(currNode);
			cout << "Expanding node " << currNode->vert->id << " (Cost: "
				<< currNode->cost << " hVal: " << currNode->vert->hVal << ")" << endl;
		}

		// Add to closed list
		m_closedList.insert(make_pair(currNode->vert, currNode));

		// Return true if dest has been reached
		if (currNode->vert == m_end)
			return true;

		// fetch edges connected to current vertex
		list<Graph::Edge*> conns = m_searchSpace->getConnectedEdges(currNode->vert);

		// create a node for each connection if one of following is true:
		//	- The vertex has not been visited
		//	- The cost to get to the vertex via this path is lower than the current best (open and closed list)
		for (list<Graph::Edge*>::iterator itr = conns.begin(); itr != conns.end(); ++itr)
		{
			// Compute cost of this path
			int nextCost = currNode->cost + (*itr)->cost;

			Graph::Vertex* endOfEdge = NULL;
			if ((*itr)->ends[0] == currNode->vert)
				endOfEdge = (*itr)->ends[1];
			else
				endOfEdge = (*itr)->ends[0];
			Tree::Node* findMe;
			findMe = new Tree::Node(endOfEdge, 0);

			// check if already on open list
			deque<Tree::Node*>::iterator openListLoc = OpenListFind(findMe);

			// check for closed list entry
			map<Graph::Vertex*, Tree::Node*>::iterator loc = m_closedList.find(findMe->vert);

			if (openListLoc != m_openList.end() && nextCost < (*openListLoc)->cost)
			{
				if (m_verbose)
					cout << "(Open List) Found better path for " << (*openListLoc)->vert->id << "(prev: " <<
						(*openListLoc)->cost << " new: " << nextCost << ")" << endl;

				// fetch parent (second to last entry in path on node)
				deque<Graph::Vertex*>::iterator parentLoc = --(*openListLoc)->path.end();
				Graph::Vertex* parentVert = *--parentLoc;

				// look up address of node current storing parentVert, can be in closed or open (if better path found prior)
				Tree::Node* parentNode = NULL;
				map<Graph::Vertex*, Tree::Node*>::iterator closedLoc = m_closedList.find(parentVert);
				if (closedLoc != m_closedList.end())
					parentNode = closedLoc->second;
				else
				{
					Tree::Node* compareNode = new Tree::Node(parentVert, 0);
					deque<Tree::Node*>::iterator itr = find(m_openList.begin(), m_openList.end(), compareNode);
					parentNode = *find(m_openList.begin(), m_openList.end(), compareNode);
					delete compareNode;
				}

				// remove the node that needs updating from the old parent
				parentNode->children.remove(*openListLoc);

				// update path on node
				(*openListLoc)->path = deque<Graph::Vertex*>(m_path);
				(*openListLoc)->path.push_back((*openListLoc)->vert);

				// update cost on node
				(*openListLoc)->cost = nextCost;

				//add to children of currNode
				currNode->addChild(*openListLoc);

				// remove old entry from open list
				m_openList.erase(openListLoc);
				continue;
			}
			else if (loc != m_closedList.end() && nextCost < loc->second->cost)
			{
				if (m_verbose)
					cout << "(Closed List) Found better path for " << loc->second->vert->id << "(prev: " <<
						loc->second->cost << " new: " << nextCost << ")" << endl;

				// fetch parent (second to last entry in path on node)
				deque<Graph::Vertex*>::iterator parentLoc = --loc->second->path.end();
				Graph::Vertex* parentVert = *--parentLoc;

				// look up address of node current storing parentVert, can be in closed or open (if better path found prior)
				Tree::Node* parentNode = NULL;
				map<Graph::Vertex*, Tree::Node*>::iterator closedLoc = m_closedList.find(parentVert);
				if (closedLoc != m_closedList.end())
					parentNode = closedLoc->second;
				else
				{
					Tree::Node* compareNode = new Tree::Node(parentVert, 0);
					parentNode = *find(m_openList.begin(), m_openList.end(), compareNode);
					delete compareNode;
				}

				// remove the node that needs updating from the old parent
				parentNode->children.remove(loc->second);

				// update path on node
				loc->second->path = deque<Graph::Vertex*>(m_path);
				loc->second->path.push_back(loc->second->vert);

				// update cost on node
				loc->second->cost = nextCost;

				//add to children of currNode
				currNode->addChild(loc->second);

				// remove from closed list
				m_closedList.erase(loc);
			}
			else
			{
				// Not on closed or open list, add to search space
				if (inPath(endOfEdge, m_path))
					continue;
				deque<Graph::Vertex*> path(m_path);
				path.push_back(endOfEdge);
				m_searchTree->addAsChild(nextCost, endOfEdge, currNode, &path);
			}
		}

		// fetch children of current node
		list<Tree::Node*> currChildren = currNode->getChildren();

		// if no children and not at dest, not on correct path
		if (!currChildren.empty())
		{
			// append to open list
			m_openList.insert(m_openList.end(), currChildren.begin(), currChildren.end());
			sort(m_openList.begin(), m_openList.end(), AStarCompareFunc);
			if (m_verbose)
				cout << "Open List Size: " << m_openList.size() << endl;
			if (m_verbose)
				displayOpenList();
		}

		// mark current vertex visited
		currNode->vert->visited = true;
	}
	// default case, no path found
	m_path.clear();
	return false;
}