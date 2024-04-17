#include "aoc_lib.h"
#include "aoc_wcout.h"

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <span>
//#include <mdspan>
#include <utility>
#include <stdexcept>
#include <ranges>
#include <format>
#include <algorithm>
#include <QCoreApplication>

namespace aoc2023_day10 {

   class TLandscape {
      public:
         /// Possible directions for the move
         enum class Directions : uint8_t { undefined, east, north, west, south };
         enum class Moves : uint8_t { undefined, straight, right, left };

         /// Possible parts of the pipe
         enum class KindOfPipe : char {
            vertical        = '|', ///< is a vertical pipe connecting north and south
            horizontal      = '-', ///< is a horizontal pipe connecting east and west.
            north_east_bend = 'L', ///< is a 90 - degree bend connecting north and east.
            north_west_bend = 'J', ///< is a 90 - degree bend connecting north and west.
            south_west_bend = '7', ///< is a 90 - degree bend connecting south and west.
            south_east_bend = 'F', ///< is a 90 - degree bend connecting south and east.
            ground          = '.', ///< is ground; there is no pipe in this tile.
            start           = 'S', ///< is the starting position of the animal; there is a pipe on this tile
            pipe            = 'X', ///< added, is a solid part of a closed pipe 
            water           = ' ', ///< added, is outside of the pipe, we declare it as water 
            undefined       = '~'  ///< insert for an undefined kind of the pipe
            };

         friend std::ostream& operator << (std::ostream& out, Directions data);
         friend std::wostream& operator << (std::wostream& out, Directions data);
         friend std::ostream& operator << (std::ostream& out, Moves data);
         friend std::wostream& operator << (std::wostream& out, Moves data);
         friend std::ostream& operator << (std::ostream& out, TLandscape::KindOfPipe data);
         friend std::wostream& operator << (std::wostream& out, TLandscape::KindOfPipe data);
         friend std::ostream& operator << (std::ostream& out, TLandscape const& data);
         friend std::wostream& operator << (std::wostream& out, TLandscape const& data);
         
      private:
         // variable with the rules for the oves through the kind of pipes
         // rules, vertical before horizontal
         // both vertical north to south
         // if both horizontal then west to east
         using rules_for_pipe_ty = std::map<KindOfPipe, std::pair<Directions, Directions>>;
         static rules_for_pipe_ty inline rules = {
               { KindOfPipe::vertical,        { Directions::north, Directions::south } },
               { KindOfPipe::horizontal,      { Directions::west,  Directions::east  } },
               { KindOfPipe::north_east_bend, { Directions::north, Directions::east  } },
               { KindOfPipe::north_west_bend, { Directions::north,  Directions::west } },
               { KindOfPipe::south_west_bend, { Directions::south, Directions::west  } },
               { KindOfPipe::south_east_bend, { Directions::south,  Directions::east } }
            };

         // is pipe, horiz, vert, turn (irrelevant)
         using rules_for_kind_ty = std::map<KindOfPipe, std::tuple<bool, bool, bool, bool>>;
         static rules_for_kind_ty inline parts = {
               { KindOfPipe::vertical,        { true,  false, true,  false } },
               { KindOfPipe::horizontal,      { true,  true,  false, false } },
               { KindOfPipe::north_east_bend, { true,  true,  true,  true  } }, 
               { KindOfPipe::north_west_bend, { true,  true,  true,  true  } }, 
               { KindOfPipe::south_west_bend, { true,  true,  true,  false } }, 
               { KindOfPipe::south_east_bend, { true,  true,  true,  false } }, 
               { KindOfPipe::ground,          { false, false, false, false } },
               { KindOfPipe::start,           { true,  true,  true,  false } },
               { KindOfPipe::pipe,            { true,  true,  true,  false } },
               { KindOfPipe::water,           { false, false, false, false } },
               { KindOfPipe::undefined,       { false, false, false, false } }
            };

