#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <ctime>
#include <set>

//contains a lot of the old things I was trying to do. Did not have time to parse ... sorry.

struct Qualities {
	int weight; //in kg
	int tastiness;
	int bag;
	bool alreadyVisited;
	bool pruned;
	Qualities() : weight(0), 
		tastiness(0) {
		alreadyVisited = false;
		pruned = false;
	}
};

struct Bag {
	int canCarry; //can carry 2kg
	int weight;
	int tastinessInBag;
	int* candyContents;
	int piecesCount;
	std::vector<int> bestCombination;
	std::vector<int> prunedVector;
	std::vector<int> candyNotInOrder; //do the same thing as before with a vector not a set, so it's not in order
	std::set<int> candy;
	Bag() : canCarry(2000), weight(0), tastinessInBag(0), piecesCount(0) { candyContents = new int[20]; }
	
};

struct Ratio {
	double tastinessRatio;
	int index; //keep track of the position that candy was in, in the original vector
	int rank; //its index in the ratio array
};

void swap(Ratio* xp, Ratio* yp) {
	Ratio temp = *xp;
	*xp = *yp;
	*yp = temp;
}

// yay bubble sort
void bubbleSort(Ratio* arr, int n) {
	int i, j;
	for (i = 0; i < n - 1; i++)
		// Last i elements are already in place   
		for (j = 0; j < n - i - 1; j++)
			if (arr[j].tastinessRatio < arr[j + 1].tastinessRatio)
				swap(&arr[j], &arr[j + 1]);
}

int getBagWeight(Bag bag, Ratio* tastinessToWeight, std::vector<Qualities> quals) {
	//candy maps to tastinesstoweight
	//tastinesstoweight maps to the original vector
	int currentWeight = 0;
	int indexSummation = 0;
	for (std::set<int>::iterator it = bag.candy.begin(); it != bag.candy.end(); it++) {
		std::cout << *it << "\n";
		int ratioIndex = *it;
		if (currentWeight + quals[tastinessToWeight[ratioIndex].index].weight <= bag.canCarry) {
			currentWeight += quals[tastinessToWeight[ratioIndex].index].weight;
			indexSummation += ratioIndex;
		}
	}
	std::cout << "--------------\n";
	return indexSummation; //all the indexes used in the bag
}

int getBagWeightVec(Bag bag, Ratio* tastinessToWeight, std::vector<Qualities> quals) {
	//candy maps to tastinesstoweight
	//tastinesstoweight maps to the original vector
	int currentWeight = 0;
	int indexSummation = 0;
	for (std::vector<int>::iterator it = bag.candyNotInOrder.begin(); it != bag.candyNotInOrder.end(); it++) {
		//std::cout << *it << "\n";
		int ratioIndex = *it;
		if (currentWeight + quals[tastinessToWeight[ratioIndex].index].weight <= bag.canCarry) {
			currentWeight += quals[tastinessToWeight[ratioIndex].index].weight;
			indexSummation += ratioIndex;
		}
	}
	//std::cout << "--------------\n";
	return indexSummation; //all the indexes used in the bag
}


int getBagTastiness(Bag bag, Ratio* tastinessToWeight, std::vector<Qualities> quals) {
	//candy maps to tastinesstoweight
	//tastinesstoweight maps to the original vector
	int currentTastiness = 0;
	int currentWeight = 0;
	for (std::set<int>::iterator it = bag.candy.begin(); it != bag.candy.end(); it++) {
		//std::cout << *it << "\n";
		int ratioIndex = *it;
		if (currentWeight + quals[tastinessToWeight[ratioIndex].index].weight <= bag.canCarry) {
			currentWeight += quals[tastinessToWeight[ratioIndex].index].weight;
			currentTastiness += quals[tastinessToWeight[ratioIndex].index].tastiness;
		}
	}
	//std::cout << "--------------\n";
	return currentTastiness;
}

int getBagTastinessVec(Bag bag, Ratio* tastinessToWeight, std::vector<Qualities> quals) {
	//candy maps to tastinesstoweight
	//tastinesstoweight maps to the original vector
	int currentTastiness = 0;
	int currentWeight = 0;
	for (std::vector<int>::iterator it = bag.candyNotInOrder.begin(); it != bag.candyNotInOrder.end(); it++) {
		//std::cout << *it << "\n";
		int ratioIndex = *it;
		if (currentWeight + quals[tastinessToWeight[ratioIndex].index].weight <= bag.canCarry) {
			currentWeight += quals[tastinessToWeight[ratioIndex].index].weight;
			currentTastiness += quals[tastinessToWeight[ratioIndex].index].tastiness;
		}
	}
	//std::cout << "--------------\n";
	return currentTastiness;
}

