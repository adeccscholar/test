#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <format>
#include <numeric>
#include <algorithm>

void Test() {
   std::string test = "rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7";
   auto seq = test | std::views::split(',') 
                   | std::views::transform([](auto v) { return std::string{ v.begin(), v.end() }; })
                   | std::ranges::to<std::vector>();

   auto sum = accumulate(seq.begin(), seq.end(), 0ull, [](unsigned long long sum, std::string const& p) {
      auto val = accumulate(p.begin(), p.end(), 0ull, [](unsigned long long sum, char c) {
         sum += static_cast<uint64_t>(c);
         sum *= 17;
         sum = sum % 256;
         return sum;
         });
      return sum += val;
      });
   std::cout << sum << "\n";
 }

