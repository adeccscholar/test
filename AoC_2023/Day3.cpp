#include "aoc_lib.h"

#include <string>
#include <stdexcept>
#include <vector>
#include <ranges>
#include <cctype>

namespace aoc2023_day3 {


   void Solution1(my_lines input, bool verbose) {
      std::cout << EPart_of_Riddle::Part1 << '\n';

      auto data = input | std::views::transform([](auto p) { return std::string_view{ p }; })
         | std::ranges::to<std::vector>();
      if(verbose) std::cerr << std::format("{} lines readed.\n", data.size());
      int check_sum = 0;
      for (size_t i = 0; i < data.size(); ++i) {
         if (verbose) std::cerr << std::format("{}\n", data[i]);
         for(auto pos1 = data[i].find_first_of(strNumbers1); pos1 != std::string_view::npos; pos1 = data[i].find_first_of(strNumbers1, pos1)) {
            size_t pos2 = data[i].find_first_not_of(strNumbers2, pos1);
            if (pos2 == std::string_view::npos) pos2 = data[i].size() - 1;
            int iLeft   = pos1 > 0 ? pos1 - 1 : 0;
            int iRight  = pos2 + 1 < data[i].size() ? pos2 + 1 : pos2;

            bool check = false;
            if (i > 0) {
               if(auto pos = data[i-1].substr(iLeft, iRight - iLeft).find_first_not_of("."); pos != std::string_view::npos) {
                  check = true;
                  goto end;
                  }
               }

            if(iLeft  != pos1 && data[i][iLeft]  != '.') { check = true; goto end; }
            if(iRight != pos2 && data[i][iRight - 1] != '.') { check = true; goto end; }

            if(i + 1 < data.size()) {
               if (auto pos = data[i + 1].substr(iLeft, iRight - iLeft).find_first_not_of("."); pos != std::string_view::npos) {
                  check = true;
                  goto end;
                  }
               }

         end:
            auto value = toInt<int>(data[i].substr(pos1, pos2 - pos1));
            if (verbose) std::cerr << std::format("{} {}\n", value, check);
            if (check) {
               check_sum += value;
               }
            pos1 = pos2 + 1;
            }

         }
      std::cout << check_sum << '\n';
      }

   void Solution2(my_lines input, bool verbose) {
       std::cout << EPart_of_Riddle::Part2 << '\n';

      auto data = input | std::views::transform([](auto p) { return std::string_view{ p }; })
                        | std::ranges::to<std::vector>();

      auto findnumber = [&data](size_t line, size_t col) {
         size_t start = col, end = col;
         while (start > 0 && isdigit(data[line][start - 1])) --start;
         while (end < data[line].size() - 1 && isdigit(data[line][end + 1])) ++end;
         return toInt<int>(data[line].substr(start, end - start + 1));
         };

      std::cout << std::format("{} lines readed.\n", data.size());
      uint64_t check_sum = 0;
      for (size_t i = 0; i < data.size(); ++i) {
         //if (verbose) (std::format("{}\n", data[i]);
         for (auto pos1 = data[i].find_first_of("*"); pos1 != std::string_view::npos; pos1 = data[i].find_first_of("*", pos1)) {
            std::pair<size_t, size_t> pos = { pos1 > 0 ? pos1 - 1 : 0, pos1 + 1 < data[i].size() ? pos1 + 1 : pos1 };
            if (verbose) std::cerr << std::format("Line {}, Col {} -> ({}, {})", i, pos1, pos.first, pos.second);
            uint32_t gear_ratio = 1;
            int iFound = 0;
            if(i > 0) {
               if(!isdigit(data[i - 1][pos1])) {
                  // possible 2 values in a line
                  if(pos1 > 0 && isdigit(data[i - 1][pos1 - 1])) {
                     auto val = findnumber(i - 1, pos1 - 1);
                     if (verbose) std::cerr << std::format(" ^- {}", val);
                     gear_ratio *= val;
                     ++iFound;
                     }
                  if (pos1 + 1 < data[i - 1].size() && isdigit(data[i - 1][pos1 + 1])) {
                     auto val = findnumber(i - 1, pos1 + 1);
                     if (verbose) std::cerr << std::format(" -^ {}", val);
                     gear_ratio *= val;
                     ++iFound;
                     }
                  }
               else {
                  for(size_t j = pos.first; j < pos.second + 1;++j) { 
                     if(isdigit(data[i-1][j])) {
                        auto val = findnumber(i - 1, j);
                        if (verbose) std::cerr << std::format(" ^ {}", val);
                        gear_ratio *= val;
                        ++iFound;
                        break;
                        }
                     }
                  }
               }

            if(pos1 > 0 && isdigit(data[i][pos1 - 1])) {
               auto val = findnumber(i, pos1 - 1);
               if (verbose) std::cerr << std::format(" < {}", val);
               gear_ratio *= val;
               ++iFound;
               }

            if (pos1 + 1 < data[i].size() && isdigit(data[i][pos1 + 1])) {
               auto val = findnumber(i, pos1 + 1);
               if (verbose) std::cerr << std::format(" > {}", val);
               gear_ratio *= val;
               ++iFound;
               }

            if(i + 1 < data.size()) {
               if (!isdigit(data[i + 1][pos1])) {
                  // possible 2 values in a line
                  if (pos1 > 0 && isdigit(data[i + 1][pos1 - 1])) {
                     auto val = findnumber(i + 1, pos1 - 1);
                     if (verbose) std::cerr << std::format(" v- {}", val);
                     gear_ratio *= val;
                     ++iFound;
                     }
                  if (pos1 + 1 < data[i + 1].size() && isdigit(data[i + 1][pos1 + 1])) {
                     auto val = findnumber(i + 1, pos1 + 1);
                     if (verbose) std::cerr << std::format(" -v {}", val);
                     gear_ratio *= val;
                     ++iFound;
                     }
                  }
               else {
                  for (size_t j = pos.first; j < pos.second + 1; ++j) {
                     if (isdigit(data[i + 1][j])) {
                        auto val = findnumber(i + 1, j);
                        if(verbose) std::cerr << std::format(" v {}", val);
                        gear_ratio *= val;
                        ++iFound;
                        break;
                        }
                     }
                  }
               }

            if (iFound >= 2) {
               if (verbose) std::cerr << std::format(" = {}", gear_ratio);
               check_sum += gear_ratio;
               }
            // possible check for > 2

            if (verbose) std::cerr << "\n";

            pos1++;
            }

         }
      std::cout << check_sum << '\n';
      }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "3nd Day, ";
      switch (part) {
      case 1: Solution1(input, verbose); break;
      case 2: Solution2(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 3nd day.");
      }
   }



} // end of namespace
