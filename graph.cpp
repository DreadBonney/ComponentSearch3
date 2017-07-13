#include "graph.hpp"




Node::Node () {

};

Node::Node (int idx) {
	this->idx = idx;
}

void Node::print() {
	cout << idx << endl;
}

bool Node::operator==(const Node &other) const {
	return this->idx == other.idx;
}

Component::Component () {

};

Component::Component (int idx) {
	this->idx = idx;
}

void Component::print() {
	cout << idx << endl;
}

int Component::size() {
	return this->nodes.size();
}