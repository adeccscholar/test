#include "aoc_lib.h"

#include <iostream>
#include <exception>
#include <stdexcept>
#include <functional>
#include <format>
#include <ranges>

using namespace std::placeholders;

namespace aoc2023_day11 {

   class TIntergalacticSpace {
      public:
         enum SpaceElements : char { undefined, empty = '.', galaxy = '#' };
         using position_ty = std::pair<size_t, size_t>;
      private:
 

         using row_data      = std::vector<SpaceElements>;
         using galaxies_data = std::vector<row_data>;
         using expanding_ty  = std::vector<size_t>;

         size_t iWidth  = 0;
         size_t iHeight = 0;

         galaxies_data data;
         
         size_t                                 factor = 1'000'000;
         std::pair<expanding_ty, expanding_ty>  stretching;

      public:
         TIntergalacticSpace() = delete;
         TIntergalacticSpace(TIntergalacticSpace const&) = delete;

         TIntergalacticSpace(std::vector<std::string_view> const& input) {
            iWidth = input.begin()->size();
            iHeight = input.size();
            data.resize(iHeight);
            for (size_t i = 0; auto const& line : input) {
               if (line.size() != iWidth) throw std::runtime_error(std::format("read intergalactic space, unexpected input in line {}", i));
               data[i].reserve(iWidth);
               std::transform(line.begin(), line.end(), std::back_inserter(data[i]), [](char p) { return static_cast<SpaceElements>(p); });
               ++i;
               }
            std::ranges::copy(std::views::iota(0) | std::views::take(iWidth) 
                                                  | std::views::filter([this](auto const& idx) {
                                                         return std::all_of(this->data.begin(), this->data.end(), [this, idx](auto row) {
                                                                              return row[idx] == SpaceElements::empty; }); })
                                       , std::back_inserter(stretching.first));

            std::ranges::copy(std::views::zip(std::views::iota(0), data)
                                            | std::views::filter([](auto const& p) -> bool {
                                                  return std::all_of(std::get<1>(p).begin(), std::get<1>(p).end(), [](auto e) {
                                                       return e == SpaceElements::empty; }); })
                                            | std::views::transform([](auto p) { return std::get<0>(p); })
                                       ,std::back_inserter(stretching.second));
            }

         SpaceElements const& operator [](position_ty const& pos) const {
            if (!(pos.first < iWidth && pos.second < iHeight))
               throw std::runtime_error(std::format("position ({}, {}) is out of space.", pos.first, pos.second));
            return data[pos.second][pos.first];
            }

         void Expand() {
            auto emptycols = std::views::iota(0) | std::views::take(iWidth) |
                                  std::views::filter([this](auto const& idx) {
                                          return std::all_of(this->data.begin(), this->data.end(), [this, idx](auto row) {
                                                     return row[idx] == SpaceElements::empty; });
                                          }) | std::ranges::to<std::vector>();

            std::ranges::sort(emptycols, [](auto lhs, auto rhs) { return lhs > rhs; });
               
            for(auto& row : data) for (auto i : emptycols) row.insert(row.begin() + i, SpaceElements::empty);
            iWidth += emptycols.size();


            auto emptyrows = std::views::zip(std::views::iota(0), data)
                                  | std::views::filter([](auto const& p) -> bool { 
                                       return std::all_of(std::get<1>(p).begin(), std::get<1>(p).end(), [](auto e) {
                                                              return e == SpaceElements::empty; }); 
                                       })
                                  | std::views::transform([](auto p) { return std::get<0>(p); })
                                  | std::ranges::to<std::vector>();

            std::ranges::sort(emptyrows, [](auto lhs, auto rhs) { return lhs > rhs; });
            for (auto i : emptyrows)  {
               data.insert(data.begin() + i, row_data(iWidth, SpaceElements::empty));
               ++iHeight;
               }
            }

