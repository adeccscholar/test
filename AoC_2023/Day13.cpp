#include "aoc_lib.h"

#include <iostream>
#include <iomanip>
#include <exception>
#include <stdexcept>
#include <optional>
#include <span>
#include <format>
#include <ranges>

namespace aoc2023_day13 {



   class TLandScape {
   public:
      enum class EElements : char { undefined, ash = '.', rocks = '#' };
      using position_ty = std::pair<size_t, size_t>;
      using landscape_data = std::vector<EElements>;
      enum class EDirection : char { undefined, horizontal, vertical };

      class TMapData {
         friend TLandScape;
         friend std::ostream& operator << (std::ostream& out, TMapData const& data) {
            for (size_t i = 0; auto const& c : data.data) {
               out << static_cast<char>(c);
               if (!(++i < data.iWidth)) {
                  out << '\n';
                  i = 0;
                  }
               }
            return out;
            }

         private:
            size_t iWidth = 0;
            size_t iHeight = 0;
            landscape_data data;

            size_t Index(size_t x, size_t y) const& {
               if (!(x < iWidth && y < iHeight))
                  throw std::runtime_error(std::format("index ({}, {}) in TMap is out of range ({}, {})", x, y, iWidth, iHeight));
               return y * iWidth + x;
               }

            EElements& operator [](size_t pos) {
               if (!(pos >= 0 && pos < data.size()))
                  throw std::runtime_error(std::format("index {} in TMap is out of range ({}, {})", pos, 0, data.size()));
               return data[pos];
               }

            EElements const& operator [](size_t pos) const {
               if (!(pos >= 0 && pos < data.size()))
                  throw std::runtime_error(std::format("index {} out of range ({}, {})", pos, 0, data.size()));
               return data[pos];
               }


         public:
            TMapData() = default;
            TMapData(TMapData const&) = default;
            TMapData(TMapData&&) noexcept = default;

            EElements& operator [](position_ty const& pos) {
               return (*this)[Index(pos.first, pos.second)];
               }

            EElements const& operator [](position_ty const& pos) const {
               return (*this)[Index(pos.first, pos.second)];
               }

            std::optional<size_t> Scan(EDirection dir) {
               static constexpr auto recorder = [](size_t size) {
                  if (size < 2) throw std::runtime_error("unexpected size for data line");
                  std::vector<size_t> daten(size);
                  daten[0] = size / 2;
                  auto offsets = std::views::iota(1ull, daten[0] + 1 ) | std::ranges::to<std::vector>();
                  for (size_t i = 1; auto offset : offsets) {
                     if (i >= daten.size()) break;
                     daten[i++] = daten[0] - offset;
                     daten[i++] = daten[0] + offset;
                     }
                  daten.erase(std::remove(daten.begin(), daten.end(), 0), daten.end());
                  return daten;
                  };

               if (dir == EDirection::horizontal) {
                  for(size_t col : recorder(iWidth)) {
                     //if(col > 0) {
                        //std::cerr << "Compare Col = " << col << '\n';
                        auto columns = std::views::zip(std::views::iota(col, iWidth), std::ranges::reverse_view(std::views::iota(0ull, col)));

                        bool mirrored = std::ranges::all_of(columns, [this](const auto& mirroredIndices) {
                           auto [index1, index2] = mirroredIndices;
                           return std::ranges::all_of(std::views::iota(0ull, iHeight), [&](size_t rowIndex) {
                              //std::cerr << "compare (" << rowIndex << ", " << index1 << ") mit (" << rowIndex << ", " << index2 << ")\n";
                              return (*this)[Index(index1, rowIndex) ] == (*this)[Index(index2, rowIndex) ];
                              });
                           });
                        if (mirrored) return col;
                     //   }
                     }
                  }
               else if (dir == EDirection::vertical) {
                  auto visiting = recorder(iHeight);
                  //std::cerr << "Scan with Height: " << iHeight << '\n';
                  //for (auto v : visiting) std::cerr << v << " ";
                  //std::cerr << '\n';
                  for (size_t row : visiting) {
                     //if(row > 0) {
                        //std::cerr << "Compare Row = " << row << '\n';
                        auto rows = std::views::zip(std::views::iota(row, iHeight), std::ranges::reverse_view(std::views::iota(0ull, row)));

                        bool mirrored = std::ranges::all_of(rows, [this](const auto& mirroredIndices) {
                           auto [index1, index2] = mirroredIndices;
                           return std::ranges::all_of(std::views::iota(0ull, iWidth), [&](size_t colIndex) {
                              //std::cerr << "compare (" << colIndex << ", " << index1 << ") mit (" << colIndex << ", " << index2 << ")\n";
                              return (*this)[Index(colIndex, index1)] == (*this)[Index(colIndex, index2)];
                              });
                           });
                        if (mirrored) return row;
                        }              
                    // }
                  }
               else throw std::runtime_error("unexpected direction for scan");
               return {};
               }

