#include <cstdio>
#include <cstdlib>
#include <queue>
#include <algorithm>
using namespace std;

struct Edge {
  int src, dst;
  float travel_time;
};

int N, M;
Edge *E;

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

  // The node right in front of McAdams Hall is number 17731931.
  
  return 0;
}
