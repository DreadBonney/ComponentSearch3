#include "io.hpp"


vector<string> split (string & line, char delim);



Graph<Node> load_graph (ifstream & nodes, ifstream & edges) {
	Graph<Node> g;

	//Nodes
	string line;
	getline(nodes, line);
	vector<string> tokens = split(line, ';');
	int spcIdx = -1;
	for (int idx=0 ; idx<tokens.size() ; idx++) {
		transform(tokens[idx].begin(), tokens[idx].end(), tokens[idx].begin(), ::tolower);
		if (tokens[idx] == "specie") {
			spcIdx = idx;
		}
	}

	int i = 0;
	while(getline(nodes, line)) {
		tokens = split(line, ';');
		if (tokens.size() > 0) {
			Node node = Node(i++);
			if (spcIdx == -1) {
				node.specie = "Unknown";
			} else {
				node.specie = tokens[spcIdx];	// Useful for practice runs
			}
			g.nodes.push_back(node);
			g.components.push_back(-1);
		}
	}


	//Edges
	getline(edges, line);
	tokens = split(line, ';');
	int srcIdx = -1, tgtIdx = -1;
	for (int idx=0 ; idx<tokens.size() ; idx++) {
		// To lower case
		transform(tokens[idx].begin(), tokens[idx].end(), tokens[idx].begin(), ::tolower);

		if (tokens[idx] == "source") {
			srcIdx = idx;
		} else if (tokens[idx] == "target") {
			tgtIdx = idx;
		}
	}

	if (srcIdx == -1 || tgtIdx == -1) {
		cerr << "Source or Target label absent from the edges csv file" << endl;
		return g;
	}


	while(getline(edges, line)) {
		tokens = split(line, ';');
		if (tokens.size() > 0) {
			int x = atoi(tokens[srcIdx].c_str());
			int y = atoi(tokens[tgtIdx].c_str());
			g.nodes[x].neighbors.push_back(y);
			g.nodes[y].neighbors.push_back(x);
		}
	}
	return g;
}

vector<string> split (string & line, char delim) {
	vector<string> strs;
	string token;
	stringstream ss;
	ss << line;
	while (ss) {
		getline(ss, token, delim);

		if (ss)
			strs.push_back(token);
	}

	return strs;
}


Graph<Component> load_meta (Graph<Node> & graph) {
	Graph<Component> g;

	int cmpIdx = 0;
	vector<int> translation (graph.nodes.size(), -1);
	for (int i=0; i<graph.nodes.size(); i++) {
		if (translation[graph.components[i]] == -1) {
			translation[graph.components[i]] = cmpIdx;
			g.nodes.push_back(Component(cmpIdx++));
		}
		g.nodes[translation[graph.components[i]]].nodes.push_back(i);

	}


	for (Component & cmp : g.nodes) {
		bool same = true;
		for (int i : cmp.nodes) {
			same = same and graph.getNodeFromIdx(i).specie == graph.getNodeFromIdx(cmp.nodes[0]).specie;
		}
		if (same) {
			cmp.specie = graph.getNodeFromIdx(cmp.nodes[0]).specie;
		} else {
			cmp.specie = "HUB";
		}

	}
	vector< vector<float> > weights (cmpIdx, vector<float> (cmpIdx, 0));
	vector< vector<int> > nbEdges (cmpIdx, vector<int> (cmpIdx, 0));

	for (int i=0; i<graph.nodes.size(); i++) {
		for (int j=0; j<graph.getNodeFromIdx(i).neighbors.size(); j++) {
			int x = graph.components[i];
			int y = graph.components[graph.getNodeFromIdx(i).neighbors[j]];
			if (translation[x] < translation[y]) {
				weights[translation[x]][translation[y]] += graph.getNodeFromIdx(i).weights[j];
				nbEdges[translation[x]][translation[y]]++;
			}
		}
	}

	for (int i=0; i<cmpIdx; i++) {
		for (int j=i+1; j<cmpIdx; j++) {
			if (nbEdges[i][j]) {
				g.nodes[i].neighbors.push_back(j);
				g.nodes[i].weights.push_back(weights[i][j]/nbEdges[i][j]);
				g.nodes[j].neighbors.push_back(i);
				g.nodes[j].weights.push_back(weights[i][j]/nbEdges[i][j]);
			}
		}
	}

	return g;
}


