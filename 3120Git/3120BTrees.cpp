#include <iostream>
#include <algorithm>
#include <stack>
#include <queue>
using namespace std;
struct node
{
	node()		{ this->key = 0; this->height = 1; left = NULL; right = NULL; }
	node(int k) { this->key = k; this->height = 1; left = NULL; right = NULL; }
	int sum = 0;
	int key;
	int height;
	node* left;
	node* right;

	bool isThreaded = false;
};

node* insert(node*, int);

node* turnTreeToDoubleThreaded(node* head)
{
	node* curr = head;

	return curr;
}

node* turnTreeToSingleThreaded(node* head, node* parent)
{
	turnTreeToSingleThreaded(head->left, head);
	//process left
	if (!head->right) head->right = parent;
	turnTreeToSingleThreaded(head->right, head);
	if (!head->left && !head->right) head->left = parent;
}

node* trimTreeNoDelete(node* head, int high, int low)
{
	if (!head) return NULL;

	std::stack<node*> _stack;
	node* curr = head;
	node* newTree = NULL;
	do
	{
		while (curr)
		{
			if (curr->right)
				_stack.push(curr->right);
			_stack.push(curr);
			curr = curr->left;
		}

		//get a node
		curr = _stack.top();
		_stack.pop();

		//do right first
		if (!_stack.empty() && curr->right && _stack.top() == curr->right)
		{
			_stack.pop();
			_stack.push(curr);
			curr = curr->right;
		}

		else
		{
			//process
			if (curr->key > low && curr->key < high)
			{
				curr->left = NULL;
				curr->right = NULL;
				newTree = insert(curr, curr->key);
			}
			curr = NULL;
		}

	} while (!_stack.empty());
	return newTree;
}

void trimTree(node* head, int high, int low)
{
	if (!head) return;

	std::stack<node*> _stack;
	node* curr = head;

	do
	{
		while (curr)
		{
			if (curr->right)
				_stack.push(curr->right);
			_stack.push(curr);
			curr = curr->left;
		}

		//get a node
		curr = _stack.top();
		_stack.pop();

		//do right first
		if (!_stack.empty() && curr->right && _stack.top() == curr->right)
		{
			_stack.pop();
			_stack.push(curr);
			curr = curr->right;
		}

		else
		{
			//process
			if (curr->key < low || curr->key > high)
			{
				//three cases

				//leaf node
				if (!curr->left && !curr->right) delete curr;
				//1 child
				else if (!curr->right)
				{
					node* temp = curr->left;
					delete curr->left;
					curr = temp;
				}

				else if (!curr->left)
				{
					node* temp = curr->right;
					delete curr->right;
					curr = temp;
				}
				//2 children
				else
				{
					//get successor
					node* succ = curr;
					if (succ->right)
					{
						succ = succ->right;
						while (succ)
							succ = succ->left;

						node* temp = succ;
						delete succ;
						curr = temp;
					}
		
				}
			}
			curr = NULL;
		}

	} while (!_stack.empty());
}

void toSumTree(node* head)
{
	if (!head) return;

	std::stack<node*> _stack;
	node* curr = head;

	do
	{
		while (curr)
		{
			if (curr->right)
				_stack.push(curr->right);
			_stack.push(curr);
			curr = curr->left;
		}

		//get a node
		curr = _stack.top();
		_stack.pop();

		//do right first
		if (!_stack.empty() && curr->right && _stack.top() == curr->right)
		{
			_stack.pop();
			_stack.push(curr);
			curr = curr->right;
		}

		else
		{
			//process
			if (!curr->left && !curr->right) 
				curr->sum += curr->key;
			else if (!curr->left && curr->right) 
				curr->sum += curr->right->sum + curr->key;
			else if (curr->left && !curr->right) 
				curr->sum += curr->left->sum + curr->key;
			else 
				curr->sum += curr->left->sum + curr->right->sum + curr->key;
			curr = NULL;
		}

	} while (!_stack.empty());
}

