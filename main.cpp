#include <iostream>
#include "graph.hpp"
#include "io.hpp"
#include "algorithms.hpp"

using namespace std;



void help(string n, string e, string b, float E, float N, float C, int S) {
	cout << endl << "Options:" << endl;
	cout << endl << "I/O options" << endl;
	cout << " -n : nodes file name (default \"" << n << "\")" << endl;
	cout << " -e : edges file name (default \"" << e << "\")" << endl;
	cout << " -b : output basename (default \"" << b << "\")" << endl;
	cout << endl << "Filtering values" << endl;
	cout << " -N : metaFilter value (default "<< N << ")" << endl;
	cout << " -E : edgeFilter value (default "<< E << ")" << endl;
	cout << " -C : metaCeil value (default "<< C << ")" << endl;
	cout << " -S : sizeFilter value (default "<< S << ")" << endl;

	cout << endl << endl;
}



int main (int argc, char * argv[]) {

	// --- Default parameters ---

	string nodesFilename = "nodes.csv"; 			// Name of the nodes file
	string edgesFilename = "edges.csv";				// Name of the edges file
	string basename = "";							// Standard output name
	float edgeFilter = 0.8;							// Ceil defining the similarity between nodes
	float metaFilter = 0.5;							// Ceil defining the interest of a meta edge
	float metaCeil = 0.2;							// Ceil defining the error limit of a meta edge
	int sizeFilter = 20;							// Ceil defining a small component


	for (int idx=1 ; idx<argc ; idx++) {
		if (argv[idx][0] == '-') {
			switch (argv[idx][1]) {
			case 'n':
				nodesFilename = string(argv[++idx]);
				break;
			case 'e':
				edgesFilename = string(argv[++idx]);
				break;
			case 'b':
				basename = string(argv[++idx]);
				break;
			case 'N':
				metaFilter = atof(argv[++idx]);
				break;
			case 'E':
				edgeFilter = atof(argv[++idx]);
				break;
			case 'S':
				sizeFilter = atoi(argv[++idx]);
				break;
			case 'C':
				metaCeil = atof(argv[++idx]);
				break;
			case 'h':
				help(nodesFilename, edgesFilename, basename, edgeFilter, metaFilter, metaCeil, sizeFilter);
				return 0;

			default:
				break;
			}
		}
	}



	// --- Loading ---

	cout << endl << "--- Loading ---" << endl;

	ifstream nodesFile (nodesFilename.c_str());
	if (!nodesFile) {
		cerr << "The file " << nodesFilename << " seems to be missing." << endl;
		return 1;
	}

	ifstream edgesFile (edgesFilename.c_str());
	if (!edgesFile) {
		cerr << "The file " << edgesFilename << " seems to be missing." << endl;
		return 1;
	}

	Graph<Node> graph = load_graph(nodesFile, edgesFile);
	cout << graph.nodes.size() << " nodes loaded" << endl;
	cout << graph.getEdgesNb() << " edges loaded" << endl;



	// --- Algorithms ---

	cout << endl << "--- Running Algorithms ---" << endl;


	cout << "-> Creating meta-graph..." << endl;


	cout << " -> Calculating weights..." << endl;
	weight(graph);


	cout << " -> Filtering lowest edges..." << endl;
	component(graph, edgeFilter);


	cout << " -> Fusing smallest components..." << endl;
	fuse(graph, sizeFilter);						// Small components fusing with their unique neighbor
	lower_fuse(graph, sizeFilter);					// Small components fusing together
	alone(graph);									// Singular components fusing with closest neighbor

	
	cout << " -> Filtering meta-edges..." << endl;
	Graph<Component> metaGraph = load_meta(graph);
	meta_filter(metaGraph, metaFilter, metaCeil);	// Removes lowest meta-edges
	cout << metaGraph.nodes.size() << " meta-nodes" << endl;
	cout << metaGraph.getEdgesNb() << " meta-edges" << endl;



	// --- Output ---
	cout << endl << "--- Saving Results ---" << endl;
	cout << " -> Saving components..." << endl;
	stringstream ss;
	ss << basename << ".components.csv";
	ofstream outStream (ss.str());
	ifstream inStream (nodesFilename);
	save_components(metaGraph, inStream, outStream);


	cout << " -> Saving meta-graph..." << endl;
	stringstream nodes;
	stringstream edges;
	nodes << basename << ".metaNodes.csv";
	edges << basename << ".metaEdges.csv";
	ofstream nodesStream (nodes.str());
	ofstream edgesStream (edges.str());
	save_meta(metaGraph, nodesStream, edgesStream);

}