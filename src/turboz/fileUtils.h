#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_
#include <fstream>


inline bool file_exists (const std::string& name) {
  std::ifstream f(name.c_str());
    return f.good();
}


#endif
