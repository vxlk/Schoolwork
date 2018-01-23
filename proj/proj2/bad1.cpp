// This is a sketch you can use for your "bad1" .. "bad3" programs...

#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 2) {
    cout << "Usage: bad1 <input size>\n";
    return 0;
  }

  int N = atoi(argv[1]);  // get first command-line argument
  if (N<1 || N>100000) {
    cout << "Invalid input size!\n";
    return 0;
  }
  
  // Generate and print bad input of size N for prog1
  // (currently just generates an input of N random numbers)
    int start = N - 1;
		int end = N;
		int u = N-1;
		int *A = new int[N];

    cout << N << "\n";

		for (int i = 0; i < N/2; i++) {
			A[i] = start;
			A[u] = end;
			u--;
			start -= 2;
			end -= 2;
		}

		for (int i = 0; i < N; i++)
			cout << A[i] << "\n";
  
  return 0;
}