         using rules_for_move_ty = std::map<std::pair<Directions, KindOfPipe>, std::pair<Directions, Moves>>;
         static rules_for_move_ty inline moves = {
              { { Directions::undefined, KindOfPipe::vertical },        { Directions::north, Moves::straight } },
              { { Directions::undefined, KindOfPipe::horizontal },      { Directions::east,  Moves::straight } },
              { { Directions::undefined, KindOfPipe::north_east_bend }, { Directions::north, Moves::right } },
              { { Directions::undefined, KindOfPipe::north_west_bend }, { Directions::west,  Moves::right } },
              { { Directions::undefined, KindOfPipe::south_west_bend }, { Directions::south, Moves::right } },
              { { Directions::undefined, KindOfPipe::south_east_bend }, { Directions::east,  Moves::right } },

              { { Directions::north,     KindOfPipe::vertical },        { Directions::north, Moves::straight } },
              { { Directions::north,     KindOfPipe::south_west_bend }, { Directions::west,  Moves::left } },
              { { Directions::north,     KindOfPipe::south_east_bend }, { Directions::east,  Moves::right } },

              { { Directions::east,      KindOfPipe::horizontal },      { Directions::east,  Moves::straight }},
              { { Directions::east,      KindOfPipe::north_west_bend }, { Directions::north, Moves::left } },
              { { Directions::east,      KindOfPipe::south_west_bend }, { Directions::south, Moves::right } },

              { { Directions::south,     KindOfPipe::vertical },        { Directions::south, Moves::straight } },
              { { Directions::south,     KindOfPipe::north_east_bend }, { Directions::east,  Moves::left } },
              { { Directions::south,     KindOfPipe::north_west_bend }, { Directions::west,  Moves::right } },

              { { Directions::west,      KindOfPipe::horizontal },      { Directions::west,  Moves::straight } },
              { { Directions::west,      KindOfPipe::north_east_bend }, { Directions::north, Moves::right } },
              { { Directions::west,      KindOfPipe::south_east_bend }, { Directions::south, Moves::left } }
            };

         using directions_to_moves = std::map<std::pair<Directions, Directions>, Moves>;
         static directions_to_moves inline turns = {
              { { Directions::north, Directions::north }, Moves::straight },
              { { Directions::east,  Directions::east  }, Moves::straight },
              { { Directions::south, Directions::south }, Moves::straight },
              { { Directions::west,  Directions::west  }, Moves::straight },
              { { Directions::north, Directions::east  }, Moves::right    },
              { { Directions::north, Directions::west  }, Moves::left     },
              { { Directions::east,  Directions::north }, Moves::left     },
              { { Directions::east,  Directions::south }, Moves::right    },
              { { Directions::south, Directions::east  }, Moves::left     },
              { { Directions::south, Directions::west  }, Moves::right    },
              { { Directions::west,  Directions::north }, Moves::right    },
              { { Directions::west,  Directions::south }, Moves::left     },
            };

         using rules_for_step_ty = std::map<Directions, std::pair<int, int>>;
         static rules_for_step_ty inline steps = {
              { Directions::north, {  0, -1 } },
              { Directions::east,  {  1,  0 } },
              { Directions::south, {  0,  1 } },
              { Directions::west,  { -1,  0 } }
            };

         using positions_ty    = std::pair<size_t, size_t>;
         using landscape_parts = std::tuple<KindOfPipe, Moves>;

         friend std::ostream& operator << (std::ostream& out, TLandscape::positions_ty const& data);
         friend std::wostream& operator << (std::wostream& out, TLandscape::positions_ty const& data);
         friend std::ostream& operator << (std::ostream& out, TLandscape::landscape_parts const& data);
         friend std::wostream& operator << (std::wostream& out, TLandscape::landscape_parts const& data);


         size_t iWidth = 0;
         size_t iHeight = 0;
         std::vector<landscape_parts> data;

         positions_ty theStartPos = { 0, 0 }; ///< start position for this landscape


         TLandscape() = default;


