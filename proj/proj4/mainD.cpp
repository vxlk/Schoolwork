#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <queue>
#include <assert.h>
#include <vector>
#include <functional>
#include <math.h>
#include <time.h>
using namespace std;

int N, D, K;


struct Point {
	int d;
	double quality; //col
	double *data; //row
	Point& operator=(const Point& other) {
		this->quality = other.quality;
		this->data = new double[D];
		for (int i = 0; i < D; i++)
			this->data[i] = other.data[i];
		return *this;
	}
};
bool operator<(const Point & p1, const Point & p2) { return p1.quality < p2.quality; }

struct Node {
	int split; //axis
	int index;
	Node *left;
	Node *right;
	Node(double *_key, int _i, int _split) { //init at a given key
		split = _split;
		index = _i;
		left = right = NULL;
	}
	Node() { //init to 0
		left = right = NULL;
	}
};

vector<double> guessVector;
const double e = M_E;
priority_queue<pair<double, Point> > Q; //will hold from largest to smallest
const double a = -0.1; //for weighted avgs
Point *P;

Node *insert(Node *T, double* key, int dim, int i)
{
	if (T == NULL) return new Node(key, i, dim);
	if (key[dim] < P[T->index].data[dim]) {
		T->split = dim;
		T->left = insert(T->left, key, (dim + 1) % D, i);
	}
	else {
		T->split = dim;
		T->right = insert(T->right, key, (dim + 1) % D, i);
	}
	return T;
}

double euclidianDistanceOneDim(double* p1, double* p2, int i) {
	    double pData = 0.0;
		pData += ((p1[i] - p2[i]) * (p1[i] - p2[i]));   //Σ p^M
	return sqrt(pData); //euclidian distance^M
}

double euclidianDistance(double* p1, double* p2) {
	double pData = 0;
	for (int i = 0; i < D; i++)
		pData += ((p1[i] - p2[i]) * (p1[i] - p2[i]));	//Σ p
	return sqrt(pData); //euclidian distance
}

double getError(vector<double> g, Point* a, int N) {
	double total = 0.0;
	for (int i = 0; i < N; i++)
		//(a[i] - g[i])^2
		total += ((a[i].quality - g[i]) * (a[i].quality - g[i]));
	
	return (total / N);
}

void addToQueue(double d, Point po) { //needed to keep the queue a fixed size of K
	pair<double, Point> p;
	p.first = d;
	p.second = po;
	if (Q.empty() || Q.size() < K)
		Q.push(p);

	if (Q.top().first > p.first && Q.size() == K) {
		Q.pop();
		Q.push(p);
	}
	return;
}


void NNSutil(Point p, Node* head, Node* T, int& dim) {
	if (T == NULL) return;

	double dist = euclidianDistance(p.data, P[T->index].data);
	if (dist < Q.top().first && dist != 0)
		addToQueue(dist, P[T->index]);
	dim = (dim + 1) % D;

		NNSutil(p, head, T->left,dim);
		NNSutil(p, head, T->right,dim);
}

void NNSinter(Point p, Node* head, Node* T, int dim) {
  double dist = euclidianDistance(p.data, P[head->index].data);
	if (dist < Q.top().first && dist != 0)
		addToQueue(dist, P[head->index]);

 if (p.data[dim] <= P[head->index].data[dim]) {
	 dim = 1;
	 NNSutil(p, head, T->left, dim);
	 if (euclidianDistanceOneDim(p.data, P[head->index].data, p.d) < Q.top().first) {
		dim = 1;
		NNSutil(p, head, T->right, dim);
	 	return;
	 }
	 else return;
	}

 else if (p.data[dim] > P[head->index].data[dim]) {
	 dim = 1;
	 NNSutil(p, head, T->right, dim);
	if (euclidianDistanceOneDim(p.data, P[head->index].data, p.d) < Q.top().first) {
		dim = 1;
		NNSutil(p, head, T->left, dim);
		return;
		}
	else return;
	}

}

void computeWeightedAvg() {
	//transfer to array for traversal first
	double denominator = 0.0;
	double numerator = 0.0;
	while (!Q.empty()) {
		double w = pow(e, ((a * Q.top().first)));
		numerator += w * Q.top().second.quality;
		denominator += w;
		Q.pop();
		//Σ (weight * value) / Σ (weight)
	}
	guessVector.push_back(numerator / denominator); //adds to collection of guesses
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		cout << "Usage: wine <data file> <k>\n";
		return 0;
	}

	/* Read input data */
	ifstream fin(argv[1]);
	K = atoi(argv[2]);
	fin >> N >> D;
	int di = 0;
	P = new Point[N];
	for (int i = 0; i<N; i++) {
		fin >> P[i].quality; //3691 qualities
		P[i].data = new double[D]; //11 datas
		for (int j = 0; j<D; j++) {
			fin >> P[i].data[j]; //11 datas now read into each i/11 qualities
		}
	}
	fin.close();

	/* Normalize data in each coordinate */
	for (int j = 0; j<D; j++) {
		double mean = 0.0;
		for (int i = 0; i<N; i++) mean += P[i].data[j];
		mean /= N;
		for (int i = 0; i<N; i++) P[i].data[j] -= mean;
		double var = 0.0;
		for (int i = 0; i<N; i++) var += pow(P[i].data[j], 2);
		double stddev = sqrt(var / N);
		for (int i = 0; i<N; i++) P[i].data[j] /= stddev;
	}

	// Now in each coordinate, we have a mean of zero and variance/standard deviation
	// of one, so the data set is "centered" at the origin and has the same relative
	// scale along each axis (so no attribute is "more important" than any other
	// attribute when computing distances).

	//insert in random order
	Node *T = NULL;
	for (int i = 1; i < N; i++) {
		int u = rand() % i;
		swap(P[i], P[u]);
	}

	//insert the now randomized data
	for (int i = 0; i < N; i++) {
		T = insert(T, P[i].data, 0, i);
	}

	for (int i = 0; i < N; i++) {
		for (int y = 0; y < K; y++)
			 addToQueue(1000.0, P[i]);
		NNSinter(P[i], T, T, 0);
		computeWeightedAvg();
	}

	double error = getError(guessVector, P, N);
	cout << error << "\n";

	return 0;
}

