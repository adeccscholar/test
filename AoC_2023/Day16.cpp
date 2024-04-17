#include "aoc_lib.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <stdexcept>
#include <vector>
#include <functional>
#include <algorithm>
#include <ranges>

#include "AoC_Matrix.h"

using namespace std::placeholders;

namespace aoc2023_day16 {

   class TLavaContraption {

      enum class EPieces : char {
         undefined = '_', empty = '.', horiz_splitter = '-', vertical_splitter = '|',
         slash_mirror = '/', backslash_mirror = '\\', out_of_map = 'X', already_energized = '!'
         };

      enum class EMotion : uint32_t { undefined = 0, rightward = 1, leftward = 2, upward = 4, downward = 8, stopped = 16 };
      enum class EMove   : char { undefined, straight, left_turn, right_turn, splitted, stopped };

      using power_state = uint32_t;

      using pieces_rules_ty = std::map<EPieces, std::tuple<bool, uint32_t>>;
      static pieces_rules_ty inline pieces_rules = {
         { EPieces::empty,             { true,     0 } },
         { EPieces::horiz_splitter,    { true,    12 } }, 
         { EPieces::vertical_splitter, { true,     3 } }, 
         { EPieces::slash_mirror,      { true,     0 } },
         { EPieces::backslash_mirror,  { true,     0 } }, 
         { EPieces::out_of_map,        { true,     0 } },
         { EPieces::already_energized, { true,     0 } }
      };

      // rules for beams moving in direction
      //   direction                                 offset      opposite            left                right              visible
      using motion_rules_ty = std::map<EMotion, std::tuple<MyMatrix::offset_ty, EMotion, EMotion, EMotion, bool>>;
      static motion_rules_ty inline motion_rules = {
         { EMotion::rightward, { MyMatrix::offset_ty {  1,  0 }, EMotion::leftward,  EMotion::upward,    EMotion::downward,  true } },
         { EMotion::leftward,  { MyMatrix::offset_ty { -1,  0 }, EMotion::rightward, EMotion::downward,  EMotion::upward,    true } },
         { EMotion::upward,    { MyMatrix::offset_ty {  0, -1 }, EMotion::downward,  EMotion::leftward,  EMotion::rightward, true } },
         { EMotion::downward,  { MyMatrix::offset_ty {  0,  1 }, EMotion::upward,    EMotion::rightward, EMotion::leftward,  true } },
         { EMotion::stopped,   { MyMatrix::offset_ty {  0,  0 }, EMotion::stopped,   EMotion::stopped,   EMotion::stopped,   false } }
      };