         size_t Index(size_t x, size_t y) const& {
            if (!(x < iWidth && y < iHeight))
               throw std::runtime_error(std::format("index ({}, {}) out of range ({}, {})", x, y, iWidth, iHeight));
            return y * iWidth + x;
            }

         bool IsPartOfPipe(size_t pos) const {
            if (auto it = parts.find(std::get<0>((*this)[pos])); it != parts.end())
               return std::get<0>(it->second);
            else throw std::runtime_error("unexpected rule to identify if position part of the pipe");
            }

         landscape_parts& operator [](size_t pos) {
            if(!(pos >= 0 && pos < data.size())) 
               throw std::runtime_error(std::format("index {} out of range ({}, {})", pos, 0, data.size()));
            return data[pos];
            }

         landscape_parts const& operator [](size_t pos) const {
            if (!(pos >= 0 && pos < data.size()))
               throw std::runtime_error(std::format("index {} out of range ({}, {})", pos, 0, data.size()));
            return data[pos];
            }

         landscape_parts& operator [](positions_ty const& pos) {
            return (*this)[Index(pos.first, pos.second)];
            }

         landscape_parts const& operator [](positions_ty const& pos) const {
            return (*this)[Index(pos.first, pos.second)];
            }

      public:
         TLandscape(TLandscape const&) = delete;
         TLandscape(TLandscape&&) noexcept = default;

         landscape_parts const& operator ()(size_t x, size_t y) const& {
            return (*this)[std::make_pair(x, y)];
            }

         landscape_parts const& operator ()(positions_ty const& pos) const {
            return (*this)[pos];
            }

         size_t Width() const { return iWidth; }
         size_t Height() const { return iHeight; } 
         positions_ty const& GetStart() const { return theStartPos; }
         KindOfPipe KindofStart() const { return std::get<0>((*this)[theStartPos]); }

         std::pair<Directions, Moves> FirstMove(positions_ty const& pos) const {
            if (auto it = moves.find({ Directions::undefined, std::get<0>((*this)[pos]) }); it != moves.end()) return it->second;
            else throw std::runtime_error("wrong first move.");
            }

