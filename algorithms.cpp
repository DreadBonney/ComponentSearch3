#include "algorithms.hpp"

// Computes the weight of an edge from the lists of neighbors
float intersectSize (vector<int> & v1, vector<int> & v2, long & total);

// Insert e in v, sorting according to the first component
void insert (vector< tuple<float, int, int> > & v, tuple<float, int, int> e);

// Checks if there is a non-direct path from x to y
bool still_connex (Graph<Component> & metaGraph, int x, int y);

// Deletes an edge from the meta graph
void remove (Graph<Component> & metaGraph, int x, int y);


// Weights the graph
void weight (Graph<Node> & graph) {

	int c=0;
	long total=0;
	for (Node & node : graph.nodes) {
		for (int i : node.neighbors) {
			c++;
			float w = intersectSize(node.neighbors, graph.getNodeFromIdx(i).neighbors, total);
			node.weights.push_back(w);
		}
	}
}


// Computes the components by doing a BFS where edges below edgeFilter are omitted
void component (Graph<Node> & graph, float edgeFilter) {


	set<int> toAnnotate;
	for (int i=0 ; i<graph.nodes.size() ; i++)
		toAnnotate.insert(i);

	int currentColor = 0;

	while (toAnnotate.size() > 0) {
		int startIdx = *toAnnotate.begin();
		graph.componentSize.push_back(0);

		set<int> nextIdxs;
		nextIdxs.insert(startIdx);

		while (nextIdxs.size() > 0) {
			int nodeIdx = *nextIdxs.begin();
			nextIdxs.erase(nodeIdx);
			if (graph.components[nodeIdx] == -1) {
				graph.components[nodeIdx] = currentColor;
				graph.componentSize[currentColor]++;
				toAnnotate.erase(nodeIdx);
				Node & node = graph.getNodeFromIdx(nodeIdx);
				for (int i=0; i<node.neighbors.size(); i++) {
					if (node.weights[i] > edgeFilter)
						nextIdxs.insert(node.neighbors[i]);
				}
			}
		}
		currentColor++;
	}
}


// Fuse nodes from a small component to (if applicable) the only neighbor
void fuse (Graph<Node> & graph, int sizeFilter) {
	for (Node & node : graph.nodes) {
		set<int> adjacent;
		for (int i : node.neighbors) {
			if (graph.componentSize[graph.components[i]] > sizeFilter) {
				// If node is not in a small component, adjacent will contain its component
				adjacent.insert(graph.components[i]);
			}
		}
		if (adjacent.size() == 1) {
			graph.componentSize[graph.components[node.idx]]--;
			graph.components[node.idx] = *adjacent.begin();
			graph.componentSize[*adjacent.begin()]++;

		}
	}
}


// Join an element by himself in his component with its closest neighbor
void alone (Graph<Node> & graph) {
	for (Node & node : graph.nodes) {
		if (graph.componentSize[graph.components[node.idx]] == 1) {
			if (node.neighbors.size() > 0) {
				int closest = 0;
				for (int j=0; j<node.neighbors.size(); j++) {
					if (node.weights[j] > node.weights[closest]) {
						closest = j;
					}
				}
				closest = node.neighbors[closest];
				graph.componentSize[graph.components[node.idx]]--;
				graph.components[node.idx] = graph.components[closest];
				graph.componentSize[graph.components[closest]]++;
			}
		}
	}
}


// Fuse small components together
void lower_fuse (Graph<Node> & graph, int sizeFilter) {
	for (int i=0; i<graph.nodes.size(); i++) {
		if (graph.componentSize[graph.components[i]] < sizeFilter) {
			set<int> neighbors;
			neighbors.insert(i);
			set<int> seen;
			while (neighbors.size() > 0) {
				int id = *neighbors.begin();
				neighbors.erase(id);
				if (seen.count(id)==0) {
					seen.insert(id);
					graph.componentSize[graph.components[id]]--;
					graph.components[id] = graph.components[i];
					graph.componentSize[graph.components[i]]++;
					for (int j : graph.getNodeFromIdx(id).neighbors) {
						if (graph.componentSize[graph.components[j]] < sizeFilter and seen.count(j) == 0) {
							neighbors.insert(j);
						}
					}
				}
			}
		}
	}
}



// Remove all edges below metaFilter and all edges below metaCeil that preserve the connexity
void meta_filter (Graph<Component> & metaGraph, float metaFilter, float metaCeil) {
	vector< tuple<float, int, int> > toRemove;
	for (Component & cmp : metaGraph.nodes) {
		for (int i=0; i<cmp.neighbors.size(); i++) {
			if (cmp.weights[i] < metaFilter and cmp.idx < cmp.neighbors[i]) {
				insert(toRemove, make_tuple(cmp.weights[i], cmp.idx, cmp.neighbors[i]));
			}
		}
	}

	for (tuple<float, int, int> t : toRemove) {
		float w = get<0>(t); int x = get<1>(t); int y = get<2>(t);
		if (still_connex(metaGraph, x, y) or w < metaCeil) {
			remove(metaGraph, x, y);
		}
	}
}


float intersectSize (vector<int> & v1, vector<int> & v2, long & total) {
	int i, j = 0;
	float c = 0;
	while( i<v1.size() and j<v2.size() ) {
		if (v1[i] < v2[j]) {
			i++;
		} else if (v2[j] < v1[i]) {
			j++;
		} else {
			i++; j++; c++;
		}
		total++;
	}
	return 2*c/(v1.size()+v2.size());
}

void insert (vector< tuple<float, int, int> > & v, tuple<float, int, int> e) {
	vector< tuple<float, int, int> >::iterator it = v.begin();
	float w = get<0>(e);
	while (it < v.end() and w > get<0>(*it)) {
		it++;
	}
	v.insert(it, e);
}



bool still_connex (Graph<Component> & metaGraph, int x, int y) {
	set<int> toSee;
	vector<int> seen (metaGraph.nodes.size(), 0);
	seen[x]++;
	for (int i : metaGraph.nodes[x].neighbors) {
		if (i != y) {
			toSee.insert(i);
		}
	}

	while (toSee.size() > 0) {
		int id = *toSee.begin();
		toSee.erase(id);
		seen[id]++;
		if (id == y) {
			return true;
		}
		for (int i : metaGraph.nodes[id].neighbors) {
			if (not(seen[i])) {
				toSee.insert(i);
			}
		}
	}
	return false;
}

void remove (Graph<Component> & metaGraph, int x, int y) {
	vector<int> new_neighbors;
	vector<float> new_weights;
	for (int i=0; i<metaGraph.nodes[x].neighbors.size(); i++) {
		if (metaGraph.nodes[x].neighbors[i] != y) {
			new_neighbors.push_back(metaGraph.nodes[x].neighbors[i]);
			new_weights.push_back(metaGraph.nodes[x].weights[i]);
		}
	}
	metaGraph.nodes[x].neighbors = new_neighbors;
	metaGraph.nodes[x].weights = new_weights;

	new_neighbors.clear();
	new_weights.clear();

	for (int i=0; i<metaGraph.nodes[y].neighbors.size(); i++) {
		if (metaGraph.nodes[y].neighbors[i] != x) {
			new_neighbors.push_back(metaGraph.nodes[y].neighbors[i]);
			new_weights.push_back(metaGraph.nodes[y].weights[i]);
		}
	}
	metaGraph.nodes[y].neighbors = new_neighbors;
	metaGraph.nodes[y].weights = new_weights;
}