      using motion_piece_rules_ty = std::map<std::tuple<EMotion, EPieces>, EMove>;
      static motion_piece_rules_ty inline motion_piece_rules = {
         { { EMotion::leftward,  EPieces::empty },             EMove::straight },
         { { EMotion::leftward,  EPieces::horiz_splitter },    EMove::straight },
         { { EMotion::leftward,  EPieces::vertical_splitter }, EMove::splitted },
         { { EMotion::leftward,  EPieces::slash_mirror },      EMove::left_turn }, 
         { { EMotion::leftward,  EPieces::backslash_mirror },  EMove::right_turn }, 
         { { EMotion::leftward,  EPieces::out_of_map },        EMove::stopped },
         { { EMotion::leftward,  EPieces::already_energized }, EMove::stopped },


         { { EMotion::rightward, EPieces::empty },             EMove::straight },
         { { EMotion::rightward, EPieces::horiz_splitter },    EMove::straight },
         { { EMotion::rightward, EPieces::vertical_splitter }, EMove::splitted },
         { { EMotion::rightward, EPieces::slash_mirror },      EMove::left_turn },
         { { EMotion::rightward, EPieces::backslash_mirror },  EMove::right_turn },
         { { EMotion::rightward, EPieces::out_of_map },        EMove::stopped },
         { { EMotion::rightward, EPieces::already_energized }, EMove::stopped },

         { { EMotion::upward,    EPieces::empty },             EMove::straight },
         { { EMotion::upward,    EPieces::horiz_splitter },    EMove::splitted },
         { { EMotion::upward,    EPieces::vertical_splitter }, EMove::straight },
         { { EMotion::upward,    EPieces::slash_mirror },      EMove::right_turn },
         { { EMotion::upward,    EPieces::backslash_mirror },  EMove::left_turn },
         { { EMotion::upward,    EPieces::out_of_map },        EMove::stopped },
         { { EMotion::upward,    EPieces::already_energized }, EMove::stopped },

         { { EMotion::downward,  EPieces::empty },             EMove::straight },
         { { EMotion::downward,  EPieces::horiz_splitter },    EMove::splitted },
         { { EMotion::downward,  EPieces::vertical_splitter }, EMove::straight },
         { { EMotion::downward,  EPieces::slash_mirror },      EMove::right_turn },
         { { EMotion::downward,  EPieces::backslash_mirror },  EMove::left_turn },
         { { EMotion::downward,  EPieces::out_of_map },        EMove::stopped },
         { { EMotion::downward,  EPieces::already_energized }, EMove::stopped },

         { { EMotion::stopped,   EPieces::empty },             EMove::stopped },
         { { EMotion::stopped,   EPieces::horiz_splitter },    EMove::stopped },
         { { EMotion::stopped,   EPieces::vertical_splitter }, EMove::stopped },
         { { EMotion::stopped,   EPieces::slash_mirror },      EMove::stopped },
         { { EMotion::stopped,   EPieces::backslash_mirror },  EMove::stopped },
         { { EMotion::stopped,   EPieces::out_of_map },        EMove::stopped },
         { { EMotion::stopped,   EPieces::already_energized }, EMove::stopped }
      };


      using kind_of_contraption = std::tuple<EPieces, power_state>;

      class TBeam;
      using hitting_func_ty     = std::function<std::pair<EPieces, MyMatrix::positions_ty>(MyMatrix::positions_ty, EMotion)>;
      using adding_func_ty      = std::function<void(TBeam&&)>;

      struct TBeam {
      private:
         hitting_func_ty        funcHit;
         adding_func_ty         funcAdd;
         MyMatrix::positions_ty position = { 0ull, 0ull };
         EMotion                motion = EMotion::undefined;
      public:
         TBeam(TBeam const&) = default;
         TBeam(TBeam&&) noexcept = default;
         TBeam(hitting_func_ty pHit, adding_func_ty pAdd, MyMatrix::positions_ty pPosition, EMotion pMotion) : funcHit(pHit), funcAdd(pAdd), position(pPosition), motion(pMotion) { }

         TBeam& operator = (TBeam const&) = default;
         TBeam& operator = (TBeam&&) noexcept = default;

         bool Move() {
            if (auto move_rule = motion_rules.find(motion); move_rule != motion_rules.end()) {
               //std::cerr << position << " -> ";
               MyMatrix::offset_ty offset = std::get<0>(move_rule->second);
               if(auto newPos = MyMatrix::Add(position, offset); newPos) { 
                  auto const& [piece, PosAfter] = funcHit(*newPos, motion);
                  position = PosAfter;
                  //std::cerr << position << " bumb in " << static_cast<char>(piece) << '\n';
                  if (auto piece_rule = motion_piece_rules.find({ motion, piece }); piece_rule != motion_piece_rules.end()) {
                     // undefined, straight, left_turn, right_turn, splitted, stopped
                     switch(piece_rule->second) {
                        case EMove::straight:   break;
                        case EMove::left_turn:  motion = std::get<2>(move_rule->second); 
                                                break;
                        case EMove::right_turn: motion = std::get<3>(move_rule->second); 
                                                break;
                        case EMove::splitted: { motion = std::get<3>(move_rule->second);
                                                //if (auto rule = motion_rules.find(std::get<2>(move_rule->second)); rule != motion_rules.end()) {
                                                //   MyMatrix::offset_ty offset = std::get<0>(rule->second);
                                                //   if (auto newPos = MyMatrix::Add(position, offset); newPos) {
                                                      //funcAdd(TBeam(funcHit, funcAdd, *newPos, std::get<2>(move_rule->second)));
                                                      funcAdd(TBeam(funcHit, funcAdd, position, std::get<2>(move_rule->second)));
                                                 //     }
                                                 //  }
                                              }  break;
                        case EMove::stopped:    motion = EMotion::stopped;
                                                break;
                        default: throw std::runtime_error("can't handle this rule.");
                        }
                     }
                  else throw std::runtime_error("undefined rule for motion to piece, critical error.");
                  }
               else {
                  motion = EMotion::stopped;
                  }
               }
            else throw std::runtime_error("undefined motion in control data for moves, critical error.");
            return Running();
            }