         //Punkt-in-Polygon-Test nach Jordan
         size_t FloodFill() {
            auto pipe_parts     = parts | std::views::filter([](const auto& entry) { return std::get<0>(entry.second); })
                                        | std::views::transform([](const auto& entry) { return entry.first; })
                                        | std::ranges::to<std::vector>();

            auto not_pipe_parts = parts | std::views::filter([](const auto& entry) { return !std::get<0>(entry.second); })
                                        | std::views::transform([](const auto& entry) { return entry.first; })
                                        | std::ranges::to<std::vector>();

            auto vert_pip_part  = parts | std::views::filter([](const auto& entry) { return std::get<0>(entry.second) && 
                                                                                            std::get<2>(entry.second) && 
                                                                                            !std::get<1>(entry.second); })
                                        | std::views::transform([](const auto& entry) { return entry.first; })
                                        | std::ranges::to<std::set>();

            auto bend_pip_part = parts | std::views::filter([](const auto& entry) { return std::get<0>(entry.second) && 
                                                                                           std::get<1>(entry.second) && 
                                                                                           std::get<2>(entry.second); })
                                       | std::views::transform([](const auto& entry) { return entry.first; })
                                       | std::ranges::to<std::set>();

            for (size_t y = 0; y < iHeight; ++y) {
               std::span<landscape_parts> row(data.data() + y * iHeight, iWidth);
               for(auto borders = std::make_tuple(row.begin(), row.end(), 0u, true); 
                        std::get<0>(borders) != std::get<1>(borders); ++std::get<2>(borders)) {
                  auto it = std::find_first_of(std::get<0>(borders), std::get<1>(borders), pipe_parts.begin(), pipe_parts.end(),
                                               [](auto lhs, auto rhs) { return std::get<0>(lhs) == rhs; });
                  if(std::get<3>(borders) || it == std::get<1>(borders))
                     std::transform(std::get<0>(borders), it, std::get<0>(borders), [](auto p) { return landscape_parts { KindOfPipe::water, Moves::undefined }; });
                  std::get<0>(borders) = it;

                  if (std::get<0>(borders) < std::get<1>(borders)) {
                     if (auto end = std::find_first_of(std::get<0>(borders), std::get<1>(borders), not_pipe_parts.begin(), not_pipe_parts.end(),
                        [](auto lhs, auto rhs) { return std::get<0>(lhs) == rhs; }); end != std::get<1>(borders)) {
                        size_t vert_lines = 0;
                        Moves  lastTurn = Moves::undefined;
                        size_t offset = std::distance(row.begin(), std::get<0>(borders));
                        size_t len = std::distance(std::get<0>(borders), end);
                        std::ranges::for_each(row.subspan(offset, len), [&vert_lines, &lastTurn, &vert_pip_part, &bend_pip_part](auto p) mutable {
                           if (vert_pip_part.find(std::get<0>(p)) != vert_pip_part.end()) ++vert_lines;
                           if (bend_pip_part.find(std::get<0>(p)) != bend_pip_part.end()) {
                              auto currentTurn = std::get<1>(p);
                              if (currentTurn == Moves::undefined || currentTurn == Moves::straight)
                                 throw std::runtime_error("undefined turn");
                              else {
                                 if (lastTurn == Moves::undefined) lastTurn = currentTurn;
                                 else {
                                    if (currentTurn != lastTurn) ++vert_lines;
                                    // no else because equal turns declined
                                    lastTurn = Moves::undefined;
                                 }
                              }
                           }
                           });
                        if (vert_lines % 2 == 1) std::get<3>(borders) = !std::get<3>(borders);
                        std::get<0>(borders) = row.begin() + offset + len;
                     }
                     else break;
                  }
                  else break;
                  }
               }
            return std::ranges::count_if(data, [](auto part) { return std::get<0>(part) == KindOfPipe::ground; });
            }
 

            using route_ty = std::tuple<positions_ty, Directions, KindOfPipe, Moves>;

            class LandscapeView {
            private:
               TLandscape const& landscape;
            public:
               LandscapeView(TLandscape const& param) : landscape(param) {}
               LandscapeView(LandscapeView const& other) = default;

               class iterator {
                  using iterator_category = std::input_iterator_tag;
                  using value_type = route_ty;
                  using difference_type = std::ptrdiff_t;
                  using reference_type = value_type&;
                  using const_reference_type = value_type const&;
                  using pointer_type = value_type*;
                  using const_pointer_type = const value_type*;
               private:
                  TLandscape const& landscape;
                  value_type        position;
                  bool              uninitialized;
                  Directions        lastDirections;
               public:
                  iterator(TLandscape const& param, positions_ty pos, bool undefined) : landscape(param) {
                     auto step = landscape.FirstMove(pos);
                     std::get<0>(position) = pos;
                     std::get<1>(position) = std::get<0>(step);
                     std::get<2>(position) = std::get<0>(landscape[pos]);
                     std::get<3>(position) = std::get<1>(step);
                     lastDirections        = std::get<0>(step);
                     uninitialized         = undefined;
                     }

                  iterator(iterator const& other) = default;

                  iterator& operator = (iterator const& ref) {
                     position = ref.position;
                     return *this;
                  }

                  reference_type operator*() { return position; }
                  pointer_type operator->() { return &position; }

                  const_reference_type  operator*() const { return position; }
                  const_pointer_type operator->() const { return &position; }

