#include "DFS.h"
#include "BFS.h"
#include "GreedyBestFirst.h"
#include "AStar.h"

#include <string>
#include <sstream>
#include <fstream>

list<Graph::Vertex> verts;
list<Graph::Edge> edges;
bool g_bVerbose = false;
Graph* g_pTheGraph = NULL;

Graph createLineGraph()
{
	verts.push_back(Graph::Vertex(1, 5));
	verts.push_back(Graph::Vertex(2, 5));
	verts.push_back(Graph::Vertex(3, 5));
	verts.push_back(Graph::Vertex(4, 5));
	verts.push_back(Graph::Vertex(5, 5));

	list<Graph::Vertex>::iterator itr = verts.begin();
	do
	{
		Graph::Vertex *prev = &(*itr++);
		Graph::Vertex* ends[2] = { prev, &(*itr) };
		edges.push_back(Graph::Edge(ends, 1));
	}
	while(itr != --verts.end());

	Graph theGraph(&verts, &edges);
	return theGraph;
}

// three vertices with edges 1-2, 1-3
Graph createTriangleGraph()
{
	verts.push_back(Graph::Vertex(1, 5));
	verts.push_back(Graph::Vertex(2, 5));
	verts.push_back(Graph::Vertex(3, 5));

	list<Graph::Vertex>::iterator itr = ++verts.begin();
	do
	{
		Graph::Vertex* ends[2] = { &(*verts.begin()), &(*itr++) };
		edges.push_back(Graph::Edge(ends, 1));
	} while (itr != verts.end());

	Graph theGraph(&verts, &edges);
	return theGraph;
}

// used to process input
// NOTE: Absolutely no format checking, this is purely for testing
Graph createGeneralGraph(string fileName)
{
	verts.clear();
	edges.clear();
	int lineNum = 1;
	ifstream inFile(fileName);
	while (inFile.good())
	{
		char line[20];
		inFile.getline(line, 20);
		stringstream lineStrm(string(line + sizeof(char)));
		if (line[0] == 'v')
		{
			int id, hVal;

			lineStrm >> id;
			lineStrm >> hVal;

			verts.push_back(Graph::Vertex(id, hVal));
		}
		else if (line[0] == 'e')
		{
			int id1, id2, cost;

			lineStrm >> id1;
			lineStrm >> id2;
			lineStrm >> cost;

			list<Graph::Vertex>::iterator itr1 = verts.begin();
			advance(itr1, id1 - 1);
			list<Graph::Vertex>::iterator itr2 = verts.begin();
			advance(itr2, id2 - 1);

			Graph::Vertex* ends[2] = { &(*itr1), &(*itr2) };
			edges.push_back(Graph::Edge(ends, cost));
		}
		else
			cout << "Invalid format at line " << lineNum << endl;
		++lineNum;
	}

	Graph theGraph(&verts, &edges);
	return theGraph;
}

Tree::Node testNode(Graph* theGraph)
{
	Graph::Vertex theVert = *theGraph->getVertices()->begin();
	Tree::Node theNode(&theVert, 0);
	return theNode;
}

bool TestDFS(Graph* theGraph)
{
	Graph::Vertex* start = &(*theGraph->getVertices()->begin());
	Graph::Vertex* end = &(*(--theGraph->getVertices()->end()));
	DFS searcher(theGraph, start, end);
	bool success = searcher.DoSearch();
	if (success)
		searcher.displayPath();
	return success;
}

bool TestBFS(Graph* theGraph)
{
	Graph::Vertex* start = &(*theGraph->getVertices()->begin());
	Graph::Vertex* end = &(*(--theGraph->getVertices()->end()));
	BFS searcher(theGraph, start, end);
	bool success = searcher.DoSearch();
	if (success)
		searcher.displayPath();
	return success;
}

bool TestGBFS(Graph* theGraph)
{
	Graph::Vertex* start = &(*theGraph->getVertices()->begin());
	Graph::Vertex* end = &(*(--theGraph->getVertices()->end()));
	GreedyBestFirst searcher(theGraph, start, end);
	bool success = searcher.DoSearch();
	if (success)
		searcher.displayPath();
	return success;
}

bool TestAStar(Graph* theGraph)
{
	Graph::Vertex* start = &(*theGraph->getVertices()->begin());
	Graph::Vertex* end = &(*(--theGraph->getVertices()->end()));
	AStar searcher(theGraph, start, end);
	bool success = searcher.DoSearch();
	if (success)
		searcher.displayPath();
	return success;
}

