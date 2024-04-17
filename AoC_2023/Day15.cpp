#include "aoc_lib.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <ranges>
#include <algorithm>
#include <numeric>

namespace aoc2023_day15 {

   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {

      try {
         std::string_view view = *input.begin();
         std::string text = to_String(view);
         text.erase(std::remove(text.begin(), text.end(), ' '), text.end());
         text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
         if (verbose) std::cerr << text << '\n';

         auto seq = text | std::views::split(',')
            | std::views::transform([](auto v) { return std::string{ v.begin(), v.end() }; })
            | std::ranges::to<std::vector>();

         auto sum = accumulate(seq.begin(), seq.end(), 0ull, [verbose](unsigned long long sum, std::string const& p) {
            auto val = accumulate(p.begin(), p.end(), 0ull, [verbose](unsigned long long sum, char c) {
               sum += static_cast<uint64_t>(c);
               sum *= 17;
               sum = sum % 256;
               return sum;
               });
            if (verbose) std::cerr << "Seq=" << p << " hash= " << val << '\n';
            return sum += val;
            });
         std::cerr << sum << "\n";
         std::cerr << part << ".\n";


         std::cout << "solution = " << sum << '\n';
      }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 15, " << part << ": " << ex.what() << '\n';
      }
   }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "15th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 8th day.");
      }
   }

} // end of namespace