                  iterator& operator++() {
                     auto FindStep = [](Directions dir, KindOfPipe pipe) {
                        if (auto it = moves.find({ dir, pipe }); it != moves.end()) return it->second;
                        else throw std::runtime_error("unexpected pipe with the current move.");
                        };

                     auto MakeStep = [this](positions_ty pos, Directions dir) mutable {
                        if (auto it = steps.find(dir); it != steps.end()) {
                           pos.first += it->second.first;
                           pos.second += it->second.second;
                           return pos;
                        }
                        else throw std::runtime_error("unexpected direction for the current step.");
                        };

                     if (std::get<1>(position) == Directions::undefined) {
                        if (landscape.GetStart() == std::get<0>(position)) {
                           auto step = landscape.FirstMove(std::get<0>(position));
                           std::get<1>(position) = std::get<0>(step);
                           }
                        else {
                           auto step = FindStep(Directions::undefined, std::get<0>(landscape[std::get<0>(position)]));
                           std::get<1>(position) = std::get<0>(step);
                           }
                        }

                     std::get<0>(position) = MakeStep(std::get<0>(position), std::get<1>(position));
                     std::get<2>(position) = std::get<0>(landscape[std::get<0>(position)]);
                     auto step = FindStep(std::get<1>(position), std::get<2>(position));
                     std::get<1>(position) = std::get<0>(step);
                     std::get<3>(position) = std::get<1>(step);
                    lastDirections = std::get<0>(step);
                     uninitialized = false;
                     return *this;
                  }

                  iterator& operator++(int) {
                     auto elem(*this);
                     ++*this;
                     return elem;
                  }

                  bool operator == (const iterator& other) const {
                     if (!uninitialized) return std::get<0>(position) == std::get<0>(other.position);
                     else return false;
                  }

                  bool operator != (const iterator& other) const {
                     if (!uninitialized && !other.uninitialized) return std::get<0>(position) != std::get<0>(other.position);
                     else return true;
                  }
               };

               iterator cbegin() const { return iterator(landscape, landscape.GetStart(), true); }
               iterator cend() const { return iterator(landscape, landscape.GetStart(), false); }

            };

            LandscapeView view() { return LandscapeView(*this); }


            static TLandscape CreateData(size_t w, size_t h, std::vector<route_ty>const& route = { }) {
               TLandscape ret;
               ret.iWidth = w;
               ret.iHeight = h;
               ret.data.assign(ret.iWidth * ret.iHeight, { KindOfPipe::ground, Moves::undefined });

               auto last = std::get<1>(route.back());

               for (auto const& [step, dir, pipe, turn] : route) ret[step] = { pipe, turn };
               return ret;
            }

            static positions_ty AddPos(positions_ty lhs, std::pair<int, int> rhs) {
               lhs.first += rhs.first;
               lhs.second += rhs.second;
               return lhs;
            }

