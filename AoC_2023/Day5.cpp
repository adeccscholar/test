#include "aoc_lib.h"

#include <iomanip>
#include <string>
#include <stdexcept>
#include <vector>
#include <tuple>
#include <map>
#include <functional>
#include <format>
#include <ranges>
#include <limits>

using namespace std::placeholders;

namespace aoc2023_day5 {

   using id_ty = unsigned long long;
   using offset_ty = long long;

   template <EPart_of_Riddle part>
   using seeds_ty   = std::vector<std::conditional_t<part == 1, id_ty, std::pair<id_ty, size_t>>>;

   using bridge_ty  = std::tuple<id_ty, id_ty, offset_ty>;
   using bridges_ty = std::vector<bridge_ty>;

   bridges_ty seed_to_soil_map;
   bridges_ty soil_to_fertilizer_map;
   bridges_ty fertilizer_to_water_map;
   bridges_ty water_to_light_map;
   bridges_ty light_to_temperature_map;
   bridges_ty temperature_to_humidity_map;
   bridges_ty humidity_to_location_map;

   inline void read_bridges(bridges_ty& data, std::vector<std::string_view>const& lines) {
      data.clear();
      for(auto& line : lines) {
         auto readed = line | std::views::split(' ')
                            | std::views::filter([](auto p) { return p.size() > 0; })
                            | std::views::transform([](auto p) { return toInt<id_ty>(std::string_view{ p }); })
                            | std::ranges::to<std::vector<id_ty>>();
         if (readed.size() != 3) throw std::runtime_error(std::format("invalid input for bridge: ", line));
         data.emplace_back( std::make_tuple(readed[1], readed[1] + readed[2] - 1, static_cast<offset_ty>(readed[0] - readed[1])));
         }
      //std::ranges::sort(data, [](auto const& lhs, auto const& rhs) { return std::get<0>(lhs) < std::get<0>(rhs); });
      }


