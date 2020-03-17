#include "getFileContents.hpp"
#include <fstream>
#include <vector>
#include <cstring>

struct rawData *getFileContents(const char *fileName) {
  // read
  const int maxBufferLength = 1024;
  std::vector<char*> blocks;
  std::ifstream file;
  file.open(fileName);
  if (file.fail()) {
    return NULL;
  }
  while (file.good()) {
    char *block = new char[maxBufferLength];
    file.read(block, maxBufferLength);
    blocks.push_back(block);
  }
  file.close();

  // combine
  struct rawData *contents = new struct rawData;
  contents->size = (blocks.size()-1)*maxBufferLength + file.gcount();
  contents->value =  new char[contents->size + 1]; // +1 for guaranteed \0 at end of contents
  char *outIterator = contents->value; // treat this as a pointer, not an array ;)
  char *outFinish = outIterator + (blocks.size()-1);
  std::vector<char *>::iterator blockIterator = blocks.begin();
  for (; outIterator < outFinish; outIterator++) {
    memcpy(outIterator, *blockIterator, maxBufferLength);
    blockIterator++;
  }
  if (blockIterator != blocks.end() && file.gcount() < maxBufferLength) {
    memcpy(outIterator, *blockIterator, file.gcount());
  }

  // cleanup
  for (blockIterator = blocks.begin(); blockIterator != blocks.end(); blockIterator++) {
    delete[] *blockIterator;
  }

  contents->value[contents->size] = '\0'; // guaranteed \0
  return contents;
}
