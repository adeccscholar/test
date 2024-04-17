#include "aoc_lib.h"

#include <string>
#include <stdexcept>
#include <vector>
#include <ranges>

namespace aoc2023_day4 {

   struct scratchcard_ty {
      int              iID = 0;
      int              iCount = 0;
      std::vector<int> winning_numbers;
      };

   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      std::cout << part << ".\n";
      std::vector<scratchcard_ty> scratchcards;

      for(auto const& line : input | std::views::transform([](auto p) { return std::string_view{ p }; })) {
         //std::cerr << std::format("Line: {}\n", line);
         scratchcard_ty scratchcard;
         size_t pos1 = line.find_first_of(strNumbers1);
         size_t pos2 = line.find_first_not_of(strNumbers2, pos1);
         if (pos1 == std::string_view::npos || pos2 == std::string_view::npos || pos1 >= pos2) {
            std::runtime_error(std::format("couldn't read draw with numbers in: {}", line));
            }
         scratchcard.iID    = toInt<int>(line.substr(pos1, pos2 - pos1));
         scratchcard.iCount = 1;

         pos1 = line.find_first_not_of(" ", line.find(':') + 1);
         pos2 = line.find('|', pos1);
         std::string_view cards_input   = trim(line.substr(pos1, pos2 - pos1));
         std::string_view cards_winning = trim(line.substr((pos2 + 1), line.size() - pos2 - 1));
         if (verbose) std::cerr << std::format("{} / {} - {}\n", scratchcard.iID, cards_input, cards_winning);

         auto input_ids = cards_input | std::views::split(' ')
                                      | std::views::filter([](auto p) {return p.size() > 0; })
                                      | std::views::transform([](auto p) { return toInt<int>(trim(std::string_view{ p })); })
                                      | std::ranges::to<std::vector>();

         auto winning_ids = cards_winning | std::views::split(' ')
                                          | std::views::filter([](auto p) {return p.size() > 0; })
                                          | std::views::transform([](auto p) { return toInt<int>(trim(std::string_view{ p })); })
                                          | std::ranges::to<std::vector>();
         std::ranges::sort(input_ids);
         std::ranges::sort(winning_ids);
         std::ranges::set_intersection(input_ids, winning_ids, std::back_inserter(scratchcard.winning_numbers));
         scratchcards.emplace_back(scratchcard);
         }

      int check_sum = 0;

      if constexpr (part == EPart_of_Riddle::Part1) {
         for(auto const& scratchcard : scratchcards) {
            int value = scratchcard.winning_numbers.size() == 0 ? 0 : std::pow(2, scratchcard.winning_numbers.size() - 1);
            if (verbose) std::cerr << std::format("points: {}\n", value);
            check_sum += value;
            }
         std::cout << std::format("sum of all winning points is {}\n", check_sum);
         }
      else {
         for(size_t i = 0; i < scratchcards.size(); ++i) {
            int points = scratchcards[i].winning_numbers.size();
            for (size_t j = 1; j <= points && (i + j) < scratchcards.size(); ++j) 
               scratchcards[i + j].iCount += scratchcards[i].iCount;
            if (verbose) std::cerr << std::format("{}: {}\n", scratchcards[i].iID, scratchcards[i].iCount);
            check_sum += scratchcards[i].iCount;
            }
         std::cout << std::format("count of cards is {}\n", check_sum);
         }
      }

   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "4th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 4th day.");
      }
   }



} // end of namespace