   template <EPart_of_Riddle part>
   void Solution(my_lines input, bool verbose) {
      seeds_ty<part>   seeds {};

      auto processing_block = [&](const std::string_view & first_line, std::vector<std::string_view>const& remaining_lines) {
         std::map<std::string_view, std::function<void(std::vector<std::string_view>const&)>> steps = {
               { "seed-to-soil map",            std::bind(read_bridges, std::ref(seed_to_soil_map),            _1) },
               { "soil-to-fertilizer map",      std::bind(read_bridges, std::ref(soil_to_fertilizer_map),      _1) },
               { "fertilizer-to-water map",     std::bind(read_bridges, std::ref(fertilizer_to_water_map),     _1) },
               { "water-to-light map",          std::bind(read_bridges, std::ref(water_to_light_map),          _1) },
               { "light-to-temperature map",    std::bind(read_bridges, std::ref(light_to_temperature_map),    _1) },
               { "temperature-to-humidity map", std::bind(read_bridges, std::ref(temperature_to_humidity_map), _1) },
               { "humidity-to-location map",    std::bind(read_bridges, std::ref(humidity_to_location_map),    _1) }
             };

         auto sep_pos = first_line.find(':');
         auto command_line = first_line.substr(0, sep_pos);

         if (command_line == "seeds") {
            seeds.clear();

            auto data = first_line.substr(sep_pos + 1, first_line.size() - sep_pos)
               | std::views::split(' ')
               | std::views::filter([](auto p) {return p.size() > 0; })
               | std::views::transform([](auto p) { return toInt<uint64_t>(std::string_view{ p }); })
               | std::ranges::to<std::vector>();

            if constexpr (part == EPart_of_Riddle::Part1) std::ranges::copy(data, std::back_inserter(seeds));
            else {
               std::ranges::transform(data | std::views::chunk(2), std::back_inserter(seeds), [](auto subrange) {
                          return std::make_pair(subrange[0], static_cast<size_t>(subrange[1]));
                          });
               }
            }
         else {
            if (auto it = steps.find(command_line); it != steps.end()) it->second(remaining_lines);
            else throw std::runtime_error(std::format("unexpected block \"{}\" in input.", command_line));
            }
         };



      std::cout << part << ".\n";

      auto input_vec = input | std::views::transform([](auto p) { return std::string_view{ p }; }) | std::ranges::to<std::vector>();
      auto blocks = input_vec | std::views::split(std::string_view{});
      for (auto const& block : blocks) {
         if (!block.empty()) {
            processing_block(*block.begin(), { std::next(block.begin()), block.end() });
            }
          }

      auto showSeeds = [](seeds_ty<part> const& seeds) {
         std::cerr << "seed:";
         for (auto const& seed : seeds) {
            if constexpr (part == EPart_of_Riddle::Part1)
               std::cerr << " " << seed;
            else
               std::cerr << " " << seed.first << "(" << seed.second << ")";;
         }
         std::cerr << '\n';
         };

      auto showMaps = [](std::string const& strText, bridges_ty const& values) {
         std::cerr << strText << '\n';
         for (auto const& [von, bis, offset] : values)
            std::cerr << "(" << std::setw(22) << von << " - " << std::setw(22) << bis << ") Offet: " 
                      << std::setw(22) << offset << '\n';
         std::cerr << '\n';
         };

      auto findValue = [](bridges_ty const& values, uint64_t value) {
         auto it = std::find_if(values.begin(), values.end(),
            [value](auto const& val) {
               return std::get<0>(val) <= value && value <= get<1>(val); });
         uint64_t ret = it != values.end() ? value + std::get<2>(*it) : value;
         return ret;
         };

      auto getSeedValue = [](auto seed) {
         if constexpr (part == EPart_of_Riddle::Part1) return seed;
         else return seed.first;
         };

      auto getSeedRange = [](auto seed) {
         if constexpr (part == EPart_of_Riddle::Part1) return 1;
         else return seed.second;
         };

      if(verbose) {
         showSeeds(seeds);
         showMaps("seed to soil", seed_to_soil_map);
         showMaps("soil to fertilizer", soil_to_fertilizer_map);
         showMaps("fertilizer to water", fertilizer_to_water_map);
         showMaps("water to light", water_to_light_map);
         showMaps("light to temperature", light_to_temperature_map);
         showMaps("temperature to humidity", temperature_to_humidity_map);
         showMaps("humidity to location", humidity_to_location_map);
         }

      uint64_t min_location = std::numeric_limits<uint32_t>::max();
      for(auto const& seed : seeds) {
         auto interval = std::views::iota(0) | std::views::take(getSeedRange(seed)) 
                           | std::views::transform([&seed, &getSeedValue](std::size_t i) { return getSeedValue(seed) + i; });
         for (auto seed_val :  interval ) {
            uint64_t soil        = findValue(seed_to_soil_map, seed_val);
            uint64_t fertilizer  = findValue(soil_to_fertilizer_map, soil);
            uint64_t water       = findValue(fertilizer_to_water_map, fertilizer);
            uint64_t light       = findValue(water_to_light_map, water);
            uint64_t temperature = findValue(light_to_temperature_map, light);
            uint64_t humidity    = findValue(temperature_to_humidity_map, temperature);
            uint64_t location    = findValue(humidity_to_location_map, humidity);

            if(verbose) {
               std::cerr << "seed:" << seed_val << " (" << getSeedValue(seed) << ")"
                         << " -> soil: " << soil << " -> fertilizer: " << fertilizer
                         << " -> water: " << water << " -> light: " << light << " -> temperature: " << temperature
                         << " -> humidity: " << humidity << " -> location: " << location << '\n';
               }
         if (min_location > location) min_location = location;
         }
      }
      std::cout << "the lowest id for location which used for the seed is " << min_location << ".\n";
      }

   void Riddle(int part, my_lines input, bool verbose) {
      std::cout << "5th Day, ";
         switch (part) {
         case 1: Solution<EPart_of_Riddle::Part1>(input, verbose); break;
         case 2: Solution<EPart_of_Riddle::Part2>(input, verbose); break;
         default: throw std::runtime_error("unexpexted part for the riddle at 6th day.");
         }
   }

} // end of namespace