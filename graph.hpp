#include <vector>
#include <iostream>
#include <string>


using namespace std;


#ifndef NODE_HPP
#define NODE_HPP

class Node {
public:
	int idx;
	vector<int> neighbors;
	vector<float> weights;
	string specie;

	Node();
	Node(int);

	void print ();
	bool operator==(const Node & other) const;
};

class Component {
public:
	int idx;
	vector<int> nodes;
	string specie;

	vector<int> neighbors;
	vector<float> weights;

	Component();
	Component(int);

	void print ();
	int size();
};

template <typename N>
class Graph {
public:
	vector<N> nodes;
	vector<int> components;
	vector<int> componentSize;

	Graph();

	N & getNodeFromIdx(int idx);
	long getEdgesNb();

	void print ();

};

template <typename N>
Graph<N>::Graph() {}

template <typename N>
void Graph<N>::print() {
	for (N node : nodes)
		node.print();
}

template <typename N>
N & Graph<N>::getNodeFromIdx (int idx) {
	if (nodes[idx].idx == idx)
		return nodes[idx];
	return *(find(nodes.begin(), nodes.end(), Node(idx)));
}


template <typename N>
long Graph<N>::getEdgesNb() {
	long edgesNb = 0;
	for (N node: nodes)
		edgesNb += node.neighbors.size();
	return edgesNb / 2;
}


#endif
