#pragma once
#include <cstdint>
#include <cstddef>





static int scalar_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  for(size_t i = startindex; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}

size_t scalar_findall(const char * stream, size_t length, char target) {
  int i = scalar_findfirst(stream, 0, length, target);
  size_t sum = 0;
  while(i != -1) {
    sum ++;
    i = scalar_findfirst(stream, i + 1, length, target);
  }
  return sum;
}



static int strchr_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  const char * currentpos = strchr(stream + startindex, target);
  if(currentpos != NULL) {
    return currentpos - stream;
  }
  return -1;
}

size_t strchr_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = strchr_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = strchr_findfirst(stream, t + 1, length, target);
  }
  return sum;
}
