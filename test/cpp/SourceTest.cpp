/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

int main() {
  TestUtil test = TestUtil("Source");

  int n = 0;




  std::cout << "Source.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