         std::vector<position_ty> Scan() {
            auto range_cols = std::views::iota(0, static_cast<int>(iWidth));
            auto range_rows = std::views::iota(0, static_cast<int>(iHeight));

            auto coordinates = std::views::cartesian_product(range_cols, range_rows)
                                       | std::views::transform([](auto p) { 
                                           return position_ty{ std::get<0>(p), std::get<1>(p) }; })
                                       | std::views::filter([this](auto const& p) -> bool {
                                           position_ty pos { std::get<0>(p), std::get<1>(p) };
                                           return (*this)[pos] == SpaceElements::galaxy; })
                                       | std::ranges::to<std::vector>();
 
            return coordinates;
            }

         // lower triangular matrix
         static std::vector<std::pair<position_ty, position_ty>> Combine(std::vector<position_ty> const& values) {
            auto lower_triangle = std::views::iota(0ull, values.size())
                                         | std::views::transform([&values](auto i) {
                                                   return  std::views::iota(i + 1u, values.size())
                                                               | std::views::transform([i, &values](size_t j) {
                                                                        return std::make_pair(values[i], values[j]); })
                                                               | std::ranges::to<std::vector>(); })
                                         | std::views::join
                                         | std::ranges::to<std::vector>();
            return lower_triangle;
            }


         unsigned long long Distance(std::pair<position_ty, position_ty> const& val) {
            // x distance
            using calc_ty = std::pair<size_t, size_t>;
            calc_ty x = (val.first.first < val.second.first) ? calc_ty { val.first.first, val.second.first } :
                                                               calc_ty { val.second.first, val.first.first };
            calc_ty y = (val.first.second < val.second.second) ? calc_ty { val.first.second, val.second.second } :
                                                                 calc_ty { val.second.second, val.first.second };

            size_t exp_x = std::count_if(stretching.first.begin(), stretching.first.end(), [&x](auto val) {
                                                                                return val > x.first && val < x.second; });
            size_t exp_y = std::count_if(stretching.second.begin(), stretching.second.end(), [&y](auto val) {
                                                                                return val > y.first && val < y.second; });

            return (x.second - x.first) + (y.second - y.first) + (factor - 1) * (exp_x + exp_y);
            }

         unsigned long long Distance_Sum(unsigned long long sum, std::pair<position_ty, position_ty> const& val) {
            return sum += Distance(val);
            }

         void Print(std::ostream& out) {
            for(auto const& row : data) {
               for (auto pos : row) out << static_cast<char>(pos);
               out << '\n';
               }

            }

      };

   inline std::ostream& operator << (std::ostream& out, TIntergalacticSpace::SpaceElements const& c) { 
      return out << static_cast<char>(c); 
      }

   inline std::ostream& operator << (std::ostream& out, TIntergalacticSpace::position_ty const& p) { 
      return out << '(' << p.first << ", " << p.second << ')'; 
      }



   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      std::cout << part << ".\n";

      try {
         auto input_vec = input | std::views::transform([](auto p) { return std::string_view{ p }; }) | std::ranges::to<std::vector>();
         TIntergalacticSpace space(input_vec);
         if(verbose) space.Print(std::cerr);
         //space.Expand();
         if(verbose) {
            std::cerr << "\n-----------------\n";
            space.Print(std::cerr);
            }
         auto positions = space.Scan();
         if(verbose) {
            std::cerr << "galaxies at positions:\n";
            for (auto const& pos : positions) std::cerr << pos << '\n';
            }
         auto pairs = TIntergalacticSpace::Combine(positions);

         auto solution = std::accumulate(pairs.begin(), pairs.end(), 0ull, std::bind(&TIntergalacticSpace::Distance_Sum, &space, _1, _2));

         std::cout << "distance of all galaxies to others is " << solution << '\n';
      }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 11, " << part << ": " << ex.what() << '\n';
      }
   }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "11th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 8th day.");
      }
   }

} // end of namespace
