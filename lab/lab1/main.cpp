#include <string>
#include <iostream>
#include "intset.h"
/*
code to test that the program is inserting and maintaining order correctly.
*/

using namespace std;

int main(void)
{
  Intset I;
  string command;
  int key;

  while(1) {
	  std::cout << "insert, remove, find, print & quit" << std::endl;
    cin >> command;
    if (cin.eof()) break;

    if (command == "insert") {

      cin >> key;
      if (I.find(key))
	cout << "Error!  Key " << key << " already in structure!\n";
      else
	I.insert(key);

    } else if (command == "remove") {
      
      cin >> key;
      if (!I.find(key)) 
	cout << "Error!  Key " << key << " not in structure!\n";
      else
	I.remove(key);

    } else if (command == "find") {

      cin >> key;
      if (I.find(key))
	cout << "Key " << key << " present.\n";
      else
	cout << "Key " << key << " absent.\n";

    } else if (command == "print") {

      cout << "Contents of structure in sorted order:\n";
      I.print();

    } else if (command == "quit") {

      break;

    } else {

      cout << "Error!  Unknown command '" << command << "'!\n";

    }
  }

  return 0;
}
