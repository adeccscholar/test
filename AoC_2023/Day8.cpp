#include "aoc_lib.h"

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <stdexcept>
#include <regex>
#include <iterator>
#include <ranges>
#include <coroutine>
#include <variant>


namespace aoc2023_day8 {

   class repeat_sequence {
   public:
      explicit repeat_sequence(std::string str) : sequence_(std::move(str)) {}

      struct iterator {
         const repeat_sequence& sequence;
         std::size_t index = 0;

         bool operator!=(const iterator& other) const noexcept {
            return index != other.index;
            }

         iterator& operator++() noexcept {
            ++index;
            if (index >= sequence.sequence_.size()) {
               index = 0;
               }
            return *this;
            }

         char operator*() const noexcept {
            return sequence.sequence_[index];
            }
         };

      // Coroutine-Handle für die Iteration
      struct repeat_sequence_handle {
         const repeat_sequence& sequence;

         bool await_ready() const noexcept {
            return false; // Coroutine ist immer bereit
            }

         void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
            // Hier könnten Sie asynchrone Operationen durchführen
            // In diesem Beispiel ist keine asynchrone Suspendierung erforderlich
            }

         iterator await_resume() const noexcept {
            return { sequence, 0 };
            }
      };

      // Funktion, um den Anfang des Bereichs zu erhalten
      iterator begin() const {
         return { *this, 0 };
         }

      // Funktion, um das Ende des Bereichs zu erhalten
      iterator end() const {
         return { *this, sequence_.size() };
         }