void save_components (Graph<Component> & metaGraph, ifstream & nodes, ofstream & componentStream) {
	
	int n=0;
	for (Component & cmp : metaGraph.nodes) {
		n += cmp.size();
	}

	vector<int> metanode (n, -1);
	vector<int> component (metaGraph.nodes.size(), -1);

	set<int> toSee;
	for (Component & cmp : metaGraph.nodes) {
		for (int i : cmp.nodes) {
			metanode[i] = cmp.idx;
		}
		toSee.insert(cmp.idx);
	}

	int currentComponent = 0;
	while(toSee.size() > 0) {
		int idx = *toSee.begin();
		toSee.erase(idx);

		component[idx] = currentComponent;
		if (metaGraph.nodes[idx].neighbors.size() < 3) {
			set<int> nexts;
			for (int i : metaGraph.nodes[idx].neighbors) {
				if (metaGraph.nodes[i].neighbors.size() < 3) {
					nexts.insert(i);
				}
			}
			while (nexts.size() > 0) {
				int i = *nexts.begin();
				nexts.erase(i);
				toSee.erase(i);
				component[i] = currentComponent;
				for (int j : metaGraph.nodes[i].neighbors) {
					if (metaGraph.nodes[j].neighbors.size() < 3 and toSee.count(j)) {
						nexts.insert(j);
					}
				}
			}
		}
		currentComponent++;
	}

	string line;
	getline(nodes, line);
	componentStream << line << ";metanode;component" << endl;

	int i=0;
	while(getline(nodes, line)) {
		if (line.size() > 0) {
			componentStream << line << ";" << metanode[i] << ";" << component[metanode[i]] << endl;
			i++;
		}
	}

	vector<int> size (currentComponent, 0);
	for (int i=0; i<n; i++) {
		size[component[metanode[i]]]++;
	}
	int c=0;
	int t=0;
	for (int s : size) {
		t++;
		if (s > 10)
			c++;
	}
}


void save_meta (Graph<Component> & metaGraph, ofstream & nodes, ofstream & edges) {

	int n=0;
	for (Component & cmp : metaGraph.nodes) {
		n += cmp.size();
	}

	vector<int> metanode (n, -1);
	vector<int> component (metaGraph.nodes.size(), -1);

	set<int> toSee;
	for (Component & cmp : metaGraph.nodes) {
		for (int i : cmp.nodes) {
			metanode[i] = cmp.idx;
		}
		toSee.insert(cmp.idx);
	}

	int currentComponent = 0;
	while(toSee.size() > 0) {
		int idx = *toSee.begin();
		toSee.erase(idx);

		component[idx] = currentComponent;
		if (metaGraph.nodes[idx].neighbors.size() < 3) {
			set<int> nexts;
			nexts.insert(idx);
			while (nexts.size() > 0) {
				int i = *nexts.begin();
				nexts.erase(i);
				toSee.erase(i);
				component[i] = currentComponent;
				for (int j : metaGraph.nodes[i].neighbors) {
					if (metaGraph.nodes[j].neighbors.size() < 3 and component[j] == -1) {
						nexts.insert(j);
					}
				}
			}
		}
		currentComponent++;
	}

	nodes << "Node;Size;Specie;Component" << endl;
	edges << "Source;Target;Type;Weight" << endl;

	for (Component & cmp : metaGraph.nodes) {
		nodes << cmp.idx << ";" << cmp.nodes.size() << ";" << cmp.specie << ";" << component[cmp.idx] << endl;

		for (int i=0; i<cmp.neighbors.size(); i++) {
			if (cmp.idx < cmp.neighbors[i])
				edges << cmp.idx << ";" << cmp.neighbors[i] << ";Undirected;" << cmp.weights[i] << endl;
		}
	}
}