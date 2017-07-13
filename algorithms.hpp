#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iostream>
#include <tuple>

#include "graph.hpp"

using namespace std;

#ifndef ALGOS_HPP
#define ALGOS_HPP

void weight (Graph<Node> & graph);
void component (Graph<Node> & graph, float edgeFilter);
void fuse (Graph<Node> & graph, int sizeFilter);
void lower_fuse (Graph<Node> & graph, int sizeFilter);
void alone (Graph<Node> & graph);
void meta_filter (Graph<Component> & metaGraph, float metaFilter, float metaCeil);

#endif