   private:
      std::string sequence_;
   };


   template <typename ty>
   using VarTy = std::variant<size_t, ty>;

   template <template <typename> class K>
   struct Fixed : K<Fixed<K>> {
      using K<Fixed>::K;
   };

   


  using network_points_ty = std::map<std::string, int>;

  using data_ty = std::tuple<std::string, size_t, size_t, bool, bool>;
  using network_ty = std::map<size_t, data_ty>;

  using network_vec = std::vector <data_ty>;

   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      std::cout << part << ".\n";

      try {
         std::string command;
         for(auto const& line : input | std::ranges::views::take(1) | std::views::transform([](auto p) { return std::string_view{ p }; })) {
            command = to_String(line);
            }

         auto repeat_commands = repeat_sequence(command);

         std::cout << '\n';

         network_points_ty network_points;
         //network_ty network;
         network_vec network;

         // regulärer Ausdruck zum Lesen der Schlüssel
         static std::regex key_pattern(R"(^([A-Z]{3})\s*)");
         auto key_vector_input = input | std::ranges::views::drop(2) 
                                       | std::views::transform([](auto p) {
                                               auto text = std::string_view{ p };
                                               auto pos  = text.find('=');
                                               return std::string_view{ p }.substr(0, pos); })
                                       | std::ranges::to<std::vector<std::string_view>>();

         std::ranges::for_each(std::views::zip(input | std::ranges::views::drop(2) , std::views::iota(0)),
            [&](const auto& tuple) {
               auto [current_str, current_id] = tuple;
               std::string_view key_vw = current_str.substr(0, current_str.find('='));
               key_vw = key_vw.substr(0, key_vw.find_last_not_of(" ") + 1);
               std::string key_str;
               std::ranges::transform(key_vw, std::back_inserter(key_str), [](char c) { return !std::isspace(c) ? c : '\0'; });
               network_points.insert( { key_str, static_cast<uint16_t>(current_id) } );
            });

         network.reserve(network_points.size());

         //static std::regex pattern(R"(^([A-Z]{3})\s*=\s*\(([A-Z]{3}),\s*([A-Z]{3})\)$)");
         static std::regex pattern(R"(^([A-Z1-9]{3})\s*=\s*\(([A-Z1-9]{3}),\s*([A-Z1-9]{3})\)$)");
         for (auto const& line : input | std::ranges::views::drop(2) | std::views::transform([](auto p) { return std::string_view{ p }; })) {
            std::string strLine = to_String(line);
            std::smatch matches;
            if (std::regex_match(strLine, matches, pattern)) {
               std::string node         = matches[1];
               std::string left_target  = matches[2];
               std::string right_target = matches[3];

               if(verbose) {
                  std::cerr << "node: " << node << ", left: " << left_target << ", right: " << right_target << std::endl;
                  if (node == "AAA" || node == "ZZZ") std::cerr << " --------> " << strLine << '\n';
                  }


               static auto Starting = [](std::string const& node) {
                  if constexpr (part == EPart_of_Riddle::Part1)
                     return node == "AAA";
                  else
                     return node.back() == 'A';
                  };

               static auto Finishing = [](std::string const& node) {
                  if constexpr (part == EPart_of_Riddle::Part1)
                     return node == "ZZZ";
                  else
                     return node.back() == 'Z';
                  };

               auto find_id = [&network_points](std::string const& node) -> size_t {
                  if (auto it = network_points.find(node); it != network_points.end()) return it->second;
                  else throw std::runtime_error("critical error, node not found");
                  };

               //network.insert({ find_id(node), { node, find_id(left_target), find_id(right_target), Starting(node), Finishing(node) } });
               network.emplace_back(data_ty { node, find_id(left_target), find_id(right_target), Starting(node), Finishing(node) });
            }
            else {
               std::cout << "error\n";
               }
            }

         if(verbose) {
            std::cerr << "Starting points:\n";
            for (const auto& element : network) {
               //if (std::get<3>(element.second) == true) std::cerr << std::get<0>(element.second) << '\n';
               if (std::get<3>(element) == true) std::cerr << std::get<0>(element) << '\n';
            }
            std::cerr << "Finishing points:\n";
            for (const auto& element : network) {
               //if (std::get<4>(element.second) == true) std::cerr << std::get<0>(element.second) << '\n';
               if (std::get<4>(element) == true) std::cerr << std::get<0>(element) << '\n';
            }
            }

         size_t iCount = 0;

         std::vector<size_t> start_points;
         
         //for(auto it = network.begin(); it != network.end(); ++it) {
            //if(std::get<3>(it->second)) start_points.emplace_back(it);
         for (size_t pos = 0; auto const& it : network) {
            if (std::get<3>(it)) start_points.emplace_back(pos);
            ++pos;
            }

         /*
         std::cout << "Startpoints: Size = " << start_points.size() << "\n";
         for (auto& it : start_points) {
            std::cout << get<0>(network[it]) << '\n';
            }
         */

         auto test_finishing = [&network](auto const& iterators) {
            for (auto const& it : iterators) {
               if(std::get<4>(network[it]) == false) {
                  return false;
                  }
               }
            return true;
            };

         QCoreApplication::processEvents();
         for (auto ch : repeat_commands) {
            //for(auto& it : start_points) it = network.find(ch == 'L' ? std::get<1>(it->second) : std::get<2>(it->second));
            for (auto& it : start_points) it = (ch == 'L' ? std::get<1>(network[it]) : std::get<2>(network[it]));
            ++iCount;
            if(iCount % 100000000 == 0) {
               std::cout << std::setw(20) << iCount << ":";
               for (auto const& it : start_points) std::cout << " " << std::get<0>(network[it]);
               std::cout << '\n';
               QCoreApplication::processEvents();
               }
            if (test_finishing(start_points)) break;
            }

         std::cout << "Count of nodes = " << network.size() << '\n';
         std::cout << "solution = " << iCount << '\n';
         }
      catch (std::exception& ex) {
         std::cerr << "error in program for day 8, " << part << ": " << ex.what() << '\n';
      }
   }


   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "8th Day, ";
      switch (part) {
      case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
      case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
      default: throw std::runtime_error("unexpexted part for the riddle at 8th day.");
      }
   }

} // end of namespace
