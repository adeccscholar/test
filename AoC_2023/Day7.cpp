#include "aoc_lib.h"

#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include <map>
#include <regex>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <numeric>
#include <ranges>
#include <execution>

using namespace std::string_literals;
using namespace std::placeholders;

namespace aoc2023_day7 {

   enum class ERank_of_Hands : uint8_t { noScore = 0, HighCard, OnePair, TwoPairs, Three_of_Kind, Full_House, 
                                         Four_of_Kind, Five_of_Kind };

   std::ostream& operator << (std::ostream& out, ERank_of_Hands rank) {
      switch(rank) {
         case ERank_of_Hands::noScore:       out << "no Score"; break;
         case ERank_of_Hands::HighCard:      out << "HighCard"; break;
         case ERank_of_Hands::OnePair:       out << "OnePair"; break;
         case ERank_of_Hands::TwoPairs:      out << "TwoPairs"; break;
         case ERank_of_Hands::Three_of_Kind: out << "Three_of_Kind"; break;
         case ERank_of_Hands::Full_House:    out << "Full_House"; break;
         case ERank_of_Hands::Four_of_Kind:  out << "Four_of_Kind"; break;
         case ERank_of_Hands::Five_of_Kind:  out << "Five_of_Kind"; break;
         default: throw std::runtime_error("unexpected");
         }
      return out;
      }

   using card_ty = std::tuple<std::string, size_t, ERank_of_Hands>;


   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      // clang-format off
      static const std::map<char, size_t> Rank_of_Cards = []() -> std::map<char, size_t> {
                                                 if constexpr (part == EPart_of_Riddle::Part1)
                                                    return { {'2',  1 },{'3',  2 },{'4',  3 },{'5',  4 },
                                                             {'6',  5 },{'7',  6 },{'8',  7 },{'9',  8 },
                                                             {'T',  9 },{'J', 10 },{'Q', 11 },{'K', 12 },
                                                             {'A', 13 } };
                                                 else
                                                    return { {'J',  1 },{'2',  2 },{'3',  3 },{'4',  4 },
                                                             {'5',  5 },{'6',  6 },{'7',  7 },{'8',  8 },
                                                             {'9',  9 },{'T', 10 },{'Q', 11 },{'K', 12 },
                                                             {'A', 13 } };
                                                 }();

      static auto constexpr GetCards = [](card_ty & deck) -> std::string {
         if constexpr (part == EPart_of_Riddle::Part1) return std::get<0>(deck);
         else {
            return std::ranges::to<std::string>(std::get<0>(deck) | std::views::filter([](auto c) { return c != 'J'; }));
            }
         };


