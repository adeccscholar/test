#include "aoc_lib.h"

#include <iostream>
#include <string>
#include <map>
#include <functional>
//#include <cmath>
#include <numeric>
#include <stdexcept>
#include <ranges>

using namespace std::string_literals;
using namespace std::placeholders;

namespace aoc2023_day6 {

   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {

      using number_ty = std::conditional_t<part == EPart_of_Riddle::Part1, size_t, unsigned long long>;

      std::vector<std::pair<number_ty, number_ty>> races;

      auto reading = [&]() {
         std::vector<number_ty> distances, times;

         static auto readData = [](std::string_view line, std::vector<number_ty>& data) {
                data.clear();
                if constexpr (part == EPart_of_Riddle::Part1)
                   std::ranges::copy(line | std::views::split(' ') | std::views::filter([](auto p) {return p.size() > 0; })
                                          | std::views::transform([](auto p) { return toInt<number_ty>(std::string_view{ p }); })
                                     , std::back_inserter(data));
                else {
                   auto withoutSpaces = line | std::views::filter([](char c) { return !std::isspace(c); });
                   data.emplace_back(toInt<number_ty>(std::string { withoutSpaces.begin(), withoutSpaces.end() } ));
                   }
                };

         static std::map<std::string, std::function<void (std::string_view)>> contrl { 
                   { "Time"s,     std::bind(readData, _1, std::ref(times)) },
                   { "Distance"s, std::bind(readData, _1, std::ref(distances))  }
                  };

         auto readingRows = [&](const std::string_view line) {
            auto sep_pos = line.find(':');
            auto command_line = line.substr(0, sep_pos);
            auto data_line = line.substr(sep_pos + 1, line.size() - sep_pos);
            if (auto it = contrl.find(to_String(command_line)); it != contrl.end()) return it->second(data_line);
            else throw std::domain_error("the value \""s + to_String(command_line) + "\" isn't in the control structure."s);
            };

         for (auto const& line : input | std::views::transform([](auto p) { return std::string_view{ p }; })) readingRows(line);
 
         std::ranges::transform(std::views::zip(times, distances), std::back_inserter(races),
                                [](const auto& race) { return std::make_pair(std::get<0>(race), std::get<1>(race)); });
         };

      std::cout << part << ".\n";

      try {
         reading();

         if(verbose) {
            for (size_t i = 0; auto const& [time, distance] : races)
               std::cerr << std::format("Race {:2d}:{:5d} ms / {:5d} mm\n", ++i, time, distance);
            }

         number_ty solution = 1;
         for (auto const& [time, distance] : races) {
            const double epsilon = 0.0000001;
            double discriminant = std::pow(static_cast<double>(time), 2.0) - (4.0 * (distance + epsilon));
            if (discriminant < 0)
               throw std::runtime_error(std::format("the discriminant is {}. there isn't a possible solution for {} mm in {} ms.", 
                                                    discriminant, distance, time));
            
            auto border = std::make_pair( static_cast<number_ty>(std::ceil ((time - std::sqrt(discriminant)) / 2)),
                                          static_cast<number_ty>(std::floor((time + std::sqrt(discriminant)) / 2)) );

            number_ty ways_to_beat = border.second - border.first + 1;

            if(verbose) {
               std::cerr << std::format("{} - {}, Count =  {}\n", border.first, border.second, ways_to_beat);
               }
            solution *= ways_to_beat;
            }

         std::cout << "The product of the number of ways to surpass the record is " << solution << ".\n";
         }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 6, " << part << ": "<< ex.what() << '\n';
         }
      }

   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "6th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 6th day.");
      }
   }

} // end of namespace
