#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

void fill_with_random_ints(vector<int>& array, int n) {
  // fill
  for (int i = 0; i < n; i++)
    array[i] = i;

  // shuffle
  for (int i = 0; i < n; i++)
  {
    int j = rand() % n;
    // swap
    int ai = array[i];
    int aj = array[j];
    array[i] = aj;
    array[j] = ai;
  }
};

int is_sorted(vector<int>& array, int n) {
  int prev = array[0];
  for (int i = 1; i < n; i++)
  {
    int curr = array[i];
    if (prev > curr) return 0;
    prev = curr;
  }

  return 1;
}

// 10^9 - 2 min 10 sec

int main() {
  int size = 1000000000;
  vector<int> array(size);

  fill_with_random_ints(array, size);

  sort(array.begin(), array.end());

  if (is_sorted(array, size)) cout << "sorted" << endl;
  else cout << "not sorted :(" << endl;

  return 0;
}
