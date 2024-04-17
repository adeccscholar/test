#include "aoc_lib.h"

#include <iostream>
#include <vector>
#include <string_view>
#include <stdexcept>
#include <format>
#include <ranges>
#include <algorithm>
#include <numeric>

namespace aoc2023_day9 {

   using data_ty = int32_t;
   using data_line_ty = std::vector<data_ty>;
   using data_lines_ty = std::vector<data_line_ty>;

   inline void read_data_line(data_line_ty& data, std::string_view line) {
      data.clear();
      std::ranges::copy(line | std::views::split(' ')
                             | std::views::filter([](auto p) { return p.size() > 0; })
                             | std::views::transform([](auto p) { return toInt<data_ty>(std::string_view{ p }); })
                        , std::back_inserter(data));
      }

   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      std::cout << part << ".\n";

      try {
 
         auto print = [](data_line_ty const& line) {
            for (size_t i = 0; auto col : line)
               std::cerr << (i++ > 0 ? ", " : "") << col;
            std::cerr << '\n';
            };

         data_lines_ty all_lines;
         for (auto line : input | std::views::filter([](auto p) { return p.size() > 0; })) {
            data_line_ty  one_line;
            read_data_line(one_line, line);
            all_lines.emplace_back(std::move(one_line));
            }

         for (size_t line = 0; line < all_lines.size();++line) {
            if (verbose) print(all_lines[line]);
            
            if(all_lines[line].size() > 1) {
               data_lines_ty differences;
               data_line_ty data;
               for (size_t idx = 1; idx < all_lines[line].size(); ++idx) {
                  data.emplace_back(all_lines[line][idx] - all_lines[line][idx - 1]);
                  }
               differences.emplace_back(std::move(data));
               if (verbose) print(differences[0]);

               bool boAllNull = std::ranges::all_of(differences[0], [](data_ty e) { return e == 0; });
               if(!boAllNull) {
                  for (size_t idx = 1; ; ++idx) {
                      data_line_ty data;
                      for (size_t idx2 = 1; idx2 < differences[idx - 1].size(); ++idx2)  data.emplace_back(differences[idx - 1][idx2] - differences[idx-1][idx2-1]);
                      differences.emplace_back(std::move(data));
                      if (verbose) print(differences.back());
                      bool boAllNull = std::ranges::all_of(differences[idx], [](data_ty e) { return e == 0; });
                      if (boAllNull) break;
                      }
                  }
               for (size_t idx3 = differences.size() - 1; idx3 > 0; --idx3)
                  if constexpr (part == EPart_of_Riddle::Part1)
                     differences[idx3 - 1].emplace_back(differences[idx3 - 1].back() + differences[idx3].back());
                  else
                     differences[idx3 - 1].emplace(differences[idx3 - 1].begin(), differences[idx3 - 1].front() - differences[idx3].front());

               if constexpr (part == EPart_of_Riddle::Part1)
                  all_lines[line].emplace_back(all_lines[line].back() + differences[0].back());
               else
                  all_lines[line].emplace(all_lines[line].begin(), all_lines[line].front() - differences[0].front());

               if (verbose) {
                  std::cerr << "------------ Result --------------------------\n";
                  print(all_lines[line]);
                  std::cerr << "------------ Finish --------------------------\n\n";
                  }
               }
            else {
               throw std::runtime_error("input can't processed");
               }
            }

         if(verbose) {
            std::cerr << "Values with added next position:\n";
            for (auto const& line : all_lines) {
               for (size_t i = 0; auto col : line)
                  std::cerr << (i++ > 0 ? ", " : "") << col;
               std::cerr << '\n';
               }
            }

         data_ty  result = std::accumulate(all_lines.begin(), all_lines.end(), 0, [](data_ty sum, std::vector<data_ty>const& data) {
                        if (!data.empty()) {
                            if constexpr (part == EPart_of_Riddle::Part1) 
                               return sum += data.back();
                            else
                               return sum += data.front();
                            }
                         else return sum;
                         });

         std::cout << "solution = " << result << '\n';
         }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 9, " << part << ": " << ex.what() << '\n';
      }
   }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "9th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 8th day.");
      }
   }

} // end of namespace
