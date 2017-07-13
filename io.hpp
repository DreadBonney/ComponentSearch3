#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <set>


#include "graph.hpp"


using namespace std;


#ifndef IO_HPP
#define IO_HPP

// Loads a graph from nodes and vertices
Graph<Node> load_graph (ifstream & verticies, ifstream & edges);

// Loads a meta graph from the associated graph
Graph<Component> load_meta (Graph<Node> & graph);

// Writes the components.csv file
void save_components (Graph<Component> & metaGraph, ifstream & nodes, ofstream & componentStream);

// Writes the metaEdges.csv and metaNodes.csv files
void save_meta (Graph<Component> & metaGraph, ofstream & nodes, ofstream & edges);

#endif