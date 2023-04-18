#include <vector>
#include <algorithm>
#include <iostream>
#include "merge.h"
using namespace std;

vector<int> vector_of_ints(int size) {
  vector<int> array(size);
  // fill
  for (int i = 0; i < size; i++)
    array[i] = i;

  // shuffle
  for (int i = 0; i < size; i++)
  {
    int j = rand() % size;
    // swap
    int ai = array[i];
    int aj = array[j];
    array[i] = aj;
    array[j] = ai;
  }
  return array;
}

int* array_of_ints(int size) {
  int* array = (int*)malloc(sizeof(int) * size);
  // fill
  for (int i = 0; i < size; i++)
    array[i] = i;

  // shuffle
  for (int i = 0; i < size; i++)
  {
    int j = rand() % size;
    // swap
    int ai = array[i];
    int aj = array[j];
    array[i] = aj;
    array[j] = ai;
  }
  return array;
}

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
  int size = 10000000;
  // vector<int> array = vector_of_ints(size);
  int* array = array_of_ints(size);

  // sort(array.begin(), array.end());
  // general_par_merge_sort(proxy_general_args(array, size, 10, 1000));
  not_main();

  // if (is_sorted(array, size)) cout << "sorted" << endl;
  // else cout << "not sorted :(" << endl;

  return 0;
}
