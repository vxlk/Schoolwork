
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cmath>
//#include <string>
#include <assert.h>

struct point {
	double x;
	double y;
	bool operator!=(const point& right) {
		return!(this->x == right.x && this->y == right.y);

	}
};

struct Node {
	point Point;
	Node *next;
	Node(point insertedPoint, Node *n) { this->Point.x = insertedPoint.x; this->Point.y = insertedPoint.y; next = n; }
	Node() { this->Point.x = 0.0; this->Point.y = 0.0; next = NULL; }
};

class list {

	private:
		Node *** table;
		int size;
		double smallestDistance; 

	public:
		list();
		~list();
		void traverseList(Node*);
		double distanceFormula(point p1, point p2);
		void insert(point);
		void print(void);
		void compareDistance();
		void openWrite();
		Node*** allocate_table(int size);

		/*Hash*/
		inline int hashs(double point, int table_size)
		{
			unsigned int h = 0;
			point *= 1000;
				h = (h * 29177589434754733 + (unsigned int)point) % table_size; //should be unique
			return h;
		}


};
