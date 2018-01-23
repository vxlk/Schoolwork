#include <iostream>
#include <string.h>
#include <assert.h>
#include "stringSet.h"

//there are two arrays, one that holds webpages
//one that hold words

using namespace std;

/* Return a hash for the string s in the range 0..table_size-1 */
int hashs(string s, int table_size)
{
	unsigned int i, h = 0;
	for (i = 0; i<s.length(); i++)
		h = (h * 291729172917 + (unsigned int)s[i]) % table_size;
	return h;
}

void allocateIDStruct(idStruct*& table){
	table = new idStruct;
}

/* Allocate a table of pointers to nodes, all initialized to NULL */
Node **allocate_table(int size)
{
	Node **table = new Node *[size];
	for (int i = 0; i<size; i++)
		table[i] = NULL;
	return table;
}

int BAD_RESULT = 0;

int& Stringset::operator[](string key) {

	int h = hashs(key, size);
	Node* n = table[h];
	while (n) {
		if (n->key == key) { return n->count; }
		n = n->next;
	}
	return BAD_RESULT;
}

Stringset::Stringset()
{   
	table = allocate_table(size);
	num_elems = 0;
	size = 0;
	wordCount = 500000;
}

Stringset::~Stringset()
{
	//delete table
	for (int i = 0; i<size; i++) {
		while (table[i] != NULL) {
			Node *temp = table[i];
			table[i] = table[i]->next;
			delete temp;
		}
	}
	delete[] table;

	//delete iIndex
	for (int i = 0; i<wordCount; i++) {
		while (iIndex[i] != NULL) {
			Node *temp = iIndex[i];
			iIndex[i] = iIndex[i]->next;
			delete temp;
		}
	}
	delete[] iIndex;

	//delete websites
	for (int i = 0; i<size; i++) {
		while (webPages[i] != NULL) {
			Node *temp = webPages[i];
			webPages[i] = webPages[i]->next;
			delete temp;
		}
	}
	delete[] webPages;
}

/* Return true if key is in the set */
bool Stringset::find(string key, Node** table)
{
	int h = hashs(key, wordCount);
	Node *n = table[h];
	if (n == NULL) return false;
	if (n->key == key)
		return true;
	return false;
}

//dynamically grows Used for iIndex
void Stringset::insertWord(string key, Node** table) {
	//if (find(key)) return;
	//add at tablePosition
	num_elems++;
	if (num_elems == wordCount)
	{
		Node**old_table = table;
		wordCount *= 2;
		table = allocate_table(wordCount);
			for(int i = 0; i<wordCount / 2; i++)
			{
				Node *n = old_table[i];
				while (n != NULL) {
					int h = hashs(n->key, wordCount);
					table[h] = new Node(n->key, table[h]);
					Node *to_delete = n;
					n = n->next;
					delete to_delete;
				}
			}
		//   De-allocate old_table
			delete[]   old_table;
	}
	table[hashs(key, wordCount)] = new Node(key, table[hashs(key, wordCount)]);
}

void Stringset::insert(string key, int position, Node** table) {
	Node* temp = table[position];
	while (temp) {
		if (temp->next == NULL) {
			temp->next = new Node(key, temp->next); //add at the end
			break;
		}
		temp = temp->next;
	}
}

/* Inserts a new ID.  It is an error if key is already in the set. */
void Stringset::insertID(int id, idStruct* table) {

	if (!table) { //meaning it has not been initialized...
		table = new idStruct(id, table); //make head
		return;
	}

	else {
		idStruct *temp = table;
		while (temp) {
			if (temp->next == NULL) {
				temp->next = new idStruct(id, temp->next); //add at the end
				break;
			}
			temp = temp->next;
		}
	}
}

/* Inserts a new key.  It is an error if key is already in the set. */
void Stringset::insertHead(string key, Node** table, int size)
{
	table[hashs(key, size)] = new Node(key, table[hashs(key, size)]);
}


void Stringset::print(void)
{
	//print webpages
	for (int i = 0; i < wordCount; i++) {
		Node* temp = iIndex[i];
		if (temp) {
			cout << temp->key << "\n";
			while (temp->next) {
				temp = temp->next;
				std::cout << temp->key << std::endl;
			}
		}
	} //end for
	
	cout << "\n////////////////////////////////////TABLE\n";
	//print words
	for (int i = 0; i < size; i++) {
		Node* temp = table[i];
		if (temp) {
			cout << temp->key << "\n";
			while (temp->next) {
				temp = temp->next;
				std::cout << temp->key << std::endl;
			}
		}
	} //end for

	cout << "\n////////////////////////////////////webpagges\n";
	//print words
	for (int i = 0; i < size; i++) {
		Node* temp = webPages[i];
		if (temp) {
			cout << temp->key << "\n";
			while (temp->next) {
				temp = temp->next;
				std::cout << temp->key << std::endl;
			}
		}
	} //end for

	
}//end function

