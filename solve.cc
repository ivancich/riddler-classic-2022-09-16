// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab ft=cpp


#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <numeric>


using Dictionary = std::multimap<std::string, std::string>;
using WordStack = std::vector<std::string>;


// returns an encoded version of the word where all letters are sorted
std::string encode(const std::string& orig) {
  std::string result = orig;
  std::sort(result.begin(), result.end());
  return result;
}


// returns a reduced encoded string by removing the which'th unique
// letter; if it cannot find a which'th unique letter, it returns
// false; otherwise it returns true upon success
bool remove(const std::string& encoded, int which, std::string& out) {
  out = encoded;
  auto i = out.begin();
  if (i == out.end()) {
    return false;
  }

  while (which > 0) {
    char here = *i;
    if (++i == out.end()) {
      return false;
    }
    char there = *i;
    if (here != there) {
      which--;
    }
  }

  out.erase(i);
  return true;
}


// read in the data file and load it into the dictionaryx
void read(const std::string& fname, Dictionary& codes) {
  std::ifstream f(fname, std::ifstream::in);
  if (!f.good()) {
    std::cerr << "could not read " << fname << std::endl;
    exit(1);
  }
  std::string line;
  while(f.good()) {
    std::getline(f, line);
    if (line.size() >= 4) {
      std::string encoded = encode(line);
      codes.insert(std::pair{ encoded, line });
    }
  }
  f.close();
}


// check a word
void check(const Dictionary& dict,
	   const std::string& encoded,
	   WordStack& stack) {
  {
    int count = 0;
    std::string build;
    auto its = dict.equal_range(encoded);
    for (auto i = its.first; i != its.second; ++i) {
      ++count;
      if (count > 1) {
	build += ", ";
      }
      build += i->second;
    }
    if (count > 1) {
      build = "{ " + build + " }";
    }
    if (count == 0) {
      return;
    }
    stack.push_back(build);
  }

  if (encoded.size() <= 4) {
    std::string s = std::accumulate(stack.crbegin(), stack.crend(), std::string(),
				    [](std::string &ss, const std::string &s)
				    {
				      return ss.empty() ? s : ss + ", " + s;
				    });
    std::cout << s << std::endl;
  } else {
    for (int which = 0; ; ++which) {
      std::string reduced;
      if (!remove(encoded, which, reduced)) {
	break; // not found
      }
      check(dict, reduced, stack);
    }
  }

  stack.pop_back();
}


int main(const int argc, const char* argv[]) {
  int size = 9;
  if (argc > 1) {
    size = atoi(argv[1]);
  }

  Dictionary dict;
  read("../../norvig-words.txt", dict);
  std::cerr << "dictionary has size " << dict.size() << std::endl;

  std::string previous;
  WordStack stack;
  for (auto& i : dict) {
    if (i.first.size() == size) {
      if (i.first == previous) {
	continue;
      } else {
	previous = i.first;
      }
      check(dict, i.first, stack);
    }
  }

  std::cerr << "done" << std::endl;
}
