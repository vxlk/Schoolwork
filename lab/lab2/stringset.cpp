#include <iostream>
#include <string.h>
#include <assert.h>
#include "stringset.h"

using namespace std;

/* Return a hash for the string s in the range 0..table_size-1 */
int hashs(string s, int table_size)
{
  unsigned int i, h = 0;
  for (i=0; i<s.length(); i++)
    h = (h * 2917 + (unsigned int)s[i]) % table_size;
  return h;
}

/* Allocate a table of pointers to nodes, all initialized to NULL */
Node **allocate_table(int size)
{
  Node **table = new Node *[size];
  for (int i=0; i<size; i++)
    table[i] = NULL;
  return table;
}

Stringset::Stringset()
{
  size = 4; // initial size of table    
  table = allocate_table(size);
  num_elems = 0; 
}

Stringset::~Stringset()
{
  for (int i=0; i<size; i++) {
    while (table[i] != NULL) {
      Node *temp = table[i];
      table[i] = table[i]->next;
      delete temp;
    }
  }
  delete[] table;
}

/* Return true if key is in the set */
bool Stringset::find(string key)
{
  int h = hashs(key, size);
  Node *n = table[h];
  while (n != NULL) {
    if (n->key == key) return true;
    n = n->next;
  }
  return false;
}

/* Inserts a new key.  It is an error if key is already in the set. */
void Stringset::insert(string key)
{
  assert (!find(key));
  num_elems++;

  if (num_elems == size) {
    
	  int newSize = size * 2;
	  Node** newArr = allocate_table(newSize);
	  for (int i = 0; i < size; i++) {
		  Node* curr = table[i];
		  if (curr != NULL) {
			  int newh = hashs(curr->key, newSize); //get new hash
			  newArr[newh] = new Node(curr->key, newArr[newh]); //insert
		
			  //check linked nodes
			  while (curr->next) {
				  curr = curr->next; //traverse
				  int newh = hashs(curr->key, newSize); //get new hash
				  newArr[newh] = new Node(curr->key, newArr[newh]); //insert
			  }//while
		  }//if
		}//for
	
	 delete[] this->table;
	 this->table = NULL;
	 this->table = allocate_table(newSize);
	 this->size = newSize;
	 this->table = newArr; //set the keys = to the newly made array

  }
  
 table[ hashs(key, size) ] = new Node( key, table[ hashs(key, size) ] );
}

/* Removes a key.  It is an error if key isn't in the set */
void Stringset::remove(string key)
{

	Node* prev = NULL;

  assert (find(key));

			int h = hashs(key, size);

			Node* curr = table[h];

			if (curr != NULL) {

				if (curr->key == key && prev == NULL && curr->next == NULL) { //only one in set
					curr->key.clear();
					curr = NULL;
					return;
				}
				if (curr->key == key && prev == NULL && curr->next != NULL) { //first in the set, link to next
					curr->key.clear();
					curr = curr->next;
					delete curr;
					return;
				}

				if (curr->key == key && prev != NULL) { //in the middle somewhere, handles end as well
					prev->next = curr->next;
					curr->key.clear();
					delete curr;
					return;
				}
				curr = curr->next;
			}
			else return;
  num_elems--;
}

void Stringset::print(void)
{

		for (int i = 0; i < size; i++) {
			Node* temp = table[i];
			if (temp != NULL) {
				std::cout << temp->key << std::endl;
				while(temp->next) {
					temp = temp->next;
					std::cout << temp->key << std::endl;
				}
			}
		} //end for
}//end function
