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
//vector<pair<string, int> > path; //contains a pair of pairs, @string - either a landmark or restaurant, @int the index in that array
				
double distanceFormula(Point p1, Point p2) {
	const double x_diff = p1.first - p2.first;
	const double y_diff = p1.second - p2.second;
	return sqrt(x_diff * x_diff + y_diff * y_diff);
}

void print_path(int* path) {
	for (int i = 0; i < landmarks.size(); i++) {
		//print the landmark
		cout << landmarks[i].first << " "; //print the name
		cout << landmarks[i].second.first << " " << landmarks[i].second.second << "\n"; //print coords
		if (path[i] != -1) { //look in the restaurant array
			int index = path[i];
			cout << restaurants[index].first << " "; //print the name
			cout << restaurants[index].second.first << " " << restaurants[index].second.second << "\n"; //print coords
		}
	}
		
}

#define INF 99999.9
void sp3() {
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

//	//rest base case
//	for (int i = 1; i < R; i++)
//		dist[0][i] = INF;
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

	/*for (int i = 0; i < landmarks.size(); i++) {
		cout << "\n";
		for (int u = 0; u < restaurants.size(); u++)
			cout << dist[i][u] << " ";
	}
	*/
	//j - 1 is how  you access in the rest array
	pair<int,int> tour[L];
	dist[0][0] = 0;
	next[0][0] = -1;
	tour[0] = make_pair(0,0);
	for (int i = 1; i < L; i++) {
		for (int j = 1; j < R; j++) {
			//go thru restaurant
			if (distanceFormula(landmarks[i - 1].second, restaurants[j-1].second) + 
				distanceFormula(restaurants[j-1].second, landmarks[i].second) + dist[i-1][j-1] < dist[i][j])
			{																	//dummy rest node
				dist[i][j] = distanceFormula(landmarks[i - 1].second, restaurants[j-1].second) + distanceFormula(restaurants[j-1].second, landmarks[i].second)
 + dist[i - 1][j - 1];
					//landpath[i] = j - 1;
				next[i][j] = j - 1; //next array needs to know before you go whether you are going to a restaurant
				//tour[i].first = i;
				//tour[i].second = j;
			}
			if (distanceFormula(landmarks[i-1].second, landmarks[i].second) + dist[i-1][j] < dist[i][j])  
			{
				dist[i][j] = distanceFormula(landmarks[i-1].second, landmarks[i].second) + dist[i-1][j];
				next[i][j] = -1;
				//tour[i].first = i;
				//tour[i].second = j;
			}
		}
	}
	
//	cout.precision (2);
/*
	for (int i = 0; i < L; i++) {
		cout << "\n";
		for (int u = 0; u < R; u++)
			cout << dist[i][u] << " ";
	}
	cout <<"\n";
	*/ for (int i = 0; i < L; i++) {
		 cout << "\n";
		 for (int u = 0; u < R; u++)
			cout << next[i][u] << " ";
	 }
	 
	cout << "\n";
	int currRest = -1;
	int row = R-1;
	cout.precision (6);
	cout << dist[L - 1][R - 1] << "\n";
/*	for(int i = 0; i < L; i++) {
		if(tour[i].second != -1 && tour[i].second != currRest) {
			cout << restaurants[tour[i].second].first << " " << restaurants[tour[i].second].second.first
			     << " " << restaurants[tour[i].second].second.second << "\n";
			currRest = tour[i].second;
		}
		cout << landmarks[tour[i].first].first << " " << landmarks[tour[i].first].second.first 
			 << " " << landmarks[tour[i].second].second.second << "\n";
	}
*/	
	vector<pair<string, Point> > revPath;	
	for (int col = L-1; col>=0; col--) {
		 revPath.push_back(make_pair(landmarks[col].first, landmarks[col].second));
		 //cout << landmarks[col].first << " " << landmarks[col].second.first << " " << landmarks[col].second.second << "\n";
		 if (next[col][row] != currRest && next[col][row] != -1) {
			 currRest = next[col][row];
			 row = currRest;
			 revPath.push_back(make_pair(restaurants[currRest].first, restaurants[currRest].second));
			 //cout << restaurants[currRest].first << " " << restaurants[currRest].second.first << " " << restaurants[currRest].second.second << "\n";
		 }
		 //reverse(revPath.begin(), revPath.end());
		// for(int i = 0; i < revPath.size(); i++)
			 //cout << revPath[i].first << " " << revPath[i].second.first << " " << revPath[i].second.second << "\n";
		//cout << landmarks[col].first << " " << landmarks[col].second.first << " " << landmarks[col].second.second << "\n";
		//if (next[col][row] != currRest && next[col][row] != -1) {
		//	cout << restaurants[row].first << " " << restaurants[row].second.first << " " << restaurants[row].second.second << "\n";
		//	currRest = next[col][row];
		//	row++;
		//}
	}
	 reverse(revPath.begin(), revPath.end());
	 for(int i = 0; i < revPath.size(); i++)
	 	cout << revPath[i].first << " " << revPath[i].second.first << " " << revPath[i].second.second << "\n";

	
	//print_path(landpath);
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