void toNodeCounterTree(node* head)
{
	//iterative post order logic
	if (!head) return;

	std::stack<node*> _stack;
	node* curr = head;

	do
	{
		//go all the way left, remember to null the end so you skip past this the second time
		while (curr)
		{
			if (curr->right) _stack.push(curr->right);
			_stack.push(curr);
			curr = curr->left;
		}

		//get a node out to work on
		curr = _stack.top();
		_stack.pop();

		if (!_stack.empty() && curr->right && _stack.top() == curr->right)
		{
			_stack.pop();
			_stack.push(curr); //have to put the head back, so you know where to go next
			curr = curr->right;
		}

		else
		{
			//process
			if (!curr->left && !curr->right) curr->sum = 0;
			else if (!curr->left && curr->right) curr->sum = curr->right->sum + 1;
			else if (curr->left && !curr->right) curr->sum = curr->left->sum + 1;
			else curr->sum = curr->left->sum + curr->right->sum + 2;
			curr = NULL;
		}

	} while (!_stack.empty());

}

void iterativeInorder(node* head)
{
	if (!head) return;
	std::stack<node*> _stack;
	node* curr = head;
	node* left = head;
	node* right = head;

	do
	{
		//pre init
		if (curr)
		{
			//fill right's temp stack
			std::stack<node*> rightStack;
			while (right)
			{
				rightStack.push(right);
				right = right->right;
			}

			//empty right's stack in descending order
			while (!rightStack.empty())
			{
				int temp = rightStack.top()->key;
				_stack.push(rightStack.top());
				rightStack.pop();
			}

			//left's is already in descending order, no need for a temp stack
			//head was already processed
			if (left) left = left->left;
			while (left)
			{
				if (left->right) _stack.push(left->right);
				_stack.push(left);
				left = left->left;
			}
		}

		//since you aren't peeking @ the stack, you don't need to check if the stack is valid, just get the next node
		//get a node to work with
		curr = _stack.top();
		_stack.pop();

		//process
		printf("%d ", curr->key);
		curr = NULL;

	} while (!_stack.empty());
}

void iterativePreOrder(node* head)
{
	if (!head) return;

	std::stack<node*> _stack;
	node* curr = head;
	_stack.push(curr);
		while (!_stack.empty())
		{
			curr = _stack.top();
			_stack.pop();
			//do processing first
			printf("%d ", curr->key);

			//your stack logic was backwards ... remember first to pushed, last to be popped
			if (curr->right) _stack.push(curr->right);
			if (curr->left) _stack.push(curr->left);
		}
}

void iterativePostOrder(node* head)
{
	//don't use recursion
	if (!head) return;
	//go all the way left, keep track of parent and right
	std::stack<node*> _stack;
	node* curr = head;

	//main loop
	do
	{
		//fill the stack, get to leftmost node
		while (curr)
		{
		if (curr->right)
			_stack.push(curr->right);
		_stack.push(curr);
		curr = curr->left;
		}

		//get a node to work with
		curr = _stack.top();
		_stack.pop();


		if (!_stack.empty() && curr->right &&_stack.top() == curr->right)
		{
			_stack.pop();
			_stack.push(curr); //keep track of the parent, move right
			curr = curr->right;
		}

		else
		{
			//do your processing here
			printf("%d ", curr->key);
			//dont forget to reset, so you can skip the stack building the second time
			curr = NULL;
		}

	} while (!_stack.empty());

}

void preOrderSum(node* head)
{
	if (!head) return;
	printf("%d\n", head->sum);
	preOrderSum(head->left);
	preOrderSum(head->right);
}

// A utility function to get the height of the tree 
int height(struct node* head)
{
	if (head == NULL)
		return 0;
	return head->height;
}

int balance(node* head)
{
	if (head == NULL) return 0;
	return height(head->left) - height(head->right);
}

//---------------------------------------------------------------
/*
struct node *rightRotate(struct node *y)
{
	struct node *x = y->left;
	struct node *T2 = x->right;

	// Perform rotation 
	x->right = y;
	y->left = T2;

	// Update heights 
	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	// Return new root 
	return x;
}

// A utility function to left rotate subtree rooted with x 
// See the diagram given above. 
struct node *leftRotate(struct node *x)
{
	struct node *y = x->right;
	struct node *T2 = y->left;

	// Perform rotation 
	y->left = x;
	x->right = T2;

	//  Update heights 
	x->height = max(height(x->left), height(x->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;

	// Return new root 
	return y;
}
//*/

//----------------------------------------------------------------------------

///*
node* leftRotate(node* head)
{
	node* y = head->right;
	
	node* T2 = y->left;

	//swap
	y->left = head;
	head->right = T2;

	//update height
	y->height = std::max(height(y->left), height(y->right)) + 1;
	//since head gained a subtree, it needs height adjusted as well
	head->height = std::max(height(head->left), height(head->right)) + 1;

	return y;
}

