#include "DFS.h"
#include "BFS.h"
#include "GreedyBestFirst.h"
#include "AStar.h"

#include <ctime>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>

list<Graph::Vertex> verts;
list<Graph::Edge> edges;
bool g_bVerbose = false;
Graph* g_pTheGraph = NULL;

using namespace std::chrono;

// used to process input
// NOTE: Absolutely no format checking
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
	
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	
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
	
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	
	cout << "Duration: " << time_span.count() << endl;
	
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

void loadGraph(string filename)
{
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

void selectionHandler(int choice)
{
	// read graph
	if( choice == 1 )
	{
		cout << "Filename: ";
		string filename;
		getline(cin, filename);
		loadGraph(filename);
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
	if(argc == 2)
		loadGraph(string(argv[1]));
	showHelp();
	int selection = -1;
	while( (selection = getOption("Selection")) != 9 )
	{
		selectionHandler(selection);
	}

	return 0;
}