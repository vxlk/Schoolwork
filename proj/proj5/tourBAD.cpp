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

void sp4() {
	/*
	//easiest way to look at it is a path is already built on landmarks
	//insert restaurants in the most optimal positions
	//landmarks.size() / restaurants.size() (int) = the spaces you can go without visiting a restaurant

	//path of landmarks is built
	for(int i = 0; i < landmarks.size(); i++)
		path.push_back(make_pair("landmark", i));

	int spaceWithoutRestaurant = landmarks.size() / restaurants.size();

	pair<int, int> bestPair; //restaurant index, landmark index

	int currentLandmark = path.back().second;
	//find a space for every restaurant
	for (int i = restaurants.size() - 1; i >= 0; i--) {
		double currentBest = 99999.9;
		for (vector<string, Point>::iterator u = landmarks[currentLandmark]; u!= ; u++) {
			if (distanceFormula(restaurants[i].second, landmarks[currentLandmark - u].second) < currentBest) {
				currentBest = distanceFormula(restaurants[i].second, landmarks[currentLandmark - u].second);
				bestPair.first = i;
				bestPair.second = currentLandmark - u;
			}
			path.insert(path.begin()))
		}
	}


	*/
}

#define INF 99999.9
void sp3() {
	int L = landmarks.size();
	int R = restaurants.size() + 1;

	double **dist;
	dist = new double *[R];
	for (int r = 0; r<L; r++) {
		dist[r] = new double[L];
		for (int c = 0; c < L; c++) {
			dist[r][c] = INF;
		}
	}
	int* landpath = new int[L];
	for (int i = 0; i < L; i++)
		landpath[i] = 0;

//	//rest base case
//	for (int i = 1; i < R; i++)
//		dist[0][i] = INF;
	//landmark base case
	for(int u = 1; u < L; u++)
		dist[u][0] = distanceFormula(landmarks[u].second, landmarks[u - 1].second);

	
	//for(int i = 0; i < R; i++)
	int **next;
	next = new int *[R];
	for (int r = 0; r < L; r++) {
		next[r] = new int[L];
		for (int c = 0; c < L; c++)
			next[r][c] = -1;
	}

	/*for (int i = 0; i < landmarks.size(); i++) {
		cout << "\n";
		for (int u = 0; u < restaurants.size(); u++)
			cout << dist[i][u] << " ";
	}
	*/
	//j - 1 is how  you access in the rest array
	dist[1][0] = 0;
	next[1][0] = -1;
	for (int i = 1; i < L; i++) {
		for (int j = 1; j < R; j++) {
			//go thru restaurant
			if (distanceFormula(landmarks[i - 1].second, restaurants[j-1].second) + 
				distanceFormula(restaurants[j-1].second, landmarks[i].second) + dist[i-1][j-1] < dist[i][j])
			{																	//dummy rest node
				dist[i][j] = distanceFormula(landmarks[i - 1].second, restaurants[j-1].second) + distanceFormula(restaurants[j-1].second, 			landmarks[i].second)
 + dist[i - 1][j - 1];
					//landpath[i] = j - 1;
				next[i][j-1] = j - 1;
			}
			else if (distanceFormula(landmarks[i-1].second, landmarks[i].second) + dist[i-1][j] < dist[i][j]) 
			{
				dist[i][j] = distanceFormula(landmarks[i-1].second, landmarks[i].second) + dist[i-1][j];
				//landpath[i] = -1; //no rest in between
			}
		}
	}
	
	for (int i = 0; i < L; i++) {
		cout << "\n";
		for (int u = 0; u < R; u++)
			cout << next[i][u];
	}
 
	int currRest = -1;
	int row = 0;
	cout << "\n" << dist[L - 1][R - 1] << "\n";
	for (int col = 0; col<L; col++) {
		cout << landmarks[col].first << " " << landmarks[col].second.first << " " << landmarks[col].second.second << "\n";
		if (next[col][row] != currRest) {
			cout << restaurants[row].first << " " << restaurants[row].second.first << " " << restaurants[row].second.second << "\n";
			currRest = next[row][col];
			row++;
		}
	}
	//print_path(landpath);
}
void sp2() {
	int N = landmarks.size() + restaurants.size(); //at each step, you have 2 choices, the next restaurant - or the next landmark
	int P = restaurants.size() + landmarks.size(); //path visits every landmark and every restaurant

	double **dist;
	dist = new double *[N];
	for (int r = 0; r<N; r++) {
		dist[r] = new double[P];
		for (int c = 0; c < P; c++) {
			//if (c == N - 1)
			//	dist[r][c] = 0;
			dist[r][c] = INF;
		}
	}

	int **next;
	next = new int *[N];
	for (int r = 0; r<N; r++)
		next[r] = new int[P];

	
	int nextRestaurant = restaurants.size() - 1;
	int nextLandmark = landmarks.size() - 1;
	
	//create a holding block of all possibilities
	vector<pair<string, Point> > wholePath;
	wholePath.insert(wholePath.begin(), restaurants.begin(), restaurants.end());
	wholePath.insert(wholePath.end(), landmarks.begin(), landmarks.end());

	dist[wholePath.size() - 1][P - 1] = 0; //base case
	int prevStep = 0;
	for (int col = N - 2; col >= 0; col--) {
		for (int row = 0; row < P; row++) {
			if (wholePath[row].first == restaurants[nextRestaurant].first || wholePath[row].first == landmarks[nextLandmark].first) {
				for (int nextStep = 0; nextStep < P; nextStep++) {
					if (distanceFormula(wholePath[row].second, wholePath[nextStep].second) + dist[nextStep][col + 1] < dist[row][col]) {
						dist[row][col] = distanceFormula(wholePath[row].second, wholePath[nextStep].second) + dist[nextStep][col + 1];
						next[row][col] = nextStep;
						prevStep = nextStep; //keep track of what you last visited
					}
				}
			}
		}

		if (find(restaurants.begin(), restaurants.end(), wholePath[prevStep]) != restaurants.end())
			nextRestaurant--;
		else nextLandmark--;
		cout << wholePath[prevStep].first << "\n";

	}

	int row = wholePath.size() - 1;
	
	for (int col = 0; col<P; col++) {
		cout << wholePath[row].first << "\n";
		row = next[row][col];
	}
}
/*
void sp() {

	int landmark_ = landmarks.size();
	int restaurant_ = restaurants.size();
	Point** grid = new Point*[restaurants.size()];
	for (int i = 0; i < landmarks.size(); i++)
		grid[i] = new Point;

	//have an array of landmarks and queue of restaurants, when canVisit is true, measure both the queue and the next array index, take the better
	int nextRestaurant = restaurants.size() - 1; //an index
	bool canVisitRestaurant = true; //true until the first one is used, then true after last used + 2
	double distFromStart = 0;
	double needARestaurant = (landmarks.size() / restaurants.size()); //ratio to say whether you have space to visit everything or not

	//insert the start node
	path.push_back(make_pair("landmark", landmark_ - 1));
	
	int i = landmark_ - 2;
		while (i >= 1) {
		//for (; i < landmark_ - 1, path[i - 1].first.first == "landmark"; i++) {
			//if the two landmarks are closer and you don't need to visit a restaurant, or you can't visit a restaurant
			if (distanceFormula(landmarks[i].second, landmarks[i - 1].second) < distanceFormula(restaurants[nextRestaurant].second, landmarks[i].second)
				&& i >= nextRestaurant || canVisitRestaurant == false)
			{
				distFromStart += distanceFormula(landmarks[i].second, landmarks[i - 1].second);
				path.push_back(make_pair("landmark", i));
				canVisitRestaurant = true; //must have at least 2 landmarks in between a restaurant
				i--;
				if (i == 0) break;
			}
			//if you have to visit a restaurant, or it is a shorter distance and your guests are hungry
			if (distanceFormula(landmarks[i].second, landmarks[i - 1].second) >
				distanceFormula(restaurants[nextRestaurant].second, landmarks[i].second) && canVisitRestaurant || i < nextRestaurant)
			{
				distFromStart += distanceFormula(restaurants[nextRestaurant].second, landmarks[i].second);
				path.push_back(make_pair("restaurant", nextRestaurant));
				nextRestaurant--;
				canVisitRestaurant = false;
				if (nextRestaurant == 0) break;
			}
		}
		//have to add last element outside of loop bc of my bad loop structure ay lmao
		if (i == 1) {							//came from
			distFromStart += distanceFormula(restaurants[path[i].second].second, landmarks[i].second);
			path.push_back(make_pair("landmark", i));
		}
		if (nextRestaurant == 1) {
			distFromStart += distanceFormula(landmarks[path[i].second].second, restaurants[0].second);
			path.push_back(make_pair("restaurant", nextRestaurant));
		}
		//insert the end node
		if (path[i].first == "landmark")
			distFromStart += distanceFormula(landmarks[path[i].second].second, landmarks[0].second);
		else distFromStart += distanceFormula(restaurants[path[i].second].second, restaurants[0].second);
		path.push_back(make_pair("landmark", 0));
	cout << distFromStart << "\n";
	print_path();
}
*/
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

	for (int i = 0; i < landmarks.size(); i++)
		cout << landmarks[i].first << "\n";
	for (int i = 0; i < restaurants.size(); i++)
		cout << restaurants[i].first << "\n";

	cout << "\n----------------------------------\n";
	/*
	*x****
	x*x***
	***x*x
	****x*
	******
	*/
	sp3();
	cin.get();
	return 0;
}
