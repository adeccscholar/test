
#include "aoc_lib.h"
#include <utility>
#include <array>
#include <vector>
#include <fstream>

namespace aoc2023_day1 {

namespace fs = std::filesystem;

using number_as_text_ty = std::array<std::string, 10>;

/// type to representate the position of a number in text
using position_text_ty = std::pair<size_t, size_t>;

number_as_text_ty number_as_text = { "null"s, "one"s, "two"s, "three"s, "four"s, "five"s, "six"s, "seven"s, "eight"s, "nine"s };

std::string replaceText(std::string_view input, number_as_text_ty const& replacements) {
   std::string ret = { input.data(), input.size() };
   position_text_ty position_first{ std::string::npos, 0 };
   position_text_ty position_last{ std::string::npos, 0 };
   for (size_t i = 0; i < replacements.size(); ++i) {
      if (size_t pos = ret.find(replacements[i]); pos != std::string::npos) {
         if (position_first.first == std::string::npos || pos < position_first.first) {
            position_first.first = pos;
            position_first.second = i;
            }
         }
      if (size_t pos = ret.rfind(replacements[i]); pos != std::string::npos) {
         if (position_last.first == std::string::npos || pos > position_last.first) {
            position_last.first = pos;
            position_last.second = i;
            }
         }
      }

   if (position_last.first != std::string::npos) {
      std::string replace_text(1, static_cast<char>(position_last.second) + '0');
      ret.replace(position_last.first, replacements[position_last.second].size(), replace_text);
      }

   if (position_first.first != std::string::npos && position_first.first != position_last.first) {
      std::string replace_text(1, static_cast<char>(position_first.second) + '0');
      ret.replace(position_first.first, replacements[position_first.second].size(), replace_text);
      }
   return ret;
   }


template <EPart_of_Riddle part>
void Solution(my_lines input, bool verbose) {
   std::cout << part << ".\n";

   try {
      int sum = 0;
      size_t cnt = 0;
      
      for (auto const& value : input) {
         auto row = [&]() {
                           if constexpr (part == EPart_of_Riddle::Part1) return value;
                           else if (part == EPart_of_Riddle::Part2) return replaceText(value, number_as_text);
                           // else error with always_false and static_assert
                          }();

         if (row.size() > 0) {
            auto pos = std::make_pair(row.find_first_of(strNumbers1), row.find_last_of(strNumbers2));
            if (pos.first == std::string::npos || pos.second == std::string::npos)
               throw std::runtime_error("unexpected input in row ["s + std::string(row.data(), row.size()) + "]"s);
            ++cnt;

            auto value = 10 * (row[pos.first] - '0') + row[pos.second] - '0';
            sum += value;
            if(verbose) std::cerr << std::setw(4) << cnt << ": " << row << " -> " << value << '\n';
            }
         }
      std::cout << "sum of all of the calibration values = " << sum << '\n';
      }
   catch (std::exception& ex) {
      std::cerr << "error in program for day 1, " << ex.what() << '\n';
      }
   }


void Riddle(int part, my_lines input, bool verbose) {
   std::cout << "1st Day, ";
   switch(part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 1st day.");
      }
   }

}