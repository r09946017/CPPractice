#include <iostream>
#include <vector>

using namespace std;

typedef struct Graph {
	vector<int> offset;	// Records of first neighbor of vertices
	vector<int> data;	// The concatenation of adjacent vectors
} graph;

class Condensation {
private:
	graph* G = new graph();		// The original graph
	graph* rev_G = new graph();	// The transpose graph	
	
	vector<bool> visited;		// Record the vertices which are already visited in DFS 
	vector<int> exit_time;		// Record the exit time of DFS
	vector<int> component;		// Store the current strongly connected component
public:
	Condensation () {
		G->offset.push_back(0);	// Add one more offset that makes the code simpler
	}

	~Condensation () {
	}
	
	void add_node () {

		G->offset.push_back(G->data.size());	// Points to the end of data

		return;
	}

	void add_edge (int from, int to) {

		G->data.insert(G->data.begin() + G->offset[from], to);

		// Update the offsets
		for (int i = from + 1; i < G->offset.size(); i ++)
			G->offset[i] ++;

		return;			
	}

	void log_graph () {

		cout << "===== Original Graph =====" << endl;

		for (int i = 0; i < G->offset.size() - 1; i ++) {
			cout << i << " :";
			
			// Print its neighbors of vertex i
			for (int j = G->offset[i]; j < G->offset[i+1]; j ++)
				cout << " " << G->data[j];
			cout << endl;
		}

		return;
	}

	void graph_transpose () {

		// Make sure that rev_G has nothing
		rev_G->offset.clear();
		rev_G->data.clear();

		for (int i = 0; i < G->offset.size(); i ++)
			rev_G->offset.push_back(0);

		for (int i = 0; i < G->offset.size() - 1; i ++) {
			for (int j = G->offset[i]; j < G->offset[i + 1]; j ++) {
				
				// Reverse the direction of the edge
				int from = G->data[j], to = i;	
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

		for (int i = G->offset[v]; i < G->offset[v + 1]; i ++)
			if (!visited[G->data[i]])
				dfs(G->data[i]);

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

	vector<int> condensation() {

		int num_of_node = G->offset.size() - 1;
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
				cur_mapping ++;	// Next strongly connected component
				rev_dfs(exit_time[i]);
				for (int j = 0; j < component.size(); j ++)
					mapping[component[j]] = cur_mapping;
				component.clear();
			}

		return mapping;
	}
};


int main() {

	/*
	Define Graph:
		Just change the value of num_of_vertices and the elements in edges
	Compile:
		g++ -std=c++11 SCC.cpp
	Run:
		./a.out	
	*/

	int num_of_vertices = 8;
	vector<vector<int>> edges = {
		{1, 2}, {1, 3}, {1, 4},
		{2, 3}, {2, 4},
		{3, 4},
		{4, 5}, {4, 6},
		{6, 7},
		{7, 4}
	};

	Condensation* tmp = new Condensation();

	// Construct the graph
	for (int i = 0; i < num_of_vertices; i ++)
		tmp->add_node();
	for (auto& e : edges)
		tmp->add_edge(e[0], e[1]);

	// Print the graphs, just for debugging
	tmp->log_graph();
	tmp->graph_transpose();
	tmp->log_graph_transpose();

	// Get the SCC id for each vertex
	vector<int> mapping = tmp->condensation();
	cout << "===== Mapping =====" << endl;
	for (int i = 0; i < num_of_vertices; i ++)
		cout << mapping[i] << ' ';
	cout << endl;

	return 0;
}


