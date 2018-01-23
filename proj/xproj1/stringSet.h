#ifndef STRINGSET_H /* Prevent multiple inclusion... */
#define STRINGSET_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct idStruct {
	int index;
	idStruct *next;
	idStruct(int ind, idStruct *n) { index = ind; next = n; }
	idStruct() { index = 0; next = NULL; }
};

struct Node {
	string key;
	Node *next;
	Node(string k, Node *n) { key = k; next = n; count = 1; id = NULL; }
	Node() { key = ""; next = NULL; count = 0; id = NULL; }
	int count;
	double weight;
	double new_weight;
	idStruct *id;
	//idStruct tempID[100];
};


class Stringset {

private:

	Node **table;  // array of words
	Node **webPages; //array of webpages
	Node **iIndex; //array of words with pointers to links
	int size;      // size of table, as currently allocated
	int num_elems; // number of elements stored in the table
	int wordCount;

public:

	inline int getSize() { return size; }
	Stringset();
	~Stringset();
	void readFile();
	bool find(string key, Node** table);
	void insert(string key, int, Node**);
	void insertWord(string key, Node** table);
	void insertHead(string key, Node**, int size);
	void print(void);
	int& operator[](string key);
	void pageRank();
	void invertedIndex();
	void insertID(int id, idStruct*);

};


#endif