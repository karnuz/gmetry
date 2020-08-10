#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>
using namespace std;



template <typename T>
T minn(T a, T b) {
  return (a>b)?b:a;
}

template <typename T>
T maxx(T a, T b) {
  return (a>b)?a:b;
}

template <typename T>
void swap(int i, int j, vector<T> &v) {
  T temp = v[i];
  v[i] = v[j];
  v[j] = temp;
}


#endif
