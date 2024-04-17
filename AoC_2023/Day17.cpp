#include "aoc_lib.h"

#include <iostream>
#include <exception>
#include <stdexcept>

namespace aoc2023_day17 {

   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      std::cout << part << ".\n";

      try {
         std::cout << "solution = " << '\n';
      }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 14, " << part << ": " << ex.what() << '\n';
      }
   }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "14th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 8th day.");
      }
   }

} // end of namespace