      auto CalculateScore = [](card_ty& deck, bool verbose) -> card_ty {
         auto constexpr CompScore = [](auto lhs, auto rhs) { return lhs.second < rhs.second; };
         
         std::map<char, size_t> result;
        
         for (auto c : GetCards(deck)) {
            if (auto it = result.find(c); it != result.end()) ++it->second;
            else result.insert({ c, 1 });
            }

         if constexpr (part == EPart_of_Riddle::Part1) {
            switch (result.size()) {
               case 5: std::get<2>(deck) = ERank_of_Hands::HighCard; break;
               case 4: std::get<2>(deck) = ERank_of_Hands::OnePair; break;
               case 3: if (auto it = std::ranges::max_element(result, CompScore); it->second == 3)
                          std::get<2>(deck) = ERank_of_Hands::Three_of_Kind;
                       else
                          std::get<2>(deck) = ERank_of_Hands::TwoPairs;
                       break;
               case 2: if (auto it = std::ranges::max_element(result, CompScore); it->second == 4)
                          std::get<2>(deck) = ERank_of_Hands::Four_of_Kind;
                       else
                          std::get<2>(deck) = ERank_of_Hands::Full_House;
                       break;
               case 1: std::get<2>(deck) = ERank_of_Hands::Five_of_Kind; break;
               default: throw std::runtime_error("unexpected number auf cards in scoring.");
               }


            if(verbose) {
               std::cerr << std::get<0>(deck) << " = " << std::get<2>(deck) << '\n';
               }
            }
         else {
            using CalcRank = std::map<std::pair<size_t, std::vector<size_t>>, ERank_of_Hands>;
            static const CalcRank Calculate = {
               { { 0, { 5 }    }, ERank_of_Hands::Five_of_Kind },
               { { 0, { 4, 1 } }, ERank_of_Hands::Four_of_Kind },
               { { 0, { 3, 2 } }, ERank_of_Hands::Full_House },
               { { 0, { 3, 1 } }, ERank_of_Hands::Three_of_Kind },
               { { 0, { 2, 2 } }, ERank_of_Hands::TwoPairs },
               { { 0, { 2, 1 } }, ERank_of_Hands::OnePair },
               { { 0, { 1, 1 } }, ERank_of_Hands::HighCard },

               { { 1, { 4 }    }, ERank_of_Hands::Five_of_Kind },
               { { 1, { 3, 1 } }, ERank_of_Hands::Four_of_Kind },
               { { 1, { 2, 2 } }, ERank_of_Hands::Full_House }, 
               { { 1, { 2, 1 } }, ERank_of_Hands::Three_of_Kind }, 
               { { 1, { 1, 1 } }, ERank_of_Hands::OnePair },

               { { 2, { 3 }    }, ERank_of_Hands::Five_of_Kind },
               { { 2, { 2, 1 } }, ERank_of_Hands::Four_of_Kind },
               { { 2, { 1, 1 } }, ERank_of_Hands::Three_of_Kind },
					
               { { 3, { 2 }    }, ERank_of_Hands::Five_of_Kind },
               { { 3, { 1, 1 } }, ERank_of_Hands::Four_of_Kind },
					
               { { 4, { 1 }    }, ERank_of_Hands::Five_of_Kind },
					
               { { 5, { }      }, ERank_of_Hands::Five_of_Kind }
            };

            std::pair<size_t, std::vector<size_t>> seek;
            auto counts = result | std::views::transform([](auto val) { return val.second; }) | std::ranges::to<std::vector>();
            std::ranges::sort(counts, [](auto lhs, auto rhs) { return lhs > rhs; });
            size_t sum = std::accumulate(counts.begin(), counts.end(), 0ull);
            if (sum > 5) throw std::runtime_error("better go home for today");
            seek.first = 5 - sum;
            std::copy_n(counts.begin(), std::min(2ull, counts.size()), std::back_inserter(seek.second));

            if(auto it = Calculate.find(seek); it != Calculate.end()) {
               std::get<2>(deck) = it->second;
               }
            else {
               throw std::runtime_error("unexpected score in recalculation.");
               }
            
            if(verbose) {
               std::cerr << sum << " " << std::get<0>(deck) << " = " << std::get<2>(deck) << " / " << GetCards(deck);
               for (auto val : counts) std::cerr << " " << val;
               std::cerr << '\n';
               }
            }


         return deck;
         };



      static auto CompareScore = [](card_ty const& lhs, card_ty const& rhs) {
         if (auto comp = static_cast<size_t>(std::get<2>(lhs)) <=> static_cast<size_t>(std::get<2>(rhs)); comp == 0) {
            for (auto [l, r] : std::ranges::views::zip(std::get<0>(lhs), std::get<0>(rhs))) {
               if (auto it_l = Rank_of_Cards.find(l); it_l != Rank_of_Cards.end()) {
                  if (auto it_r = Rank_of_Cards.find(r); it_r != Rank_of_Cards.end()) {
                     if (it_l->second < it_r->second) return true;
                     else if (it_l->second > it_r->second) return false;
                     }
                  else throw std::runtime_error("unexpected card rank");
                  }
               else throw std::runtime_error("unexpected card rank");
               }
            return false;
            }
         else return comp < 0;
         };
      // clang-format on

      std::cout << part << ".\n";

      try {

         std::vector<card_ty> decks;
         for (auto const& line : input | std::views::transform([](auto p) { return std::string_view{ p }; })) {
            static const std::regex parser_input("^([AKQJT2-9]{5})\\s+([1-9][0-9]{0,4})$");
            std::string strLine = to_String(line);
            if (std::regex_match(strLine, parser_input));
            else throw std::runtime_error("The input line \""s + strLine + "\" don't match the rules."s);

            std::smatch match;
            if (std::regex_search(strLine, match, parser_input)) {
               card_ty card;
               std::get<0>(card) = match[1];
               std::get<1>(card) = toInt<size_t>(match[2]);
               std::get<2>(card) = ERank_of_Hands::noScore;
               decks.emplace_back(std::move(card));
               }
            else throw std::runtime_error("unexpected input, can't parse the line.");
            }

         std::transform(decks.begin(), decks.end(), decks.begin(), std::bind(CalculateScore, _1, verbose));
         std::sort(decks.begin(), decks.end(), CompareScore);

         auto calc_values = std::views::zip(std::views::iota(1), decks)
                                | std::views::transform([](auto p) { return std::make_pair(std::get<0>(p), std::get<1>(p)); })
                                | std::ranges::to<std::vector>();
         auto sum = std::accumulate(calc_values.begin(), calc_values.end(), 0ull, [](uint64_t partialSum, auto const& p) {
                             return static_cast<uint64_t>(partialSum + (p.first * std::get<1>(p.second))); });
         
         std::cout << "solution = " << sum << '\n';
         }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 7, " << part << ": " << ex.what() << '\n';
         }
      }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "7th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 7th day.");
      }
   }

} // end of namespace