         bool Running() const { return motion != EMotion::stopped && motion != EMotion::undefined; }
      };

      MyMatrix::TFixedMatrix<kind_of_contraption> data;
      std::vector<TBeam> beams;
      std::vector<TBeam> new_beams;

   public:
      TLavaContraption() = default;

      void run() {
         size_t count = 0;
         while(beams.size() > 0 ) { //}&& ++count < 1000) {
            std::erase_if(beams, [](auto const& e) { return !e.Running(); } );
            if(new_beams.size() > 0) {
               std::ranges::copy(new_beams, std::back_inserter(beams));
               new_beams.clear();
               }
            for (auto& beam : beams) {
               bool cont;
               do {
                  cont = beam.Move();
                  } 
               while (cont);
               }
            }
         }

      void start() {
         //auto const& [piece, PosAfter] = Hit({ 0, 0 }, EMotion::rightward);
         beams.insert(beams.begin(), TBeam(std::bind(&TLavaContraption::Hit, this, _1, _2),
                                           std::bind(&TLavaContraption::AddBeam, this, _1), { 0, 0 }, EMotion::rightward));
         }

      auto count() const {
         return data.count([](auto const e) { return std::get<1>(e) != 0; });
         }

      std::pair<EPieces, MyMatrix::positions_ty> Hit(MyMatrix::positions_ty pos, EMotion step_motion) {
         if(!(pos.first < data.Width() && pos.second < data.Height())) return { EPieces::out_of_map, pos };
           
         auto space = [&]() -> std::vector<MyMatrix::positions_ty> {
                          switch(step_motion) {
                             case EMotion::rightward:
                                return data.GetRow(pos.second) 
                                   | std::views::filter([&pos](auto const& p) { return p.first >= pos.first; })
                                   | std::views::transform([](auto p) { return MyMatrix::positions_ty{ p }; } )
                                   | std::ranges::to<std::vector>();
                             case EMotion::leftward:
                                return data.GetRow(pos.second) 
                                   | std::views::filter([&pos](auto const& p) { return p.first <= pos.first; })
                                   | std::views::reverse
                                   | std::views::transform([](auto p) { return MyMatrix::positions_ty{ p }; })
                                   | std::ranges::to<std::vector>();
                             case EMotion::downward:
                                return data.GetCol(pos.first)
                                   | std::views::filter([&pos](auto const& p) { return p.second >= pos.second; })
                                   | std::views::transform([](auto p) { return MyMatrix::positions_ty{ p }; })
                                   | std::ranges::to<std::vector>();
                             case EMotion::upward:
                                return data.GetCol(pos.first)
                                   | std::views::filter([&pos](auto const& p) { return p.second <= pos.second; })
                                   | std::views::reverse
                                   | std::views::transform([](auto p) { return MyMatrix::positions_ty{ p }; })
                                   | std::ranges::to<std::vector>();
                             default: return { };
                             }
                       }();
         
         if(auto it = std::find_if(space.begin(), space.end(), [this, &pos](auto pos) mutable {
                                                                  auto piece = std::get<0>(data[pos]);
                                                                  if (piece == EPieces::empty ||
                                                                      piece == EPieces::already_energized ||
                                                                      piece == EPieces::out_of_map) return false;
                                                                  else return true;
                                                                  }); it != space.end()) {
            bool boArrive = std::all_of(space.begin(), it, [this, step_motion](auto pos) mutable {
                       if (std::get<0>(data[pos]) == EPieces::already_energized || std::get<0>(data[pos]) == EPieces::out_of_map) return false;
                       else {
                          if (std::get<1>(data[pos]) & static_cast<uint32_t>(step_motion)) return false;
                          else std::get<1>(data[pos]) |= static_cast<uint32_t>(step_motion);
                          return true;
                          }
                       });
            if (!boArrive) return { EPieces::already_energized, pos };
            else pos = *it;
            }
         else {
            return { EPieces::out_of_map, space.back() };
            }
         
         auto& [piece, energized] = data[pos];
         if (piece == EPieces::already_energized || piece == EPieces::out_of_map) return { piece, pos };
         else {
            if (energized & static_cast<uint32_t>(step_motion)) return { EPieces::already_energized, pos };
            else {
               if (auto energy_rule = pieces_rules.find(piece); energy_rule != pieces_rules.end()) {
                  auto const& [can_energized, max_energy] = energy_rule->second;
                  if (max_energy && max_energy & static_cast<uint32_t>(step_motion)) energized |= max_energy;
                  }
               else throw std::runtime_error("missing rule for energize the piece.");
               }
            }

         /*
         if (auto energy_rule = pieces_rules.find(piece); energy_rule != pieces_rules.end()) {
            auto const& [can_energized, max_energy] = energy_rule->second;
            if (can_energized) {
               if(max_energy & static_cast<uint32_t>(step_motion)) {
                  if (!(energy & static_cast<uint32_t>(step_motion))) {
                     energy |= static_cast<uint32_t>(step_motion);
                     }
                  if (energy == max_energy) return { EPieces::already_energized, pos };
                  else return { piece, pos };
                  }
               else {
                  energy |= static_cast<uint32_t>(step_motion);
                  if (energy == max_energy) return { EPieces::already_energized, pos };
                  else return { piece, pos };
                  }
               }
            }
         */
         return { piece, pos };
         }

