#include <string>
#include <fstream>
#include <iostream>
#include "stringset.h"

using namespace std;

/*
Entering a word into your terminal will result in a hashed search to all of the applicable words
*/

void spellcheck(void)
{
  Stringset S;
  string word;

  ifstream wordfile;
  wordfile.open("words.txt");
  while (wordfile >> word)
    S.insert(word);
  wordfile.close();

  cout << "Dictionary loaded.  Please enter words to spell check.\n";
  while (cin >> word) {

	  cout << "Possible alternatives for word '" << word << "':\n";

	  for (int i = 0; i < word.length(); i++) {
		  char alphabetIndexer = 'a';
		  while (alphabetIndexer <= 'z' && alphabetIndexer != word[i]) {
			  word[i] = alphabetIndexer;
			  if (S.find(word)) {
				  std::cout << word << "\n";
			  }
		  alphabetIndexer++;
		  }//while
	  }//for
  }//while
}//function

void test(void)
{
  Stringset S;
  string key, command;

  while(cin >> command) {
    
    if (command == "insert") {
      
      cin >> key;
      if (S.find(key))
	cout << "Error!  Key " << key << " already in structure!\n";
      else
	S.insert(key);

    } else if (command == "remove") {
      
      cin >> key;
      if (!S.find(key)) 
	cout << "Error!  Key " << key << " not in structure!\n";
      else
	S.remove(key);

    } else if (command == "find") {

      cin >> key;
      if (S.find(key))
	cout << "Key " << key << " present.\n";
      else
	cout << "Key " << key << " absent.\n";

    } else if (command == "print") {

      cout << "Contents of structure:\n";
      S.print();

    } else if (command == "quit") {

      break;

    } else {

      cout << "Error!  Unknown command '" << command << "'!\n";

    }
  }
}

int main(void)
{
  test();
  spellcheck();
  return 0;
}
