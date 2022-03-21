#include <iostream>
#include <vector>

using namespace std;

typedef struct Graph {
	vector<int> offset;	// Records of first neighbor of vertices
	vector<int> data;	// The concatenation of adjacent vectors
} graph;

class Condensation {
private:
	graph* org_G = new graph();	// The original graph
	graph* rev_G = new graph();	// The transpose graph	
	
	vector<bool> visited;		// Record the vertices which are already visited in DFS 
	vector<int> exit_time;		// Record the exit time of DFS
	vector<int> component;		// Store the current strongly connected component
public:
	Condensation () {
		org_G->offset.push_back(0);	// Add one more offset that makes the code simpler
	}

	~Condensation () {
		delete org_G;
		delete rev_G;
	}
	
	void add_node () {

		org_G->offset.push_back(org_G->data.size());	// Points to the end of data

		return;
	}

	void add_edge (int from, int to) {

		org_G->data.insert(org_G->data.begin() + org_G->offset[from], to);

		// Update the offsets
		for (int i = from + 1; i < org_G->offset.size(); i ++)
			org_G->offset[i] ++;

		return;			
	}

	graph* get_graph () {
		
		return org_G; 	
	}

	void log_graph () {

		cout << "===== Original Graph =====" << endl;

		for (int i = 0; i < org_G->offset.size() - 1; i ++) {
			cout << i << " :";
			
			// Print its neighbors of vertex i
			for (int j = org_G->offset[i]; j < org_G->offset[i+1]; j ++)
				cout << " " << org_G->data[j];
			cout << endl;
		}

		return;
	}

	void graph_transpose () {

		// Make sure that rev_G has nothing
		rev_G->offset.clear();
		rev_G->data.clear();

		for (int i = 0; i < org_G->offset.size(); i ++)
			rev_G->offset.push_back(0);

		for (int i = 0; i < org_G->offset.size() - 1; i ++) {
			for (int j = org_G->offset[i]; j < org_G->offset[i + 1]; j ++) {
				
				// Reverse the direction of the edge
				int from = org_G->data[j], to = i;	
				rev_G->data.insert(rev_G->data.begin() + rev_G->offset[from], to);

				// Update the offsets
				for (int k = from + 1; k < rev_G->offset.size(); k ++)
					rev_G->offset[k] ++;
			}
		}
		
		return;
	}

	void log_graph_transpose () {

		cout << "===== Transpose Graph =====" << endl;

		for (int i = 0; i < rev_G->offset.size() - 1; i ++) {
			cout << i << " :";

			// Print the neighbors of vertex i
			for (int j = rev_G->offset[i]; j < rev_G->offset[i + 1]; j ++)
				cout << " " << rev_G->data[j];
			cout << endl;
		}

		return;
	}

	void dfs (int v) {

		visited[v] = true;

		for (int i = org_G->offset[v]; i < org_G->offset[v + 1]; i ++)
			if (!visited[org_G->data[i]])
				dfs(org_G->data[i]);

		exit_time.push_back(v);
		
		return;
	}

	void rev_dfs (int v) {

		visited[v] = true;
		component.push_back(v);

		for (int i = rev_G->offset[v]; i < rev_G->offset[v + 1]; i ++)
			if (!visited[rev_G->data[i]])
				rev_dfs(rev_G->data[i]);

		return;
	}

	vector<int> get_condensation_mapping() {

		graph_transpose();

		int num_of_node = org_G->offset.size() - 1;
		visited.assign(num_of_node, false);

		for (int i = 0; i < num_of_node; i ++)
			if (!visited[i])
				dfs(i);

		reverse(exit_time.begin(), exit_time.end());	// Order vertices w.r.t. exit time (from large to small) 		

		visited.assign(num_of_node, false);
		vector<int> mapping(num_of_node);
		int cur_mapping = 0;

		for (int i = 0; i < num_of_node ;i ++)
			if (!visited[exit_time[i]]) {
				cur_mapping ++;		// Next strongly connected component
				rev_dfs(exit_time[i]);	// Perform DFS on transpose graph to get the SCC of the current vertex
				for (int j = 0; j < component.size(); j ++)
					mapping[component[j]] = cur_mapping;
				component.clear();
			}

		return mapping;
	}
};

void Test (int num_of_vertices, vector<vector<int>>& edges, int debug = 1) {
	
	Condensation* tmp = new Condensation();

	// Construct the graph
	for (int i = 0; i < num_of_vertices; i ++)
		tmp->add_node();
	for (auto& e : edges)
		tmp->add_edge(e[0], e[1]);

	// Print the graphs, just for debugging
	if (debug) {
		tmp->log_graph();
		tmp->graph_transpose();
		tmp->log_graph_transpose();
	}
	
		
	// Output final graph structure
	graph* G = tmp->get_graph();
	cout << "===== Graph Structure ====" << endl;
	cout << "Offset :";
	for (auto& offset : G->offset)
		cout << " " << offset;
	cout << endl;
	cout << "Data :";
	for (auto& data : G->data)
		cout << " " << data;
	cout << endl;

	// Get the SCC id for each vertex
	vector<int> mapping = tmp->get_condensation_mapping();
	cout << "===== Mapping =====" << endl;
	for (int i = 0; i < num_of_vertices; i ++)
		cout << mapping[i] << ' ';
	cout << endl;

	delete tmp;

	return;
}

int main() {

	/*
	Define Graph:
		Just change the value of num_of_vertices and the elements in edges
	Compile:
		g++ -std=c++11 SCC.cpp
	Run:
		./a.out	
	*/

	int debug = 1;

	// Example 1 from Wikipedia

	int Ex1_num_of_vertices = 8;
	vector<vector<int>> Ex1_edges = {
		{0, 1},
		{1, 2}, {1, 4}, {1, 5},
		{2, 3}, {2, 6},
		{3, 2}, {3, 7},
		{4, 0}, {4, 5},
		{5, 6},
		{6, 5},
		{7, 3}, {7, 6}
	};

	Test (Ex1_num_of_vertices, Ex1_edges);

	// Example 2 from Wikipedia

	int Ex2_num_of_vertices = 16;
	vector<vector<int>> Ex2_edges = {
		{0, 2},
		{1, 0}, {1, 5},
		{2, 1}, {2, 4},
		{3, 2}, {3, 9},
		{4, 3}, {4, 5}, {4, 10},
	
		{5, 6}, {5, 8}, {5, 13},

		{6, 7},
		{7, 8}, {7, 15},
		{8, 6}, {8, 15},

		{9, 10},
		{10, 11}, {10, 12},
		{11, 9},
		{12, 11}, {12, 13},

		{13, 15}, {13, 14},
		{14, 13},
	};

	Test (Ex2_num_of_vertices, Ex2_edges);

	return 0;
}