// This function manually builds a tree to represent the search
//	space of the graph returned by CreateLineGraph
Tree createSimpleTree(Graph* theGraph)
{
	list<Graph::Vertex>* vertices = theGraph->getVertices();
	list<Graph::Vertex>::iterator itr = vertices->begin();
	Graph::Vertex theVert = *itr++;
	Tree theTree(&theVert);
	theVert = *itr++;
	theTree.addAsChild(0, &theVert, theTree.getHead());
	return theTree;
}

// reset visited to false for all vertices
void resetVerts()
{
	for (list<Graph::Vertex>::iterator itr = verts.begin(); itr != verts.end(); ++itr)
		itr->visited = false;
}

int getOption(string prompt)
{
	string line; 
	int selection = 0;
	while( selection == 0 )
	{
		cout << prompt << ": #";
		getline(cin, line);
		stringstream lineStrm(line);
		lineStrm >> selection;
	}
	return selection;
}

enum eSearches {
	eNONE,
	eDFS,
	eBFS,
	eGBFS,
	eAStar
};

int getSearch()
{
	cout <<
		"  Valid Options (select by number): " << endl <<
		"    1) DFS" << endl <<
		"    2) BFS" << endl <<
		"    3) Greedy Best First" << endl <<
		"    4) A*" << endl <<
		"    9) Main Menu" << endl << endl;
	
	return getOption("Selection");
}

void findVerts(int startVert, int endVert, Graph::Vertex* &start, Graph::Vertex* &end)
{
	list<Graph::Vertex>* theVerts = g_pTheGraph->getVertices();
	for(list<Graph::Vertex>::iterator itr = theVerts->begin(); itr != theVerts->end(); ++itr)
	{
		if( start && end )
			break;
			
		if( itr->id == startVert )
			start = &(*itr);
		if( itr->id == endVert )
			end = &(*itr);
	}
}

void runSearch( int selection )
{
	int startVert;
	int endVert;
	
	Graph::Vertex* start = NULL;
	Graph::Vertex* end = NULL;
	
	while( !start || !end )
	{
		startVert = getOption("Start Vertex");
		endVert = getOption("End Vertex");
		findVerts(startVert, endVert, start, end);
	}
	
	Search* searcher = NULL;
	if( selection == eSearches::eDFS )
		searcher = new DFS(g_pTheGraph, start, end);
		
	else if( selection == eSearches::eBFS )
		searcher = new BFS(g_pTheGraph, start, end);
		
	else if( selection == eSearches::eGBFS )
		searcher = new GreedyBestFirst(g_pTheGraph, start, end);
		
	else if( selection == eSearches::eAStar )
		searcher = new AStar(g_pTheGraph, start, end);
		
	searcher->setVerbose( g_bVerbose );
		
	bool success = searcher->DoSearch(NULL);		
	if( success )
		searcher->displayPath();
	else
		cout << "No path found." << endl;
	
	bool foundPath = TestDFS(g_pTheGraph);
	
	resetVerts();
}

void showHelp()
{
	cout <<
		"Valid Options (select by number): " << endl <<
		"  1) Read Graph" << endl <<
		"  2) Run Search" << endl <<
		"  3) Toggle Verbose Mode" << endl <<
		"  9) Exit" << endl << endl;
}


void selectionHandler(int choice)
{
	// read graph
	if( choice == 1 )
	{
		cout << "Filename: ";
		string filename;
		getline(cin, filename);
		if( g_pTheGraph )
			delete g_pTheGraph;
		g_pTheGraph = new Graph(createGeneralGraph(filename));
		if( g_pTheGraph )
		{
			cout << endl << "Results of Graph Read:" << endl;
			cout << *g_pTheGraph << endl;
		}
		else
			cout << "Failed to read graph." << endl;
	}
	// run a search
	else if( choice == 2 )
	{
		if( !g_pTheGraph )
		{
			cout << "No graph has been loaded." << endl << endl;
			return;
		}
		int selection = 0;
		while( (selection = getSearch()) != 9 )
			runSearch( selection );
		showHelp();
	}
	else if( choice == 3 )
	{
		g_bVerbose = !g_bVerbose;
		cout << "Verbose mode now ";
		if( g_bVerbose )
			cout << "ENABLED." << endl;
		else
			cout << "DISABLED." << endl;
		cout << endl;
	}
}

int main(int argc, const char* argv[])
{	
	showHelp();
	int selection = -1;
	while( (selection = getOption("Selection")) != 9 )
	{
		selectionHandler(selection);
	}

	return 0;
}