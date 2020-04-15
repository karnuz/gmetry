#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>
using namespace std;


float min(float a, float b);

float max(float a, float b);

template <typename T>
void swap(int i, int j, vector<T> &v);



float min(float a, float b) {
  return (a>b)?b:a;
}

float max(float a, float b) {
  return (a>b)?a:b;
}

template <typename T>
void swap(int i, int j, vector<T> &v) {
  T temp = v[i];
  v[i] = v[j];
  v[j] = temp;
}

#endif
