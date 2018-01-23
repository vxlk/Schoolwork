#ifndef STRINGSET_H /* Prevent multiple inclusion... */
#define STRINGSET_H

#include <string>
using namespace std;

struct Node {
  string key;
  Node *next;
  Node(string k, Node *n) { key = k; next = n; } //1 line insertion at the beginning of a list
  Node() { key = ""; next = NULL; }
};

class Stringset {

 private: 
  Node **table;  // array of pointers to linked lists
  int size;      // size of table, as currently allocated
  int num_elems; // number of elements stored in the table

 public:
  Stringset();
  ~Stringset();
  bool find(string key);
  void insert(string key);
  void remove(string key);
  void print(void);
};

#endif