void swapCandies(Bag& bag1, Bag& bag2, int index) {
	//insert into two
	//delete out of one
	std::set<int>::iterator it;
	it = bag1.candy.find(index);
	if (it == bag1.candy.end()) return; //should never happen. just in case
	bag1.candy.erase(it);
	bag2.candy.insert(index);
}

void swapCandiesVec(Bag& bag1, Bag& bag2, int index) {
	//cant find return
	std::vector<int>::iterator it = std::find(bag1.candyNotInOrder.begin(), bag1.candyNotInOrder.end(), index);
	if (it == bag1.candyNotInOrder.end())
		//return;
		std::cout << "gonna crash here";
	
	//std::advance(it, index);
	bag1.candyNotInOrder.erase(it);
	bag2.candyNotInOrder.insert(bag2.candyNotInOrder.begin(), index); //insert at beginning of new bag, so it's always counted
}

std::set<int> filter(std::set<int>& indexSummations, int index, bool& found) {
	
	std::set<int>::iterator it = indexSummations.find(index);
	if (it == indexSummations.end()) { //if it couldn't find
		indexSummations.insert(index);
		found = false;
	}
	
	else found = true;

	return indexSummations;
}

void pruneVector(Bag bag, std::vector<Qualities> quals, Ratio* tastinessToWeight) {
	//this would return only the values in the vector that would be used in the weighing process
	int currentWeight = 0;
	for (int i = 0; i < bag.candyNotInOrder.size(); i++)
		if (currentWeight + quals[tastinessToWeight[i].index].weight <= bag.canCarry) {
			currentWeight += quals[tastinessToWeight[i].index].weight;
			bag.prunedVector[i] = bag.candyNotInOrder[i];
		}
}

int getTastinessArray(int* a, Ratio* tastinessToWeight, std::vector<Qualities> quals) {
	int currentTastiness = 0;
	int currentWeight = 0;
	int i = 0;
	int index;
	int size = sizeof(a) / sizeof(a[0]);
	while (currentWeight < 2000 && i < size) {
		//std::cout << *it << "\n";
			index = a[i];
			currentWeight += quals[tastinessToWeight[index].index].weight;
			currentTastiness += quals[tastinessToWeight[index].index].tastiness;
			i++;
	}
	//std::cout << "--------------\n";
	return currentTastiness;
}

int getTastinessArrayMark(int* a, Ratio* tastinessToWeight, std::vector<Qualities>& quals) {
	int currentTastiness = 0;
	int currentWeight = 0;
	int i = 0;
	int size = sizeof(a) / sizeof(a[0]);
	while (currentWeight < 2000 && i < size) {
		//std::cout << *it << "\n";
		currentWeight += quals[tastinessToWeight[a[i]].index].weight;
		currentTastiness += quals[tastinessToWeight[a[i]].index].tastiness;
		quals[tastinessToWeight[a[i]].index].alreadyVisited = true; //mark it as visited
		i++;
	}
	//std::cout << "--------------\n";
	return currentTastiness;
}

void permute(int *a, int*& bestArray, int l, int r, int& currentTastiness, int& bestTastiness, Ratio* tastinessToWeight, std::vector<Qualities> quals)
{
	if (tastinessToWeight[a[0]].tastinessRatio < 1.5)
		return;
	int i;
	if (l == r) {
		currentTastiness = getTastinessArray(a, tastinessToWeight, quals);
		if (currentTastiness > bestTastiness) {
			bestTastiness = currentTastiness;
			bestArray = a;
		}
	}
		
	else
	{
		for (i = l; i <= r; i++)
		{
			std::swap(a[l], a[i]);
			//filter(std::set<int>& indexSummations, int index, bool& found)
			permute(a, bestArray, l + 1, r, currentTastiness, bestTastiness, tastinessToWeight, quals);
			std::swap(a[l], a[i]);
		}
	}
}

