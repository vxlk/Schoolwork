#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;

typedef pair<double, double> Point;

vector<pair<string, Point > > restaurants;
vector<pair<string, Point > > landmarks;
				
double distanceFormula(Point p1, Point p2) {
	const double x_diff = p1.first - p2.first;
	const double y_diff = p1.second - p2.second;
	return sqrt(x_diff * x_diff + y_diff * y_diff);
}

#define INF 99999.9
void sp3() {
	
	//j - 1 is how  you access in the restaurant array
	int L = landmarks.size();
	int R = restaurants.size() + 1;

	double **dist;
	dist = new double *[L];
	for (int r = 0; r<L; r++) {
		dist[r] = new double[R];
		for (int c = 0; c < R; c++) {
			dist[r][c] = INF;
		}
	}

	//landmark base case
	for(int u = 1; u < L; u++)
		dist[u][0] = distanceFormula(landmarks[u].second, landmarks[u - 1].second);

	
	//for(int i = 0; i < R; i++)
	int **next;
	next = new int *[L];
	for (int r = 0; r < L; r++) {
		next[r] = new int[R];
		for (int c = 0; c < R; c++)
			next[r][c] = -1;
	}

	//init start
	dist[0][0] = 0;
	for (int i = 1; i < L; i++) {
		for (int j = 1; j < R; j++) {
			//go thru restaurant
			if (distanceFormula(landmarks[i - 1].second, restaurants[j-1].second) + 
				distanceFormula(restaurants[j-1].second, landmarks[i].second) + dist[i-1][j-1] < dist[i][j])
			{																	//dummy rest node
				dist[i][j] = distanceFormula(landmarks[i - 1].second, restaurants[j-1].second) 
							 + distanceFormula(restaurants[j-1].second, landmarks[i].second) + dist[i - 1][j - 1];
				next[i][j] = j - 1;
			}
			if (distanceFormula(landmarks[i-1].second, landmarks[i].second) + dist[i-1][j] < dist[i][j])  
			{
				dist[i][j] = distanceFormula(landmarks[i-1].second, landmarks[i].second) + dist[i-1][j];
				next[i][j] = -1; //-1 means no restaurant in between
			}
		}
	}

	//init pathfinding params
	int currRest = -1;
	int row = R-1;
	
	//print distance
	cout << "Total tour distance: " <<dist[L - 1][R - 1] << "\n";

	vector<pair<string, Point> > revPath;	
	for (int col = L-1; col>=0; col--) {
		//add landmark to path
		 revPath.push_back(make_pair(landmarks[col].first, landmarks[col].second));
		 //check for restaurant
		 if (next[col][row] != currRest && next[col][row] != -1) {
			 currRest = next[col][row];
			 row = currRest;
			 revPath.push_back(make_pair(restaurants[currRest].first, restaurants[currRest].second));
		 }
	}
	//reverse the path, so it starts at the beginning
	 reverse(revPath.begin(), revPath.end());
	 for(int i = 0; i < revPath.size(); i++)
	 	cout << revPath[i].first << " " << revPath[i].second.first << " " << revPath[i].second.second << "\n";

}

int main(void) {

	ifstream in1;
	ifstream in2;
	in1.open("landmarks.txt");
	in2.open("restaurants.txt");

	pair<string, Point> temp;
	while (in1 >> temp.first >> temp.second.first >> temp.second.second)
		landmarks.push_back(temp);
	while (in2 >> temp.first >> temp.second.first >> temp.second.second)
		restaurants.push_back(temp);

	//sorted alphabetically
	sort(landmarks.begin(), landmarks.end());
	sort(restaurants.begin(), restaurants.end());

	sp3();

	return 0;
}
