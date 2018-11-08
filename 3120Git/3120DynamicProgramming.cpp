#include <iostream>
#include <algorithm>

#define W 10

int weightedInterval(int* start, int* finish, int* val, int numItems)
{
	//sort start and finish arrays

	//generate p array
	int* p = new int[numItems];
	for (int i = 0; i < numItems; ++i)
	{
		p[i] = 0;
		for (int j = 0; j < i; ++j)
			if (finish[j] <= start[i] && val[j] > p[i]) 
				p[i] = j+1; //index from 1...numItems !! 0...numItems
	}

	//solve using dp
	int* A = new int[numItems+1];
	A[0] = 0; //include the case of using none, ie val = 0;
	for (int i = 1; i <= numItems; ++i)
		A[i] = std::max(A[i - 1], val[i - 1] + A[p[i - 1]]);
	return A[numItems];
}

int knapSack(int* weight, int* val, int numItems)
{
	//dynamic 2d array
	/*
	numItems WWWWWWWWWWWW
	numItems WWWWWWWWWWWW
	numItems WWWWWWWWWWWW
	numItems WWWWWWWWWWWW
	*/
	int** A = new int*[numItems+1];
	for (int i = 0; i < numItems+1; ++i)
	{
		A[i] = new int[W+1];
		for (int j = 0; j < numItems+1; ++j)
			A[i][j] = 0;
	}

	//solve via DP
	for (int i = 1; i <= numItems; ++i)
	{
		for (int w = 1; w <= W; ++w)
		{
			//if (!w) continue; //leave none taken @ 0
			if (w < weight[i-1]) 
				A[i][w] = A[i - 1][w]; //get prev best
			else A[i][w] = std::max(A[i - 1][w], val[i-1] + A[i-1][w - weight[i-1]]); //best between not taking this and taking it
																					//compare prev val to this->val + its best prev combination
		}
	}
	return A[numItems][W];
}

int lcsubsequence(std::string seq1, std::string seq2)
{
	//make table
	int** A = new int*[seq1.size() + 1];
	for (int i = 0; i < seq1.size() + 1; ++i)
	{
		A[i] = new int[seq2.size() + 1];
		for (int j = 0; j < seq2.size() + 1; ++j)
		{
			A[i][j] = 0;
		}
	}

	//solve using dp
	for (int i = 1; i < seq1.size() + 1; ++i)
	{
		for (int j = 1; j < seq2.size() + 1; ++j)
			if (seq1[i - 1] == seq2[j - 1])
				A[i][j] = A[i - 1][j - 1] + 1; //add to curr best
			else A[i][j] = std::max(A[i - 1][j], A[i][j - 1]); //keep track of prev best
	}
	return A[seq1.size()][seq2.size()];
}

//entry point
int main()
{
	int weight[5] = { 3, 4, 6, 2, 7 };
	int value[5] = { 5, 7, 8, 4, 2 };

	int start[5] = { 1, 2, 4, 6, 9 };
	int end[5] = { 3, 4, 6, 8, 10 };
	int val[5] = { 6, 5, 9, 11, 12 };

	int numItems = 5;

	std::string seq1 = "abcdefg";
	std::string seq2 = "abdcfge";

	int wi = weightedInterval(start, end, val, numItems);
	int knap = knapSack(weight, value, numItems);
	int lcs = lcsubsequence(seq1, seq2);

	printf("\nWeighted Interval : %d", wi);
	printf("\n0-1 Knapsack : %d", knap);
	printf("\nLCS # : %d", lcs);

	int x;
	std::cin >> x;

	return 0;
}