         static TLandscape CreateData(std::vector<std::string_view> const& input) {
            TLandscape ret;
            ret.iWidth = input.begin()->size();
            ret.iHeight = input.size();
            ret.data.reserve(ret.iWidth * ret.iHeight);
            
            for (size_t i; auto const& line : input) {
               if (line.size() != ret.iWidth) throw std::runtime_error("unexpected input");
               std::transform(line.begin(), line.end(), std::back_inserter(ret.data), [](char p) { return landscape_parts { static_cast<KindOfPipe>(p), Moves::undefined }; });
               }

            if (auto it = std::find(ret.data.begin(), ret.data.end(), landscape_parts{ KindOfPipe::start, Moves::undefined }); it != ret.data.end()) {
               size_t pos = std::distance(ret.data.begin(), it);
               ret.theStartPos = { pos % ret.iWidth, pos / ret.iWidth };

               std::pair<Directions, Directions> start_pos = { Directions::undefined, Directions::undefined };
               if (ret.theStartPos.first > 0) { // left corner
                  auto pipe = std::get<0>(ret(ret.theStartPos.first - 1, ret.theStartPos.second));
                  if (auto it = rules.find(pipe); it != rules.end()) {
                     if (it->second.second == Directions::east) start_pos.second = Directions::west;
                     }
                  }
               if (ret.theStartPos.first + 1 < ret.iWidth) { // right corner
                  auto pipe = std::get<0>(ret(ret.theStartPos.first + 1, ret.theStartPos.second));
                  if (auto it = rules.find(pipe); it != rules.end()) {
                     if (it->second.first == Directions::west || it->second.second == Directions::west) {
                        if (start_pos.second == Directions::west) start_pos.first = Directions::west;
                        start_pos.second = Directions::east;
                        }
                     }
                  }

               if (start_pos.first == Directions::undefined) { // only possible with horizontal pipe
                  if (ret.theStartPos.second > 0) { // top corner
                     auto pipe = std::get<0>(ret(ret.theStartPos.first, ret.theStartPos.second - 1));
                     if (auto it = rules.find(pipe); it != rules.end()) {
                        if (it->second.first == Directions::south || it->second.second == Directions::south)
                           start_pos.first = Directions::north;
                        }
                     }
                  if (ret.theStartPos.second + 1 < ret.iHeight) { // bottom corner
                     auto pipe = std::get<0>(ret(ret.theStartPos.first, ret.theStartPos.second + 1));
                     if (auto it = rules.find(pipe); it != rules.end()) {
                        if (it->second.first == Directions::north) {
                           if (start_pos.first == Directions::north) start_pos.second = Directions::south;
                           else start_pos.first = Directions::south;
                           }
                        }
                     }
                  }

               if (start_pos.first == Directions::undefined || start_pos.second == Directions::undefined) 
                  throw std::runtime_error("couldn't determine the start pipe");

               if (auto it = std::find_if(rules.begin(), rules.end(), [&start_pos](auto const& p) { return p.second == start_pos; }); it != rules.end())
                  std::get<0>(ret[ret.theStartPos]) = it->first;
               else throw std::runtime_error("read error, pipe couldn't find in rules.");

               auto step = ret.FirstMove(ret.GetStart());
               std::get<1>(ret[ret.theStartPos]) = std::get<1>(step);
               }
            else {
               throw std::runtime_error("no starting position found");
               }
            
             return ret;
            }


      };


   inline std::ostream& operator << (std::ostream& out, TLandscape::positions_ty const& data) {
      return out << "(" << data.first << ", " << data.second << ")";
      }

   inline std::wostream& operator << (std::wostream& out, TLandscape::positions_ty const& data) {
      return out << L"(" << data.first << L", " << data.second << L")";
      }


   inline std::ostream& operator << (std::ostream& out, TLandscape::Directions data) {
      switch (data) {
      case TLandscape::Directions::undefined: out << "undefined"; break;
      case TLandscape::Directions::east:      out << "eastward";  break;
      case TLandscape::Directions::north:     out << "northward"; break;
      case TLandscape::Directions::west:      out << "westward";  break;
      case TLandscape::Directions::south:     out << "southward"; break;
      default: throw std::runtime_error("unexpected direction");
      }
      return out;
      }

   inline std::wostream& operator << (std::wostream& out, TLandscape::Directions data) {
      switch (data) {
         case TLandscape::Directions::undefined: out << L"undefined"; break;
         case TLandscape::Directions::east:      out << L"eastward";  break;
         case TLandscape::Directions::north:     out << L"northward"; break;
         case TLandscape::Directions::west:      out << L"westward";  break;
         case TLandscape::Directions::south:     out << L"southward"; break;
         default: throw std::runtime_error("unexpected direction");
         }
         return out;
      }

   inline std::ostream& operator << (std::ostream& out, TLandscape::Moves data) {
      switch(data) {
         case TLandscape::Moves::undefined: out << "undefined"; break;
         case TLandscape::Moves::straight:  out << "forward"; break;
         case TLandscape::Moves::right:     out << "right turn"; break;
         case TLandscape::Moves::left:      out << "left turn"; break;
         default: throw std::runtime_error("unexpected move in output");
         }
      return out;
      }

   inline std::wostream& operator << (std::wostream& out, TLandscape::Moves data) {
      switch (data) {
         case TLandscape::Moves::undefined: out << L'?'; break;
         case TLandscape::Moves::straight:  out << L'↑'; break;
         case TLandscape::Moves::right:     out << L'→'; break;
         case TLandscape::Moves::left:      out << L'←'; break;
         default: throw std::runtime_error("unexpected move in output");
         }
   }


