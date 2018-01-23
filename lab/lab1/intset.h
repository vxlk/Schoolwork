#ifndef INTSET_H /* Prevent multiple inclusion... */
#define INTSET_H

struct node {
	node* next; //link to the next address
	int value; //key held in each link
};

class Intset {

 private: 
 node *head;

 public:
  Intset();

  //destructor
  ~Intset();

  //find the right position, used to deal with multiple of the same entries
  bool find(int key);
  
  //inserts into the correct position while maintaining order
  void insert(int key);
  
  //removes from position while maintaining order
  void remove(int key);

  //walks through the list and prints the contents
  void print(void);

};

#endif
