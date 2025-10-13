/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>
#include <math.h>

#include "supernovas.h"

using namespace supernovas;

class TestUtil {

public:
  std::string _classname;

  explicit TestUtil(const std::string& classname) : _classname(classname) {}

  bool equals(const std::string& funcname, double a, double b, double precision = 0.0) {
    if(fabs(a-b) > fabs(precision)) {
      std::cerr << "ERROR! " << _classname << "::" << funcname << ": " << std::to_string(a) << " != " << std::to_string(b) << "\n";
      return false;
    }
    return true;
  }

  bool differs(const std::string& funcname, double a, double b, double precision = 0.0) {
    if(fabs(a-b) <= fabs(precision)) {
      std::cerr << "ERROR! " << _classname << "::" << funcname << ": " << std::to_string(a) << " != " << std::to_string(b) << "\n";
      return false;
    }
    return true;
  }

  bool equals(const std::string& funcname, long a, long b) {
    if(a != b) {
      std::cerr << "ERROR! " << _classname << "::" << funcname << ": " << std::to_string(a) << " != " << std::to_string(b) << "\n";
      return false;
    }
    return true;
  }

  bool check(const std::string& funcname, bool value) {
    if(!value)
      std::cerr << "ERROR! " << _classname << "::" << funcname << "\n";
    return value;
  }
};



