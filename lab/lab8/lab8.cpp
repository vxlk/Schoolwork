#include <iostream>
#include <cstdlib>
#include <algorithm>
using namespace std;

//globals
int finished = 0;
bool alreadyVisited[1000][1000]{ { false, false } };

struct Bucket {
	int whatsInA;
	int maxA;
	int whatsInB;
	int maxB;
	int index;
	Bucket* prev; //doubly linked list
	Bucket* next;

	//used for init
	Bucket(int maxA, int maxB) : maxA(maxA), maxB(maxB), whatsInA(0), whatsInB(0), index(0)
	{
		prev = NULL;
		next = NULL;
	}

	//used for chaining
	Bucket(int A, int B, Bucket* prevJug) : whatsInA(A), whatsInB(B), index(0) 
	{
		prev = prevJug;
		next = NULL;
	}

	//used for copying
	Bucket(int InA, int InB, int index, Bucket* nextBucket) : whatsInA(InA), whatsInB(InB), index(index)
	{
		next = nextBucket;
	}

};

Bucket* savedOrder;

void recurse(Bucket* jugs, int N, int inA, int inB, int Amax, int Bmax) {   //you swapped pour outs and fills	

/*
	O - O
	 \
	  O
	   \
	    O    keep branching out until you find a dead end
*/

	//update whats held in the structure, if it's a new combination
	if (!alreadyVisited[inA][inB]) {
		alreadyVisited[inA][inB] = true;
		jugs->next = new Bucket(inA, inB, jugs); //make a new branch, it now remembers the previous state
		jugs = jugs->next;
	}

	if (inA + inB == N) {
		//avoid overwrite
		if (finished == 1)
			return;

		Bucket* temp = jugs;
		while (temp->prev) { //skip the first element, it is a placeholder
			if (temp->index == 6) temp = temp->prev; //skip the backtracks, and the start
			savedOrder = new Bucket(temp->whatsInA, temp->whatsInB, temp->prev->index, savedOrder);
			temp = temp->prev;
		}
		finished = 1;
		return;
	}
	
/*
	 /
	O -
	 \    try all solutions for the this node
*/

	//try a new combination
	if(alreadyVisited[inA][inB])
		jugs->index += 1;
	
	//recurse on this node
	if (finished == 0) {
		switch (jugs->index)
		{
		case 0:
			recurse(jugs, N, Amax, jugs->whatsInB, Amax, Bmax); //0
		case 1:
			recurse(jugs, N, jugs->whatsInA, Bmax, Amax, Bmax); //1
		case 2:
			recurse(jugs, N, 0, jugs->whatsInB, Amax, Bmax); //2
		case 3:
			recurse(jugs, N, jugs->whatsInA, 0, Amax, Bmax); //3
		case 4:
			recurse(jugs, N,
				jugs->whatsInA > Bmax - jugs->whatsInB ?
				jugs->whatsInA - (Bmax - jugs->whatsInB) : 0,
				jugs->whatsInA + jugs->whatsInB >= Bmax ?
				Bmax : jugs->whatsInB + jugs->whatsInA,
				Amax, Bmax); //4 
		case 5:
			recurse(jugs, N,
				jugs->whatsInB + jugs->whatsInA > Amax ?
				Amax : jugs->whatsInA + jugs->whatsInB,
				jugs->whatsInB >= (Amax - jugs->whatsInA) ?
				jugs->whatsInB - (Amax - jugs->whatsInA) : 0,
				Amax, Bmax); //5
		default:
			if (!jugs->prev) goto exit;
			recurse(jugs->prev, N, jugs->prev->whatsInA, jugs->prev->whatsInB, Amax, Bmax); //backtrack
		}
	}

	exit:
	return;
}


bool graphSearch(Bucket* jugs, int N, int bucketAMax, int bucketBMax) {
	
	recurse(jugs, N, 0, 0, bucketAMax, bucketBMax);
	
	//found a path
	if (finished == 1)
		return true;
	
	return false;
}

void convertToString(int index) {

/*
		fill A -> 0
		fill B -> 1
		empty A -> 2
		empty B -> 3
		pour A->B 4
		pour B->A 5
*/

	switch (index) {
	case 0:
		cout << "\nFill Jug 1 ";
		break;
	case 1:
		cout << "\nFill Jug 2 ";
		break;
	case 2:
		cout << "\nEmpty Jug 1 ";
		break;
	case 3: 
		cout << "\nEmpty Jug 2 ";
		break;
	case 4: 
		cout << "\nPour 1 - > 2 ";
		break;
	case 5: 
		cout << "\nPour 2 -> 1 ";
		break;
	}
}

int main(void) {

	int bucketA;
	int bucketB;
	int N;

	cout << "Enter Jug 1 Fill Capacity: \n";
	cin >> bucketA;
	cout << "Enter Jug 2 Fill Capacity: \n";
	cin >> bucketB;
	cout << "Enter The Combination You Want: \n";
	cin >> N;

	//init
	Bucket* jugs = new Bucket(bucketA, bucketB);
							  //maxA     maxB
	if (!graphSearch(jugs, N, bucketA, bucketB)) {
		cout << "impossible!\n";
	return 0;
	}
	
	//print in order
	savedOrder = savedOrder->next; //skip the first 0,0 combination, as per instructions
	while (savedOrder) {
		convertToString(savedOrder->index);
		cout << " |" << savedOrder->whatsInA << "| |" << savedOrder->whatsInB << "| \n";
		savedOrder = savedOrder->next;
	}
	
	return 0;
}