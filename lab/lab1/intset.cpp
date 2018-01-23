#include <iostream>
#include <assert.h>
#include "intset.h"


/*
Most of the modification to the code is pretty trivial -- just add a comparison based on the ID so you only walk through the list until you get to a node with an ID larger then the new one you need to insert (or reach the end of the list).

This is where things get slightly tricky: before you "realize" you've reached the right spot in the list, you've already gone one node too far (and in a singly linked list, there's no way to go back). The trick to fix that is pretty simple: allocate a new (empty) node and insert it after the too-large node you found. Copy that too-large node's contents into the new one you just inserted, and then copy the data for the new node into the spot it just vacated.

I should add, however, that all of this is mostly a moot point. If you want a sorted collection of items, a linked list is usually a really lousy choice. Unless you're doing something like homework where you have no choice but to do whatever brain-dead crap you've been assigned, look up std::set [Edit: or std::multiset, if duplicates are allowed -- or possibly std::map or std::multimap, if you want to be able to find a node based on an ID] and forget about implementing it yourself.
*/



using namespace std;

Intset::Intset()
{
	head = NULL;
}

Intset::~Intset()
{
	
	if (!head) {
		return; //nothing to deallocate 
	}
	
	else { //deleting 
		node* temp;
		while (head != NULL)
		{
			temp = head->next;
			delete head;
			head = temp;
		}
		temp = NULL; 
	}
	
}

/* Return true if key is in the set */
bool Intset::find(int key)
{
	node* current = head;  // Initialize current
	while (current != NULL)
	{
		if (current->value == key)
			return true;
		current = current->next;
	}
	return false;

}

/* Inserts a new key.  It is an error if key is already in the set. */
void Intset::insert(int key)
{
 
  assert (!find(key));
  //node* temp = new node;
  node* pre = NULL;
  node* current = new node;
  
  //make head
  if (!head) {
	  node* temp = new node;
	  temp->value = key;
	  temp->next = head;
	  head = temp;
	  return;
  }

  current = head;

  while (current) {

	  if (key < current->value) {   //it cant handle numbers bigger than the previous...

		  //found insertion point
		  node* temp = new node;
		  temp->value = key;
		  //put behind the one we are looking at
		  temp->next = current;
		  
		  

		  if (!pre) {
			  //you're at the beginning
			  head = temp;
		  }

		  else {
			  //add in front of previous node
			  pre->next = temp;

		  } //end if 
	  return;
	  }
		pre = current; //set existing location before doing anything..
		current = current->next; //the scouter
	  
  } //end while

		node* temp1 = new node;
		temp1->value = key;
		pre->next = temp1;
		temp1->next = current; //add a link behind the current
  
}

/* Removes a key.  It is an error if key isn't in the set */
void Intset::remove(int key)
{

  assert (find(key));

  node* pre = new node;
  node* current = new node;

  current = head;

  if (head == NULL) {
	  return; //the list is empty
  }

  //check head before traversing...
  if (current->value == key){
		  //remove the head
		  node *temp = new node;
		  temp = head;
		  head = head->next;
		  delete temp;
		  return;
	  }

  //find function guaruntees the # is in the list, so we can be more sloppy
  while (current->value != key) {

	   if (current->next->next == NULL) { //delete the end
		   delete current->next;
		   current->next = NULL;
		  return;
	  }

	   //traverse
	   pre = current;
	   current = current->next;

	   //found
	   if (key == current->value) {
		   pre->next = current->next;
		   delete current;
		   return;
	   }
  } //end while
  
}

void Intset::print(void)
{
	node *temp = new node;
	temp = head;
	while (temp != NULL) {
		std::cout << temp->value << std::endl;
		temp = temp->next;
	}
}