   inline std::ostream& operator << (std::ostream& out, TLandscape::KindOfPipe data) { return out << static_cast<char>(data); }
   
   inline std::wostream& operator << (std::wostream& out, TLandscape::KindOfPipe data) {
      switch(data) {
         case TLandscape::KindOfPipe::vertical:        out << L'│'; break;
         case TLandscape::KindOfPipe::horizontal:      out << L'─'; break;
         case TLandscape::KindOfPipe::north_east_bend: out << L'└'; break;
         case TLandscape::KindOfPipe::north_west_bend: out << L'┘'; break;
         case TLandscape::KindOfPipe::south_west_bend: out << L'┐'; break;
         case TLandscape::KindOfPipe::south_east_bend: out << L'┌'; break;
         case TLandscape::KindOfPipe::ground:          out << L'.'; break;
         case TLandscape::KindOfPipe::start:           out << L'S'; break;
         case TLandscape::KindOfPipe::pipe:            out << L'■'; break;
         case TLandscape::KindOfPipe::water:           out << L'~'; break;
         case TLandscape::KindOfPipe::undefined:       out << L' '; break;
         default: throw std::runtime_error("unexpected KindOfPipe");
         }
      return out;
      }

   inline std::ostream& operator << (std::ostream& out, TLandscape::landscape_parts const& data) {
      return out << std::get<0>(data);
      }

   inline std::wostream& operator << (std::wostream& out, TLandscape::landscape_parts const& data) {
      return out << std::get<0>(data);
      }


   inline std::ostream& operator << (std::ostream& out, TLandscape const& data) {
      for (size_t i = 0; auto const& c : data.data) {
         out << c;
         if (!(++i < data.iWidth)) {
            out << '\n';
            i = 0;
            }
         }
      return out;
      }


   inline std::wostream& operator << (std::wostream& out, TLandscape const& data) {
      for (size_t i = 0; auto const& c : data.data) {
         out << c;
         if (!(++i < data.iWidth)) {
            out << L'\n';
            i = 0;
            }
         }
      return out;
      }



   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      std::cout << part << ".\n";

      try {

         auto input_vec = input | std::views::transform([](auto p) { return std::string_view{ p }; }) | std::ranges::to<std::vector>();
         auto test = TLandscape::CreateData(input_vec);
         auto step = test.FirstMove(test.GetStart());

         if(verbose) {
            std::cerr << "\nStart at " << test.GetStart() << " with kind " << test.KindofStart() 
                      << " in direction " << std::get<0>(step) << " / " << std::get<1>(step) << '\n';

            TWout_Wrapper w(std::wcerr);
            std::wcerr << L'\n' << test << L"\n\n";
            }

         std::vector<TLandscape::route_ty> data;
         auto const view = test.view();
         std::copy(view.cbegin(), view.cend(), std::back_inserter(data));

         if(verbose) {
            std::cerr << "\nlist with all pipe parts:\n";
            for(auto const& pos : data) {
               std::cerr << std::get<0>(pos) << " " << std::get<2>(pos) << " " << std::get<1>(pos) << " " << std::get<3>(pos) << '\n';
               Processing_Events();
               }
            }

         std::cout << "Part1: The opposite side is in " << data.size() / 2 << " steps.\n";

         auto target = TLandscape::CreateData(test.Width(), test.Height(), data);
         std::cout << "Part2: There are " << target.FloodFill() << " closed areas in the landscape.\n";

         if(verbose) {
            TWout_Wrapper w(std::wcerr); // attention, this wrapper connect TextEdit memOutput
            std::wcerr << L'\n' << target << L"\n\n";
         }
   
         }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 10, " << part << ": " << ex.what() << '\n';
      }
   }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "10th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 8th day.");
      }
   }

} // end of namespace
