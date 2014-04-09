#include "../DFS.h"
#include "../BFS.h"
#include "../GreedyBestFirst.h"
#include "../AStar.h"

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

enum eSearches {
	eNONE,
	eDFS,
	eBFS,
	eGBFS,
	eAStar
};

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

void runSearches()
{
	
	Graph::Vertex* start = NULL;
	Graph::Vertex* end = NULL;
	
	int trials = 0;
	unsigned vertCount = verts.size();
	double timeBFS, timeDFS, timeGBFS, timeAStar;
	timeBFS = timeDFS = timeGBFS = timeAStar = 0.0;
	for( int startVert = 1; startVert <= vertCount; ++startVert )
	{
		for( int endVert = 1; endVert <= vertCount; ++endVert )
		{
			if( trials >= 1000 )
			{
				cout << "Averages: " << endl;
				cout << "\tBFS: " << timeBFS/(trials/3) << endl;
				cout << "\tDFS: " << timeDFS/(trials/3) << endl;
				cout << "\tGBFS: " << timeGBFS/(trials/3) << endl;
				return;
			}
			if( startVert == endVert )
				continue;
			else
				findVerts(startVert, endVert, start, end);
			
			for( int search = eDFS; search < eAStar; ++search )
			{
				++trials;
				high_resolution_clock::time_point t1 = high_resolution_clock::now();
				
				Search* searcher = NULL;
				
				if( search == eSearches::eDFS )
				{
					//cout << "DFS ";
					searcher = new DFS(g_pTheGraph, start, end);
				}
				else if( search == eSearches::eBFS )
				{
					//cout << "BFS ";
					searcher = new BFS(g_pTheGraph, start, end);
				}
				else if( search == eSearches::eGBFS )
				{
					//cout << "GBFS ";
					searcher = new GreedyBestFirst(g_pTheGraph, start, end);
				}
				else if( search == eSearches::eAStar )
				{
					//cout << "AStar ";
					searcher = new AStar(g_pTheGraph, start, end);
				}
					
				//searcher->setVerbose( true );
					
				bool success = searcher->DoSearch(NULL);
				
				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
				
				// if( success )
					// cout << "yes ";
				// else
					// cout << "no ";
				
				if( search == eSearches::eDFS )
					timeBFS += time_span.count();
				else if( search == eSearches::eBFS )
					timeDFS += time_span.count();
				else if( search == eSearches::eGBFS )
					timeGBFS += time_span.count();
				else if( search == eSearches::eAStar )
					timeAStar += time_span.count();

				delete searcher;
				//cout << time_span.count() << endl;
				resetVerts();
			}
		}
	}
	cout << "Averages: " << endl;
	cout << "\tBFS: " << timeBFS/trials << endl;
	cout << "\tDFS: " << timeDFS/trials << endl;
	cout << "\tGBFS: " << timeGBFS/trials << endl;
	return;
}

bool loadGraph(string filename)
{
	if( g_pTheGraph )
		delete g_pTheGraph;
	g_pTheGraph = new Graph(createGeneralGraph(filename));
	if( !g_pTheGraph )
		return false;
	return true;
}

int main(int argc, const char* argv[])
{
	if(!loadGraph(string(argv[1])))
		return -1;
	runSearches();	
	return 0;
}