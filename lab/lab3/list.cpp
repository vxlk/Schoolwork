#include "list.h"
using namespace std;

/*
I believe my b of 1000 is a good choice because that is the sqrt of 1 million. It keeps the linked lists small, and it doesn't require me to allocate 
an upsettingly large amount of memory for my table.  Keeping the program portable, even on my grandpa's 2gb of ram.
*/

void list::openWrite() {

	point pointStorage;

	std::ifstream input;
		if (!input) { 
			std::cout << "file not opened";
			return; 
		}

		input.open("points.txt");

		while (input) {

			input >> pointStorage.x >> pointStorage.y;
			insert(pointStorage);
		}
	return;
}

/* Allocate a table of pointers to nodes, all initialized to NULL */
Node ***list::allocate_table(int size)
{
	Node*** table = new Node **[size];
	for (int u = 0; u < size; u++) {
		table[u] = new Node*[size];
		for (int i = 0; i < size; i++) {
			table[u][i] = NULL;
		}
	}
	return table;
}

list::list()
{
	size = 1000; // initial size of table    
	table = allocate_table(size);
}

//delete the table
list::~list()
{
	for (int u = 0; u < size; u++) {
		for (int i = 0; i < size; i++) {
			while (table[u][i] != NULL) {
				Node *temp = table[u][i];
				table[u][i] = table[u][i]->next;
				delete temp;
			}
		}
		delete[] table[u];
	}
	delete[] table;
}



/* Inserts a new key.  It is an error if key is already in the set. */
void list::insert(point key)
{
	table[hashs(key.x, size)][hashs(key.y, size)] = new Node(key, table[hashs (key.x, size)][hashs(key.y, size)]);
}


void list::print(void)
{

	std::cout << "The smallest distance:" << " " << smallestDistance << "\n";

}//end function

double list::distanceFormula(point p1, point p2) {
	const double x_diff = p1.x - p2.x;
	const double y_diff = p1.y - p2.y;
	return std::sqrt(x_diff * x_diff + y_diff * y_diff);
}

void list::compareDistance() {
	
	double distance = 1.0000;
	smallestDistance = distance;

	/*Define all possible directions to move by 1 cell, 8 in total*/
	int move[8][2] = { { -1,0 },{ -1, -1 },{ 0, -1 },{ 1, -1 },{ 1, 0 },{ 1, 1 },{ 0, 1 },{ -1, 1 } };

	for (int i = 0; i < size; i++) { //traverse array of 2d arrays
		for (int u = 0; u < size; u++) { //traverse array of pointers
			for (int l = 0; l < 8; l++) { //check 8 neighbors
				int movei = i + move[l][0];
				int moveu = u + move[l][1];
					//make sure the neighbor is within array bounds
					if (movei < 0 || moveu < 0 || movei >= size || moveu >= size)
						continue;
					//check all the nodes in the neighborhood
					traverseList(table[i][moveu]);
					traverseList(table[movei][u]);
	
		//checks inside current linked list
		traverseList(table[i][u]);
			}
		}
	}
	
	return;
}

void list::traverseList(Node* tables) {

	double distance = 1.0000;

	if (tables != NULL) { //if there exists a head
		Node* head = tables;
		Node* curr = tables->next;
		Node* scouter = tables->next;
		for (scouter; scouter != NULL; scouter = scouter->next) {//
			if (scouter->Point != head->Point)
				distance = distanceFormula(head->Point, scouter->Point); //compare head
			if (distance < smallestDistance)
				smallestDistance = distance;
			for (curr; curr != NULL; curr = curr->next) { //move 1 down the list 
				for (Node* scouterb = head; scouterb != NULL; scouterb = scouterb->next) { //compare every value to the current value //
					if (scouterb->Point != curr->Point)
						distance = distanceFormula(curr->Point, scouterb->Point); //compare next in list
					if (distance < smallestDistance)
						smallestDistance = distance;
				}
			}
		}
	}
} //end function