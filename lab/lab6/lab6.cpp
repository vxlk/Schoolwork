#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <ctime>

struct Point {
	double x;
	double y;
	int index;
	Point() : index(-1) {}
};

double distanceFormula(Point p1, Point p2) {
	const double x_diff = p1.x - p2.x;
	const double y_diff = p1.y - p2.y;
	return std::sqrt(x_diff * x_diff + y_diff * y_diff);
}

void reversePath(Point* pointArray, int first, int second) {
	while (first < second) {
		std::swap(pointArray[first], pointArray[second]);
		first++;
		second--;
	}
}

int main(void) {


	int n = 0;
	//Read in file
	std::ifstream in;
	in.open("tsp_points.txt");

	Point* points = new Point[50];

	while (in >> points[n].x >> points[n].y) {
		points[n].index = n;
		n++;
	}

	int newArray[50];
	double totalDistance = 0.0;
	double bestDistance = 10000.0;

	for (int v = 0; v < 100; v++) {
		int tourDistance = 0;
		//random permutation
		for (int i = 1; i < n; i++) {
			std::srand(std::time(0));
			int randomIndex = rand() % i;
			//reversePath(points, randomIndex, i); //reverse everything in the middle, rotating the path
			std::swap(points[randomIndex], points[i]);
		}
			//get initial distance
			for (int y = 1; y < n; y++)
				tourDistance += distanceFormula(points[y], points[y - 1]);

			//refine this tour to make it better
		for (int i = 0; i < 3; i++) { //run multiple times to make sure you get the best
			for (int w = 0; w < n; w++) {
				for (int u = w + 1; u < n; u++) {
					int thisDistance = 0;
					reversePath(points, w + 1, u); //try reversal
					for (int y = 1; y < n; y++) //get new distance
						thisDistance += distanceFormula(points[y], points[y - 1]);

					if (thisDistance < tourDistance) { //if best, keep
						tourDistance = thisDistance;
						continue;
					}
					else reversePath(points, w + 1, u); //or go back

				}
			}
		}
			//record your distance for this tour
			totalDistance = 0;
			for (int y = 1; y < n; y++)
				totalDistance += distanceFormula(points[y], points[y - 1]);
			
			//include the travel back
			totalDistance += distanceFormula(points[0], points[n - 1]);
			
			//write the best available to a new array
			if (totalDistance < bestDistance) {
				for (int i = 0; i < n; i++)
					newArray[i] = points[i].index;
				bestDistance = totalDistance;
			}
		}
	
	//write the distance held in the best array
	std::cout << bestDistance << "\n";

	for (int i = 0; i < n; i++)
		std::cout << newArray[i] << " ";


	return 0;
}