            void PrintScan(EDirection dir, size_t pos, std::ostream& out) {
               for (size_t row = 0ull; row < iHeight; ++row) {
                  for (size_t col = 0ull; col < iWidth; ++col) {
                     out << static_cast<char>((*this)[Index(col, row)]);
                     if (dir == EDirection::vertical && col == pos - 1) out << '|';
                     }
                  if (dir == EDirection::horizontal && row == pos - 1) {
                     out << '\n' << std::setw(iWidth)
                         << std::setfill('-') << std::right << '-'
                         << std::setfill(' ') << '\n';
                     }
                  else out << '\n';
                  }
               }



            void Read(std::vector<std::string_view> const& input) {
               iWidth = input.begin()->size();
               iHeight = input.size();         
               data.reserve(iWidth * iHeight);
               for (size_t i; auto const& line : input) {
                  if (line.size() != iWidth) throw std::runtime_error("unexpected input while read landscape");
                  std::transform(line.begin(), line.end(), std::back_inserter(data), [](char p) { return static_cast<EElements>(p); });
                  }
               }
         };

   private:
      
      TMapData data;
      
   public:
      TLandScape() = delete;
      TLandScape(TLandScape const&) = delete;
      TLandScape(TLandScape&&) noexcept = delete;

      void Print(std::ostream& out) {
         out << "map:\n" << data;
         }

      size_t Scan(bool verbose) {
         size_t sum = 0;

         auto horizontal = data.Scan(EDirection::horizontal);
         auto vertical   = data.Scan(EDirection::vertical);

         if (horizontal && !vertical) {
            sum += *horizontal; 
            if(verbose) data.PrintScan(EDirection::vertical, *horizontal, std::cerr);
            }
         else if(vertical && !horizontal) {
            sum += 100 * *vertical; // std::cout << "vertical at " << *test << '\n';
            if (verbose) data.PrintScan(EDirection::horizontal, *vertical, std::cerr);
            }
         else if(vertical && horizontal) {
            if (verbose) {
               std::cout << "\n\nProblem, 2 Scans found: " << *vertical << " / " << *horizontal << '\n';
               Print(std::cerr);
               }
            }
         else {
            if (verbose) {
               std::cerr << "\nno match found:\n";
               Print(std::cerr);
               }
            }

         if(verbose) std::cerr << " -> " << sum << "\n\n";

         return sum;
         }

      TLandScape(std::vector<std::string_view> const& input) { // , std::vector<std::string_view> const& input_2) {
         data.Read(input);
         //data2.Read(input_2);
         }
   };


   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      std::cout << part << ".\n";

      auto input_vec = input | std::views::transform([](auto p) { return std::string_view{ p }; }) | std::ranges::to<std::vector>();
      auto blocks = input_vec | std::views::transform([](auto p) { return std::string_view{ p }; }) 
                              | std::views::split(std::string_view{}) | std::ranges::to<std::vector>();


      //std::ranges::for_each(blocks | std::views::chunk(2), [](auto subrange) {
      //TLandScape test({ subrange[0].begin(), subrange[0].end() }, { subrange[1].begin(), subrange[1].end() });
      size_t solution = 0;
      std::ranges::for_each(blocks, [&solution, verbose](auto subrange) {
          TLandScape test({ subrange.begin(), subrange.end() });
          //test.Print(std::cerr);
          solution += test.Scan(verbose);
          });
 

      try {
         std::cout << "solution = " << solution << '\n';

      }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 13, " << part << ": " << ex.what() << '\n';
      }
   }



   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "13th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 8th day.");
      }
   }

} // end of namespace