      void PrintEnergy(std::ostream& out) const {
         for(size_t y = 0; y < data.Height();++y) {
            for (auto const& pos : data.GetRow(y)) out << (std::get<1>(data[pos]) > 0 ? '#' : ' ');
            out << '\n';
            }
         }

      void PrintMap(std::ostream& out) const {
         for (size_t y = 0; y < data.Height(); ++y) {
            for (auto const& pos : data.GetRow(y)) out << static_cast<char>(std::get<0>(data[pos]));
            out << '\n';
            }
         }

      void AddBeam(TBeam&& newBeam) {
         new_beams.emplace_back(std::move(newBeam));
         }

      void Insert_Beam(TBeam&& newBeam) {
         }

      void Read(my_lines input) {
         auto input_vec = input | std::views::transform([](auto p) { return std::string_view{ p }; }) | std::ranges::to<std::vector>();
         data = MyMatrix::TFixedMatrix<kind_of_contraption>::CreateMatrix(input_vec.begin()->size(), input_vec.size(), { EPieces::empty, 0 });
         for (size_t i = 0; auto const& line : input) {
            if (line.size() != data.Width()) throw std::runtime_error(std::format("unexpected input size in line {}, size is {}, expected was {}", i, line.size(), data.Width()));
            std::vector<kind_of_contraption> values(data.Width());
            std::ranges::transform(line, values.begin(), [](char p) { return kind_of_contraption{ static_cast<EPieces>(p), 0 }; });
            data.FillLine(i++, values);
            }
         }
   };
      

   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      std::cout << part << ".\n";

      try {
         TLavaContraption data;
         data.Read(input);
         data.PrintMap(std::cout);
         data.start();
         data.run();
         data.PrintEnergy(std::cerr);
         std::cout << "solution = " << data.count() << '\n';
      }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 16, " << part << ": " << ex.what() << '\n';
      }
   }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "16th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 8th day.");
      }
   }

} // end of namespace