void Stringset::readFile() {

	ifstream in;
	string s;
	int b, index;
	in.open("webpages.txt");
	//in.open("C:/Users/Ty/Desktop/proj1/fakewebpages.txt"); //PC
	//in.open("C:/Users/ty/Desktop/labs/proj1/fakewebpages.txt"); //laptop

	if (!in) {
		std::cout << "file not opened\n";
	}

	cout << "\ngetting size...\n";
	//get the size of the array
	while (in >> s) {
		//in >> s;
		if (s == "NEWPAGE")
			size++;
	}
	in.close();
	cout << wordCount;
	table = allocate_table(size);
	webPages = allocate_table(size);
	iIndex = allocate_table(wordCount);

	cout << "\nwebPages and Table...";
	in.open("webpages.txt");
	//in.open("C:/Users/Ty/Desktop/proj1/fakewebpages.txt"); //PC
	//in.open("C:/Users/ty/Desktop/labs/proj1/fakewebpages.txt"); //laptop

	cout << "\nreading words...\n";

	while (in >> s) {

		if (s == "NEWPAGE") { //new webpage encountered
			in >> s;				  //read in the link as the head.. hash to get position
			index = hashs(s, size);
			insertHead(s, webPages, size); //webpages
			insertHead(s, table, size); //sequential list
		}
		else {
			//if https is read in w.o a new page then ignore it
			if (s.find("http://") == string::npos) {                 ///////////////////////////
				//check if it's already in
				if (!find(s, iIndex))
					insertWord(s, iIndex); //insert the word into word array
				b = hashs(s, wordCount);
				if (iIndex[b])
					//insert(table[index]->key, b, iIndex); //insert the url into the word array
					insertHead(table[index]->key, iIndex, wordCount);
					//insert the id into that node's id storage
					//wordCount++;
					//insert into regular list
				//insert(s, index, table);
			}

			//++wordCount;
			//	cout << wordCount << "\n";
			//	cout << size << "\n";


			else if (s.find("http://") != string::npos) {
				//if it hashes to a key in the index of known webpages
				int test = hashs(s, size);
				if (webPages[test]) {
					if (s.compare(webPages[test]->key))
						insert(s, index, table);
					else in.ignore();
					//cout << num_elems << "\n";
				}
			}
		}

		}

	cout << num_elems << "numelemts";
	cout << wordCount;
	in.close();
}

void Stringset::pageRank() {

	for (int k = 0; k < size; k++)	{ //set defaults
		if (webPages[k])
			webPages[k]->weight = (1.0 / size);
	}

	//do this 50 times
	for (int u = 0; u < 50; u++) {
		int t = 0;//keep count of linked web pages
		//loop thru each webpage
		for (int i = 0; i < size; i++) {
			if (webPages[i]) {
			webPages[i]->new_weight = (0.1 / size);
			//for each link on the [i] webpage
			for (Node* temp = webPages[i]; temp != NULL; temp = temp->next) {
				t++; //get total links on page
			}
			for (Node* temptwo = webPages[i]->next; temptwo != NULL; temptwo = temptwo->next) {
				int index = hashs(temptwo->key, size);
				webPages[index]->new_weight += (0.9 * webPages[index]->weight / t); //add the new weight for each link on the list
			}
			webPages[i]->weight = webPages[i]->new_weight;
			} //ends if
		}
	}
	


}

void Stringset::invertedIndex() {

	int index; 

	/*for (int i = 0; i < size; i++) {
		Node* temp = table[i]; //the url     //////////////////////////
		if (table[i]) {
			//get the id of the url the words belong to
			int idStorage = hashs(table[i]->key, size);
			cout << idStorage;
			//if (temp->next)
				//temp = temp->next;
			while (temp) {
				//step down the list, collect the words
				//add the idStorage unit, to that word's collection
				if (temp->key.find("http://") == string::npos) {//skip the link
					index = hashs(temp->key, wordCount); //find the word in iIndex
					if (iIndex[index])
						insert(table[i]->key, index, iIndex);
					}
				 //insert the id into that node's id storage
					
				temp = temp->next;
				}
				//else temp = temp->next;
			}
		} //for
	*/
	//now add the urls if the word appears on their page, back to table
	//the word's id the hashed index of the webpage it belongs to
	//print();	

		int x = 0;
		string s;

		while (s != "quit") {
			cout << "\nEnter a word to be found\n";
			cin >> s;
			int b = hashs(s, wordCount);//hash to find the word in word table
			if (iIndex[b] && iIndex[b]->key == s) {
				Node* traverse = iIndex[b]->next; //skip the word
				while (traverse) { //find all the urls associated with that word
					
					//traverse = traverse->next; //skip word go to urls
					//int lookup = iIndex[b]->id[x];
					//s = table[traverse->index]->key;
					
					int urlpos = hashs(traverse->key, size);
					cout << traverse->key;//get that index to send to webpage structure
					cout << webPages[urlpos]->weight << " " << "\n"; //use webpage structure to get weight
					//traverse = traverse->next; //go to next id
					traverse = traverse->next;
				}
			}
			else cout << "\n not in structure\n";
		}

}
