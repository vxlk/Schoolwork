// This is a sketch you can use for your "bad1" .. "bad3" programs...

#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <assert.h>
using namespace std;

struct Node {
	int key;
	int size;
	Node *left;
	Node *right;
	Node(int k) { key = k; size = 1; left = right = NULL; }
};

void fix_size(Node *T)
{
	T->size = 1;
	if (T->left) T->size += T->left->size;
	if (T->right) T->size += T->right->size;
}

void split(Node *T, int r, Node **L, Node **R)
{
	if (T == NULL) {
		*L = NULL;
		*R = NULL;
		return;
	}

	if (r < T->size) {
		split(T->left, r, L, &T->left);
		*R = T;
		//fix size
	}

	else {
		split(T->right, r, &T->right, R);
		*L = T;
		//fix size
	}

	fix_size(T);
}

Node *insert_keep_balanced(Node *T, int v, int r)
{
	// If k is the Nth node inserted into T, then:
	// with probability 1/N, insert k at the root of T
	// otherwise, insert_keep_balanced k recursively left or right of the root of T
	if (T == NULL) return new Node(v);
	if (rand() % (T->size + 1) == 0) {
		// insert at root
		Node *new_root = new Node(v);
		split(T, r, &new_root->left, &new_root->right);
		fix_size(new_root);
		return new_root;
	}
	int rank;

	if (!T->left) rank = 0;
	else rank = T->left->size;

	if (r == T->size) T = insert_keep_balanced(T, v, r);
	else if (r < T->size) T->left = insert_keep_balanced(T->left, v, r);
	else T->right = insert_keep_balanced(T->right, v, r - rank - 1);
	fix_size(T);
	return T;
	// otherwise insert recursively...
	// TBD: you'll need to fill in this part
}

// insert value v at rank r
Node *insert(Node *T, int v, int r)
{

	if (T == NULL) return new Node(v);
	int rank_of_root = T->left ? T->left->size : 0;
	if (r <= rank_of_root) T->left = insert(T->left, v, r);
	else T->right = insert(T->right, v, r - rank_of_root - 1);
	fix_size(T);
	return T;
	/*
	int rank;

	if (T == NULL) return new Node(v);

	if (!T->left) rank = 0;
	else rank = T->left->size;

	//traverse
	if (rank > r || rank == r) T->left = insert(T->left, v, r);
	else T->right = insert(T->right, v, r - rank - 1);
	fix_size(T);
	return T;
	//if (T->left) T->left = insert(T->left, T->left.key, T->left.size);
	// TBD: fill this part in
*/
}

Node *select(Node *T, int r)
{
	assert(T != NULL && r >= 0 && r<T->size);

	int rank_of_root = T->left ? T->left->size : 0;
	if (r == rank_of_root) return T;
	if (r < rank_of_root) return select(T->left, r);
	else return select(T->right, r - rank_of_root - 1);
}




int get_rand(int N)
{
	// I forgot to attend the lecture where Prof Dean. explained how to generate
	// random numbers well, so hopefully this will work ok...
	// It looks like get_rand(N) always returns the same number if you call
	// it multiple times with the same value of N, but how could someone 
	// possibly exploit that to make quicksort run slowly...
	return 123456789 % N;
}

Node* turnQuickSortIntoBubbleSort(Node* A, int len, int i) {
	int piv = get_rand(len);

	if (len <= 1) {
		A = insert(A, i, piv);
		return A;
	}
	else {
		turnQuickSortIntoBubbleSort(A, len - 1, i + 1);
		A = insert(A, i, piv);
		return A;
	}
}

void print_inorder(Node *T)
{
	if (T == NULL) return;
	print_inorder(T->left);
	cout << T->key << "\n";
	print_inorder(T->right);
}


int main(int argc, char *argv[]) {

	if (argc != 2) {
    cout << "Usage: bad1 <input size>\n";
    return 0;
  }

  int N = atoi(argv[1]);  // get first command-line argument
  if (N<1 || N>100000) {
    cout << "Invalid input size!\n";
    return 0;
  }

	Node* A = NULL;

//	for (int i = 0; i < N; i++)
//		A = insert(A, i, i);


	int c = 0; //init to start
	A = turnQuickSortIntoBubbleSort(A, N, c);

	cout << N << "\n";
	print_inorder(A);

	return 0;
}