void exhaustive(Ratio* tastinessToWeight, Bag* bag, int n, bool* alreadyVisited, std::vector<Qualities> qual, std::set<int> indexSummations) {

	for (int t = 0; t < n; t++) 
			qual[t].alreadyVisited = false;

	int bestTastiness = 0;
	int currentTastiness = 0;
	int x = 0;
	int h = 0;
	int* bestOfBag[3];
	//insert everything left into one bag
	
	for (int b = 0; b < 3; b++) {
		x = 0;
		//put candy in
		for (int i = 0; i < n; i++) {
			if (qual[tastinessToWeight[i].index].alreadyVisited == false)
				bag[b].candyContents[x] = tastinessToWeight[i].index;
				x++;
		}
		//permute best
		//mark these values as read
		permute(bag[b].candyContents, bestOfBag[b], 0, n - 1, currentTastiness, bestTastiness, tastinessToWeight, qual);
		for (int y = 0; y < x; y++) { //set used to visited
			///qual[tastinessToWeight[bestOfBag[b][y]].index].alreadyVisited = true;
		}
	}
	
		//do one bag at a time
		//get the optimal solution
		//flag those ones
		//do that for the rest;
	int finalvalue = 0;
	for (int i = 0; i < 3; i++) {
		for (int u = 0; u < n; u++) {
			finalvalue += qual[bestOfBag[i][u]].tastiness;
			std::cout << "\n" << finalvalue + bestTastiness;
		}
	}
}

void combinationUtil(int arr[], int data[], int best[], int start, int end,
	int index, int r, Ratio* tastinessToWeight, std::vector<Qualities> qual, int& bestTaste);

void interface(int arr[], int best[], int n, int r, Ratio* tastinessToWeight, std::vector<Qualities> qual, int& bestTaste)
{
	// A temporary array to store all combination one by one
	int* data = new int[r];

	// Print all combination using temprary array 'data[]'
	combinationUtil(arr, data, best, 0, n - 1, 0, r, tastinessToWeight, qual, bestTaste);
}



void combinationUtil(int arr[], int data[], int best[], int start, int end,
	int index, int r, Ratio* tastinessToWeight, std::vector<Qualities> qual, int& bestTaste) {
	// Current combination is ready to be printed, print it
	if (index == r)
	{
		int size = sizeof(data) / sizeof(data[0]);
		//std::sort(data, data + r - 1);
		while (std::next_permutation(data, data + r - 1 )) {
			int currentTaste = getTastinessArray(data, tastinessToWeight, qual);
			if (currentTaste > bestTaste) {
				bestTaste = currentTaste;
				for (int j = 0; j < r; j++)
					best[j] = data[j];
			}
		}
		return;
	}

	// replace index with all possible elements. The condition
	// "end-i+1 >= r-index" makes sure that including one element
	// at index will make a combination with remaining elements
	// at remaining positions
	for (int i = start; i <= end && end - i + 1 >= r - index; i++) {
		data[index] = arr[i];
		combinationUtil(arr, data, best, i + 1, end, index + 1, r, tastinessToWeight, qual, bestTaste);
	}
}


void testEachBag(Ratio* tastinessToWeight, Bag* bag, int n, bool* alreadyVisited, std::vector<Qualities> qual, std::set<int> indexSummations) {
	
	int finalValue = 0;
	int bestTastiness = 0;
	int currentTastiness = 0;
	int x = 0;
	int h = 0;
	int r = 6;
	int bestTaste = 0;
	int* bestOfBag1 = new int[6];
	int* bestOfBag2 = new int[6];
	int* bestOfBag3 = new int[6];
	
	for (int b = 0; b < 3; b++) {
		x = 0;
		//put candy in
		for (int i = 0; i < n; i++) {
			if (qual[tastinessToWeight[i].index].alreadyVisited == false) {
				bag[b].candyContents[x] = tastinessToWeight[i].index;
				x++;
			}
		}
		//calls the recursion
		if (b == 0) {
			interface(bag[b].candyContents, bestOfBag1, n, r, tastinessToWeight, qual, bestTaste);
			for (int i = 0; i < 6; i++) {
				finalValue = getTastinessArrayMark(bestOfBag1, tastinessToWeight, qual);
		//		finalValue += qual[tastinessToWeight[bestOfBag1[i]].index].tastiness;
				//std::cout << bestOfBag1[i] << "\n";
			}
		//	std::cout << finalValue << "\n";
		}

		else if (b == 1) {
			interface(bag[b].candyContents, bestOfBag2, n, r, tastinessToWeight, qual, bestTaste);
			for (int i = 0; i < 6; i++) {
				//finalValue += getTastinessArrayMark(bestOfBag1, tastinessToWeight, qual);
				//std::cout << bestOfBag2[i] << "\n";
			}
		}

		else {
			interface(bag[b].candyContents, bestOfBag3, n, r, tastinessToWeight, qual, bestTaste);
			for (int i = 0; i < 6; i++) {
				//finalValue += getTastinessArrayMark(bestOfBag1, tastinessToWeight, qual);
				//finalValue += bestTastiness;
				//std::cout << bestOfBag3[i] << "\n";
			}
		}
	}
	std::cout << "Exhaustive: " << finalValue << "\n";
}