node* rightRotate(node* head)
{
	node* y = head->left;
	node* T2 = y->right;

	//swap
	y->right = head;
	head->left = T2;

	//readjust size
	y->height = std::max(height(y->left), height(y->right)) + 1;
	head->height = std::max(height(head->left), height(head->right)) + 1;

	return y;
}
//*/
void preOrder(node* root)
{
	if (root != NULL)
	{
		printf("%d ", root->key);
		preOrder(root->left);
		preOrder(root->right);
	}
}

node* insertNotRecursive(node* head, int k)
{
	if (!head) return new node(k);
	if (head->key == k) return head;

	node* curr = head;
	node* parent = head;

	while(curr)
	{
		if (curr->key == k) return head; //skip duplicates
		parent = curr;
		curr->key > k ? curr = curr->left : curr = curr->right;
	}	

	parent->key > k ? parent->left = new node(k) : parent->right = new node(k);

	head->height = std::max(height(head->left), height(head->right)) + 1;

	int bal = balance(head);

	//check for all balance cases, update with appropriate rotations
	//left left
	if (bal > 1 && k < head->left->key)
		return rightRotate(head);
	//left right
	if (bal > 1 && k > head->right->key)
	{
		head->left = leftRotate(head->left);
		return rightRotate(head);
	}
	//right left
	if (bal < -1 && k < head->right->key)
	{
		head->right = rightRotate(head->right);
		return leftRotate(head);
	}
	//right right
	if (bal < -1 && k > head->right->key)
		return leftRotate(head);

	return head;
}

node* insert(node* head, int k)
{
	if (!head) return new node(k);
	if (head->key == k) return head;
	/*
	if (head->left)
		if (k < head->left->key) head->left = insert(head->left, k);
	else
		head->right = insert(head->right, k);
		*/
	k < head->key ? head->left = insert(head->left, k) : head->right = insert(head->right, k);

	//AVL CODE ---------------

	head->height = std::max(height(head->left), height(head->right)) + 1;

	//check the balance
	int bal = balance(head);

	//left left
	if (bal > 1 && head->left->key > k)
	{
		return rightRotate(head);
	}
	//left right
	if (bal > 1 && head->left->key < k)
	{
		head->left = leftRotate(head->left);
		return rightRotate(head);
	}

	//right left
	if (bal < -1 && head->right->key > k)
	{
		head->right = rightRotate(head->right);
		return leftRotate(head);
	}
	//right right
	if (bal < -1 && head->right->key < k)
	{
		return leftRotate(head);
	}
	//------------------------
	return head;
}

//Entry Point
int main()
{
	struct node *root = NULL;

	/* Constructing tree given in the above figure */
	bool recurse = true;

	if (recurse)
	{
		root = insert(root, 10);
		root = insert(root, 20);
		root = insert(root, 30);
		root = insert(root, 40);
		root = insert(root, 50);
		root = insert(root, 25);
	}
	
	else
	{
		root = insertNotRecursive(root, 10);
		root = insertNotRecursive(root, 20);
		root = insertNotRecursive(root, 30);
		root = insertNotRecursive(root, 40);
		root = insertNotRecursive(root, 50);
		root = insertNotRecursive(root, 25);
	}

	/* The constructed AVL Tree would be
	30
	/  \
	20   40
	/  \     \
	10  25    50
	*/

	printf("Preorder traversal of the constructed AVL"
		" tree is \n");
	preOrder(root);
	printf("\nSUM traversal of the constructed AVL"
		" tree is \n");
	toSumTree(root);
	preOrderSum(root);
	printf("\nNode Counter traversal of the constructed AVL"
		" tree is \n");
	toNodeCounterTree(root);
	preOrderSum(root);
	printf("\nPreorderI traversal of the constructed AVL"
		" tree is \n");
	iterativePreOrder(root);
	printf("\nPostorderI traversal of the constructed AVL"
		" tree is \n");
	iterativePostOrder(root);
	printf("\nInorderI traversal of the constructed AVL"
		" tree is \n");
	iterativeInorder(root);
	printf("\nTrimmed traversalND of the constructed AVL"
		" tree is \n");
	node* newNode = trimTreeNoDelete(root, 40, 25);
	iterativeInorder(newNode);
	printf("\nTrimmed traversal of the constructed AVL"
		" tree is \n");
	trimTree(root, 40, 25);
	iterativeInorder(root);
	int x;
	std::cin >> x;
	return 0;
}