#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <string_view>
#include <charconv>
#include <filesystem>
#include <atomic>
#include <concepts>

#include <QCoreApplication>


using namespace std::literals::string_literals;

const std::string strNumbers1 = "123456789"s;
const std::string strNumbers2 = "0123456789"s;
const bool boWithTrace = true;

enum EPart_of_Riddle : uint8_t { Part1 = 1, Part2 = 2 };

inline void Processing_Events() {
   QCoreApplication::processEvents();
   }

inline std::ostream& operator << (std::ostream& out, EPart_of_Riddle const& part) {
   switch (part) {
      case EPart_of_Riddle::Part1: out << "1st part"; break;
      case EPart_of_Riddle::Part2: out << "2nd part"; break;
      default: throw std::domain_error("unexpected value for parts of the riddles.");
      }
   return out;
   }

template<bool trace = boWithTrace>
void Trace(std::string const& text, std::ostream& out = std::cerr) {
   if constexpr (trace == true) {
      out << text;
      }
   }

static constexpr auto input = [](std::string const& strFile) {
   std::ifstream ifs;
   ifs.open(strFile);
   if (!ifs.is_open()) {
      std::ostringstream os;
      os << "File " << strFile << " couldn't opened.";
      throw std::runtime_error(os.str());
      }
   return ifs;
   };

/*
inline std::string to_String(std::string_view str) {
   return std::string{ str.data(), str.size() }; 
   }
*/
template <typename ty>
std::string to_String(ty const& str) {
   return std::format("{}", str);
   }


inline std::string_view trim(std::string_view str) {
   size_t pos1 = str.find_first_not_of(" \t");
   return str.substr(pos1, str.size() - pos1);
}

template <typename ty>
concept my_integral_ty = std::is_integral_v<ty> && !std::is_same_v<ty, bool>;

template <my_integral_ty ty>
ty toInt(std::string_view str) {
   static auto constexpr toString = [](std::string_view str) { return std::string{ str.data(), str.size() }; };
   std::size_t pos{};
   if constexpr (std::is_same_v<ty, long long>) return stoll(toString(str), &pos);
   else if constexpr (std::is_same_v<ty, unsigned long long>) return std::stoull(toString(str), &pos);
   else {
      ty result{};
      auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

      if (ptr == str.data() + str.size()) [[likely]] {
         switch (ec) {
            case std::errc(): return result;
            case std::errc::invalid_argument:  throw std::invalid_argument("The given argument '"s + toString(str) + "' is invalid"s);
            case std::errc::result_out_of_range: throw std::out_of_range("The result for '"s + toString(str) + "' isn't in range that can be represented for int values."s);
            default: throw std::runtime_error("unexpected eror in method from_chars for input '"s + toString(str) + "'."s);
            }
         }
      else throw std::invalid_argument("Not all characters in '"s + toString(str) + "' could be converted."s);
      }
   }

template <my_integral_ty ty>
ty toInt(std::string const& str) {
   return toInt<ty>(std::string_view{ str.data(), str.size() });
   }

inline auto GetContent(std::filesystem::path const& strFile, std::string& strBuffer) {
   std::ifstream ifs(strFile);
   ifs.exceptions(std::ios_base::badbit);
   if (!ifs.is_open()) [[unlikely]] throw std::runtime_error("file \""s + strFile.string() + "\" can't opened"s);
   else {
      const auto iSize = std::filesystem::file_size(strFile);
      strBuffer.resize(iSize);
      ifs.read(strBuffer.data(), iSize);
      return std::string_view { strBuffer.data(), strBuffer.size() };
      }
   }

struct my_line_iterator {
   using iterator_category = std::input_iterator_tag;
   using value_type        = std::string_view;
   using difference_type   = std::ptrdiff_t;
   using reference_type    = value_type const&;
   using pointer_type      = const value_type*;

   my_line_iterator() { };
   my_line_iterator(std::string_view const& input) : theText(input) { ++*this; };

   my_line_iterator& operator = (std::string_view const& vw) {
      theText = vw;
      start_pos = 0u;
      end_pos = 0u;
      return *this;
      }

   my_line_iterator& operator = (my_line_iterator const& ref) {
      theText = ref.theText;
      theLine = ref.theLine;
      start_pos = ref.start_pos;
      end_pos = ref.end_pos;
      return *this;
      }

   reference_type operator*() const { return theLine; }
   pointer_type operator->() const { return &theLine; }

   my_line_iterator& operator++() {
      if (theText) {
         end_pos = theText->find('\n', start_pos);
         if (end_pos != std::string_view::npos) {
            theLine = theText->substr(start_pos, end_pos - start_pos);
            start_pos = end_pos + 1;
            }
         else {
            //theText = { };
            theText = std::nullopt;
            }
         }
      return *this;
      }

   my_line_iterator operator++(int) {
      auto elem(*this);
      ++*this;
      return elem;
      }

   friend  bool operator == (my_line_iterator const& lhs, my_line_iterator const& rhs) {
      return lhs.theText == rhs.theText;
      }

   friend  bool operator != (my_line_iterator const& lhs, my_line_iterator const& rhs) {
      return !(lhs == rhs);
      }

   std::string AsString() {
      if (!theText) throw std::runtime_error("text isn't initialize.");
      else return to_String(trim(*theText));
      }

   private:
      std::optional<std::string_view> theText = { };
      std::string_view                theLine;
      size_t                          start_pos = 0u, end_pos = 0u;
   };

struct my_lines {
   my_lines(std::string_view const& input) { theText = input; }
   my_lines(my_lines const& ref) { theText = ref.theText; }

   my_line_iterator begin() const { return my_line_iterator(theText); }
   my_line_iterator end() const { return my_line_iterator(); }


   std::string_view theText;

   };

struct my_line {
   my_line(void) : view() { }
   my_line(std::string_view const& input) : view(input) { }
   std::string_view view;
   };

struct my_line_count {
   my_line_count(void) : view(), index(counter++) { }
   my_line_count(std::string_view const& input) : view(input), index(counter++) { }

   std::string_view view;
   int index;

   static inline std::atomic<int> counter = 0;
   static int GetCounter(void) { return counter; }
   static void reset(void) { counter = 0; }
};