void recursion(std::vector<Qualities> qual, int start, int end, int& index, std::vector<int> currentlyHeld, int n) {
	int currentWeight = 0;

	for (int i = 0; i < start; i++)
		currentWeight += qual[i].weight;

	if (start == end)
		//check if best
	//add the candy
	while (currentWeight + qual[index].weight < 2000) {
		currentlyHeld.push_back(index);
		index++; //index is where the next one begins
		}
	while (index != n) {
		index++;
		if (qual[index].weight + currentWeight < 2000) {
			currentlyHeld.push_back(index);

		}
	}
	if (index == n) {
		end--;
	}
		
		//at vector[index] ++ the value until it == n
		//then ++ index - 1 until index == start
		//if it makes space, go to end + 1 and try all possibilities with the current combination

}

void ExhaustiveApproach(Bag* bags, std::vector<Qualities> qual, int n) {

	int best = 0;
	int currentWeight = 0;
	int oldSize;
	int bestTastiness = 0;
	//std::vector<int> inBag;
	int* bestInBag = new int[n];
	for (int b = 0; b < 1; b++) {
		for (int v = 0; v < n; v++) {
			//stuffing the bag full
			bags[b].candyContents[v] = v;
			bags[b].weight += qual[v].weight;
			qual[v].alreadyVisited = true;
			bags[b].piecesCount++;
			int tempCount = bags[b].piecesCount;
			for (int i = 0; i < n; i++) {
			
				do {
					for (int h = 0; h < 16; h++) {
						if (qual[h].weight + bags[b].weight < 2000 && !qual[h].alreadyVisited && !qual[h].pruned) {
							bags[b].weight += qual[h].weight;
							bags[b].tastinessInBag += qual[bags[b].piecesCount].tastiness;
							qual[h].bag = b;
							bags[b].candyContents[bags[b].piecesCount] = h;
							bags[b].piecesCount++;
							std::cout << bags[b].candyContents[bags[b].piecesCount - 1] << " " << bags[b].weight << "\n";
						}
					}
					//check if it is best
					if (best < currentWeight) {
						best = currentWeight;
						for (int y = 0; y < bags[b].piecesCount; y++)
							bestInBag[y] = bags[b].candyNotInOrder[y];
					
					}

					/*//reset
					for (int t = 0; t < n; t++) {
						if (qual[t].bag != b)
							qual[t].alreadyVisited = false;
					}
					*/

					if (tempCount < bags[b].piecesCount) { //did not remove one, we are done
						bags[b].weight -= qual[bags[b].piecesCount].weight;
						bags[b].tastinessInBag -= qual[bags[b].piecesCount].tastiness;
						qual[bags[b].piecesCount].alreadyVisited = true;
						bags[b].piecesCount--;
					}
					
				} while (tempCount > bags[b].piecesCount);

				if (bags[b].piecesCount != 1) { //at head
					bags[b].weight -= bags[b].candyContents[bags[b].piecesCount];
					bags[b].tastinessInBag -= bags[b].candyContents[bags[b].piecesCount];
					qual[bags[b].piecesCount].alreadyVisited = true;
					bags[b].piecesCount--; //remove
				}

				for (int t = 0; t < n; t++) {
					if (t != bags[b].piecesCount + 1)
						qual[t].alreadyVisited = false;
				}
				qual[bags[b].piecesCount].pruned = true;



			}
		}
	}
	//int totalTastiness = 0;

	for (int i = 0; i < 3; i++) {
		for (int u = 0; u < n; u++) {
			//totalTastiness += qual[bestInBag[i]].tastiness;
		}
	}
}

