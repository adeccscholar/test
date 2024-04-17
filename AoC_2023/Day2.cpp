#include "aoc_lib.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <ranges>
#include <numeric>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace aoc2023_day2 {

   /// data type to represent possible colors for dices
   enum class EColors : uint8_t { blue, red, green };

   /// data type for colors and values, used for maximal values and for the draw
   using dices_with_colors = std::map<EColors, unsigned short>;

   /// maximal values for dices with a special color in the 1th part of the daily riddle
   const dices_with_colors amounts_of_dices = { { EColors::blue, 14 }, { EColors::green, 13 }, { EColors::red, 12 } };

   inline bool Check(dices_with_colors const& max, dices_with_colors const& draw) {
      for (auto const& [color, count] : draw) {
         if (auto it = max.find(color); it != max.end()) {
            if (count > it->second) return false;
            }
         else if(count > 0) return false;
         }
      return true;
      } 

   inline void Compare(dices_with_colors& max, dices_with_colors const& draw) {
      for (auto const& [color, count] : draw) {
         if (auto it = max.find(color); it != max.end()) {
            if (count > it->second) it->second = count;
            }
         else max.insert( { color, count } );
         }
      }

   void Add(dices_with_colors& draw, std::string_view str) {
      static std::map<std::string_view, EColors> bridge = { {"red"sv, EColors::red },
                                                            {"blue"sv, EColors::blue},
                                                            {"green"sv, EColors::green} };

      size_t pos1 = str.find_first_of(strNumbers1);
      size_t pos2 = str.find_first_not_of(strNumbers2, pos1);
      if (pos1 == std::string_view::npos || pos2 == std::string_view::npos || pos1 >= pos2) {
         std::runtime_error(std::format("couldn't read draw with numbers in: {}", str));
         }
      int iCount = toInt<unsigned short>(str.substr(pos1, pos2 - pos1));
      pos1 = str.find_first_not_of(" ", pos2);
      auto text = str.substr(pos1, str.size() - pos1);
      if(auto it = bridge.find(text); it != bridge.end()) {
         draw[it->second] += iCount;
         }
      // std::cerr << std::format(" --> {} = {}\n", text, iCount);
      }

template <EPart_of_Riddle part>
void Solution(my_lines input, bool verbose) {

   std::cout << part << '\n';

   auto lines = input | std::views::transform([](auto p) { return std::string_view{ p }; }) | std::ranges::to<std::vector>();
   
   if(verbose) std::cerr << lines.size() << " rows readed and will processed now.\n";

   uint32_t check_sum = 0;
   
   for(auto const& line : lines) {
      if(verbose) std::cerr << std::format("process line: {}\n", line);

      std::conditional_t<part == EPart_of_Riddle::Part2, dices_with_colors, bool > check_value;
      if constexpr (part == EPart_of_Riddle::Part1) check_value = true;

      // read the game in the line
      size_t pos1 = line.find_first_of(strNumbers1);
      size_t pos2 = line.find_first_not_of(strNumbers2, pos1);
      if(pos1 == std::string_view::npos || pos2 == std::string_view::npos || pos1 >= pos2) {
         std::runtime_error(std::format("couldn't read game id in input line: {}", line));
         }
      auto iGame = toInt<int>(line.substr(pos1, pos2 - pos1));

      if (verbose) std::cerr << std::format("Game ID = {}\n",iGame);
      pos1 = line.find_first_not_of(" ", line.find(':') + 1);
      std::string_view draws_input = trim(line.substr(pos1, line.size() - pos1));
      if (verbose) std::cerr << std::format("Game data: {}\n", draws_input);
      
      auto draws = draws_input | std::views::split(';') | std::views::transform([](auto d) { return trim(std::string_view { d.begin(), d.end() }); })
                               | std::ranges::to<std::vector>();

      for (auto const& draw : draws) {
         if (verbose) std::cerr << std::format("draw: {}\n",draw);
         dices_with_colors draw_to_check;

         auto dices = draw 
            | std::views::split(',')
            | std::views::transform([](auto d) { return std::string_view{ d.begin(), d.end() }; })
            | std::ranges::to<std::vector>();


         for(auto const& dice : dices | std::views::transform([](auto d) { return trim(d); })) {
            if (verbose) std::cerr << std::format("dice: {} read as |", dice);
            Add(draw_to_check, dice);
            }

         if constexpr (part == EPart_of_Riddle::Part1) {
            if(!Check(amounts_of_dices, draw_to_check)) {
               check_value = false;
               break;
               }
            }
         else {
            Compare(check_value, draw_to_check);
            }
         }

      if constexpr (part == EPart_of_Riddle::Part1) {
         if (check_value) check_sum += iGame;
         else check_value = true;
         }
      else {
         auto values = check_value | std::views::transform([](auto p) { return p.second;  }) | std::ranges::to<std::vector>();
         check_sum += std::reduce(values.begin(), values.end(), 1, std::multiplies<>());

         }
      }

   std::cout << "the solution for " << part << " is " << check_sum << ".\n";
   }

void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "2nd Day, ";
      switch (part) {
         case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
         case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
         default: throw std::runtime_error("unexpexted part for the riddle at 2nd day.");
         }
   }



} // end of namespace
