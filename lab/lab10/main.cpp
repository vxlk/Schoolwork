#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include<algorithm>
using namespace std;

map<string, int> id;
map<string, vector<string>> Neighbors;
map<string, int> dists;
queue<string> Q;
map<string, int> bestDistances;

int bfs() {
	int i = 0;
	while (!Q.empty()) {
		string to_visit = Q.front();
		Q.pop();
		//for (int i=0; i<Neighbors[to_visit].size(); i++) {
		for (auto &n : Neighbors[to_visit]) {
			if (dists[n] == 0) {   // is nbr unvisited so far...?
				Q.push(n);
				dists[n] = dists[to_visit] + 1;
			}
		}
	}
	int farthest = 0;
	for (map<string, vector<string>>::iterator i = Neighbors.begin(); i != Neighbors.end(); i++) {
		int now = dists[i->first];
		if (now > farthest)
			farthest = dists[i->first];
	}
	return farthest;
}

void printPath() {
	vector<pair<int, string> > rev;
	for (map<string, int>::iterator it = bestDistances.begin(); it != bestDistances.end(); it++) {
		pair<int, string> temp;
		temp.first = it->second;
		temp.second = it->first;
		rev.push_back(temp);
	}
		
	//copy into reversed map so we can sort
	sort(rev.begin(), rev.end());
	for (vector<pair<int, string> >::iterator it = rev.begin(); it != rev.end(); it++)
		cout << it->first << " " << it->second << "\n";
}

int main(void) {

	ifstream in;
	in.open("usa_48_state_graph.txt");

	string lastNode;
	string node;
	string neighbor;
	int n = 0;

	while (in >> node >> neighbor) {
		Neighbors[node].push_back(neighbor);
		dists[node] = 0;
		Neighbors[neighbor].push_back(node);
		dists[neighbor] = 0;
	}

	//loop over every state
	for (map<string, vector<string>>::iterator it = Neighbors.begin(); it != Neighbors.end(); it++) {
		Q.push(it->first);
		//reset roots
		dists[it->first] = 0;
		for (map<string, vector<string>>::iterator its = Neighbors.begin(); its != Neighbors.end(); its++)
			dists[its->first] = 0;
		//search to fill distance
		bestDistances[it->first] = bfs();
	}

	printPath();
	return 0;
}
