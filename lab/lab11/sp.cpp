#include <cstdio>
#include <cstdlib>
#include <queue>
#include <algorithm>
#include <vector>
#include <set>
#include <iostream>
#include <limits>
using namespace std;

float INF = numeric_limits<float>::infinity();

//contains the index to the block of common edges
vector<int> edgeIndex;

struct Edge {
  int src, dst;
  float travel_time;
};

int N, M;
Edge *E;

//min pq
priority_queue<pair<float, int>,vector<pair<float, int> >,greater<pair<float, int> > > Q;

float dijkstra(int src) {
	int n = 0;
	
	//source
	Q.push(make_pair(0, src));

	vector<float> data(N, INF);
	//set source
	data[src] = 0;
	
	while(!Q.empty()) {
		int node = Q.top().second;
		Q.pop();
		//check neighbors
		for(int f = edgeIndex[node]; f < edgeIndex[node + 1]; f++) {
			if (data[node] + E[f].travel_time < data[E[f].dst]) {
				data[E[f].dst] = data[node] + E[f].travel_time;
				Q.push(make_pair(data[E[f].dst], E[f].dst)); //edge index
			}
		}
	}

	float sum = 0.0;
	for(int i = 0; i < N; i++)
		if (data[i] > sum && data[i] != INF) {
			sum = data[i];
		}

	return sum; //distance
}

void getNodeArray() {
	int currentIndex = -1;
	for(int i = 0; i < M; i++) { //read every edge
		if (E[i].src != currentIndex) {
			currentIndex = E[i].src;
			edgeIndex.push_back(i);
		}
	}
}

// compare two edges based on their source nodes
// (break ties using destination nodes)
bool comp(Edge e1, Edge e2)
{
  if (e1.src == e2.src) return e1.dst < e2.dst;
  return e1.src < e2.src;
}

int main(void)
{
  FILE *fp = popen ("zcat /tmp/usa_network.txt.gz", "r");
  fscanf (fp, "%d %d", &N, &M);
  E = new Edge[M];
  printf ("Reading input");
  for (int i=0; i<M; i++) {
    fscanf (fp, "%d %d %f", &E[i].src, &E[i].dst, &E[i].travel_time);
    if (i % 1000000 == 0) { printf ("."); fflush(stdout); }
  }
  fclose(fp);
  printf (" Done.\n" ); fflush(stdout);

  sort(E, E+M, comp);
  getNodeArray();

  int start = 17731931;

  //find x
  float x = dijkstra(start);
  
  //rebuild swapped
  for (int i = 0; i < M; i++)
	  swap(E[i].src, E[i].dst);
  sort(E, E+M, comp);
  edgeIndex.clear();
  getNodeArray();

  //find y
  float y = dijkstra(start);

  float sum = (x + y) / 2.0;
  sum = sum / 3600.0;
  cout << "\n" << sum;
  cout << "\n" << (x + y) / 3600.0;
  // The node right in front of McAdams Hall is number 17731931.
 // /3600
  return 0;
}