int main(void) {

	int n = 0;
	//Read in file
	std::ifstream in;
	in.open("candy.txt");

	std::vector<Qualities> qual(1); //initial size of 1
	Bag* bag = new Bag[4]; //3 bags, all carry 2 kgs

	while (in >> qual[n].weight >> qual[n].tastiness) {
		qual.push_back(qual[n]);
		n++;
	}

	//create an array of ratios tastiness/weight
	Ratio* tastinessToWeight = new Ratio[n];
	for (int i = 0; i < n; i++) {
		double ratioVal = qual[i].tastiness / (double)qual[i].weight;
		tastinessToWeight[i].tastinessRatio = ratioVal;
		tastinessToWeight[i].index = i;
	}

	//reverse sort the array of ratios ->want the most tastiness to least weight
	bubbleSort(tastinessToWeight, n);
	
	//init before each algorithm-------------------
	bool* alreadyVisited = new bool[n];
	for (int i = 0; i < n; i++)
		alreadyVisited[i] = false;

	//Greedy
	int u = 0; //keep track of what bag
	int x = 0; //keep track of ratio index
	//since your candy is sorted by best, just go down the array and add the next best while you can...
	while (u <= 3)
	{
		if (bag[u].weight + qual[tastinessToWeight[x].index].weight <= bag[u].canCarry) { //if you can add. do it
			bag[u].weight += qual[tastinessToWeight[x].index].weight;
			bag[u].tastinessInBag += qual[tastinessToWeight[x].index].tastiness;
			alreadyVisited[tastinessToWeight[x].index] = true; //mark it as seen, so it doesn't go back over later
			x++; //move up the ratio array
		}
		else {
			for (int i = 0; i < n; i++) //look for something more to fit, b4 moving on
				if (bag[u].weight + qual[i].weight <= bag[u].canCarry && alreadyVisited[i] == false) {
					bag[u].weight += qual[i].weight;
					bag[u].tastinessInBag += qual[i].tastiness;
					alreadyVisited[i] = true;
				}
			u++; //go to next bag, if we can't find any more that fit
		}
	}

	std::cout << "Greedy: " << bag[0].tastinessInBag + bag[1].tastinessInBag + bag[2].tastinessInBag << "\n";
	//std::cout << "Weight: " << bag[0].weight << " " << bag[1].weight << " " << bag[2].weight;
	

	for (int i = 0; i < n; i++)
		tastinessToWeight[i].rank = i;
	//Iterative
	//init before each algorithm-------------------
	for (int i = 0; i < n; i++)
		alreadyVisited[i] = false;
	//fill all 3 randomly using all candy
	//pick a random two and swap them
	//if better keep. better is closer to 2k 2k 2k
	int bestTastiness = 0;
	for (int x = 0; x < 1000; x++) {
		
		for (int i = 0; i < 3; i++) { //reset bags
			bag[i].tastinessInBag = 0;
			bag[i].weight = 0;
			bag[i].candy.clear();
		}
		
		for (int i = 0; i < n; i++) { //randomly insert
			//std::srand(std::time(0));
			int u = rand() % 3;
				bag[u].weight += qual[i].weight;
				bag[u].tastinessInBag += qual[i].tastiness;
				//with this insertion method, the first will always be the best -> use candy.set to call tastinesstoWeight.index to get the o.g. index
				bag[u].candy.insert(i);
				//keep track of what candy is in what bag
				qual[i].bag = u;
		}

			for (int w = 0; w < n; w++) { //look through all the available candy
				for (int b = 0; b < 3; b++) { //try all bags
					if (b == qual[w].bag)
						continue;
					int prevBag = qual[w].bag; //so you can go back
					//std::cout << "bag it's in->" << qual[w].bag << " " << "bag it's going in->" << b << " "  << w << "\n";
					//std::cout << "index->" << tastinessToWeight[w].index << "\n";
					swapCandies(bag[qual[tastinessToWeight[w].index].bag], bag[b], tastinessToWeight[w].index); //swap the current candy to all bags
					qual[tastinessToWeight[w].index].bag = b;
					//qual[w].bag = b; //update the candy's new bag

					int totalTaste = getBagTastiness(bag[0], tastinessToWeight, qual)
						+ getBagTastiness(bag[1], tastinessToWeight, qual)
						+ getBagTastiness(bag[2], tastinessToWeight, qual);
					//is it better?
					if  (totalTaste > bestTastiness) {
						bestTastiness = totalTaste;
						continue;
					}

					else {
						//qual[w].bag = prevBag;
						qual[tastinessToWeight[w].index].bag = prevBag;
						swapCandies(bag[b], bag[qual[tastinessToWeight[w].index].bag], tastinessToWeight[w].index); //swap back
					}
				}
			}
		}

	std::cout << "Refinement: " << bestTastiness << "\n";
	//Pruned Exhaustive
	//create a set of tried index summations
	std::set<int> indexSummations;
	int i = 0;
	bestTastiness = 0;
	//recursive swap
	//add up used indexes
	//if !find -> add
	//if added, compare the new tastiness
	//if better keep
	for (int i = 0; i < 3; i++) { //reset bags
		bag[i].tastinessInBag = 0;
		bag[i].weight = 0;
		bag[i].candy.clear();
	}
	
	//recursiveSearch(i, n-1, bestTastiness, indexSummations, bag, tastinessToWeight, qual);
	//exhaustive(tastinessToWeight, bag, n, alreadyVisited, qual, indexSummations);
	testEachBag(tastinessToWeight, bag, n, alreadyVisited, qual, indexSummations);
	//ExhaustiveApproach(bag, qual, n);

	return 0;
}
