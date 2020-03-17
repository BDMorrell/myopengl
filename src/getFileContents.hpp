#pragma once
#include <cstddef>  // size_t

struct rawData {
  size_t size;
  char *value; // this should be deleted with the object!
};

struct rawData *getFileContents(const char *fileName); // please delete[] returned value; null returned on error
