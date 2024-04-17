#pragma once

#include <vector>
#include <stdexcept>
#include <optional>
#include <format>
#include <ranges>

namespace MyMatrix {

using positions_ty = std::pair<size_t, size_t>;
using offset_ty    = std::pair<int64_t, int64_t>;



inline std::optional<positions_ty> Add(positions_ty const& lhs, offset_ty const& rhs) {
   if((rhs.first < 0 && std::abs(rhs.first) > lhs.first) || (rhs.second < 0 && std::abs(rhs.second) > lhs.second)) return {};
   return positions_ty {  lhs.first + rhs.first, lhs.second + rhs.second };
   }

template <class Element_ty>
class TFixedMatrix {
   friend void swap(TFixedMatrix& lhs, TFixedMatrix& rhs) noexcept { lhs.swap(rhs); }
   private:
      size_t iWidth            = 0;        ///< 'iWidth' corresponds to the number of columns in the matrix."
      size_t iHeight           = 0;        ///< 'iHeight' corresponds to the number of lines in the matrix."
      std::vector<Element_ty> vault;       ///< 'vault' represents the data structure for the matrix, implemented as a one-dimensional vector.

   public:
      TFixedMatrix() = default; 

      TFixedMatrix(TFixedMatrix const& other) { copy(other); }
      TFixedMatrix(TFixedMatrix&& other) noexcept { swap(other); }
      ~TFixedMatrix() = default;
         
      TFixedMatrix& operator = (TFixedMatrix const& other) {
         copy(other);
         return *this;
         }

      TFixedMatrix& operator = (TFixedMatrix&& other) {
         swap(other);
         return *this;
         }


      Element_ty& operator [](size_t pos) {
         if (!(pos >= 0 && pos < vault.size()))
            throw std::runtime_error(std::format("index {} for the matrix out of range ({}, {})", pos, 0, vault.size()));
         return vault[pos];
         }

      Element_ty const& operator [](size_t pos) const {
         if (!(pos >= 0 && pos < vault.size()))
            throw std::runtime_error(std::format("index {} for the matrix is out of range ({}, {})", pos, 0, vault.size()));
         return vault[pos];
         }

      Element_ty& operator [](positions_ty const& pos) {
         return (*this)[Index(pos.first, pos.second)];
         }

      Element_ty const& operator [](positions_ty const& pos) const {
         return (*this)[Index(pos.first, pos.second)];
         }

      size_t Width() const { return iWidth; }  
      size_t Height(void) const { return iHeight; }

      std::vector<positions_ty> GetCol(size_t x) const {
         if (!(x < iWidth)) throw std::runtime_error(std::format("the requested index for x = {0:} is out of range (0, {1:}) for this matrix ({1:}, {2:})", x, iWidth, iHeight));
         return std::views::iota(0ull, iHeight) | std::views::transform([x](auto y) { return positions_ty { x, y }; }) 
                                             | std::ranges::to<std::vector>();
         }


      std::vector<positions_ty> GetRow(size_t y) const {
         if (!(y < iHeight)) throw std::runtime_error(std::format("the requested index for y = {0:} is out of range (0, {2:}) for this matrix ({1:}, {2:}) ", y, iWidth, iHeight));
         return std::views::iota(0ull, iWidth) | std::views::transform([y](auto x) { return positions_ty { x, y }; }) 
                                            | std::ranges::to<std::vector>();
         }
      
      void FillLine(size_t y, std::vector<Element_ty> values) {
         if (!(y < iHeight)) throw std::runtime_error(std::format("can't fill the line {} in a matrix with {} lines.", y, iHeight));
         if (values.size() != iWidth) throw std::runtime_error(std::format("Not possible to fill {} values in the line with {} elements.", values.size(), iWidth));
         std::ranges::copy(values, vault.begin() + Index(0, y));
         }
   
      static TFixedMatrix<Element_ty> CreateMatrix(size_t x, size_t y, Element_ty elem) {
         TFixedMatrix<Element_ty> ret;
         ret.iWidth = x;
         ret.iHeight = y;
         ret.vault.resize(x * y, elem);
         return ret;
         }

      auto count(std::function<bool (Element_ty const&)> func) const {
         return std::count_if(vault.begin(), vault.end(), func);
         }

   private:
      void swap(TFixedMatrix& other) noexcept {
         std::swap(iWidth, other.iWidth);
         std::swap(iHeight, other.iHeight);
         vault.swap(other.vault);
         }

      void copy(TFixedMatrix const& other) {
         bool toShrink = iWidth * iHeight > other.iWidth * other.iHeight ? true : false;
         iWidth = other.iWidth;
         iHeight = other.iHeight;
         vault.resize(iWidth * iHeight);
         if (toShrink) vault.shrink_to_fit();
         std::ranges::copy(other.vault, vault);
         }

      /// Methode to calculate the one dimensional index for a position with the paramter 'x' and 'y'
      size_t Index(size_t x, size_t y) const& {
         if (!(x < iWidth && y < iHeight))
            throw std::runtime_error(std::format("index ({}, {}) out of range for the matrix ({}, {})", x, y, iWidth, iHeight));
         return y * iWidth + x;
         }

};

} // end of namespace myMatrix

inline std::ostream& operator << (std::ostream& out, MyMatrix::positions_ty const& val) {
   return out << "(" << val.first << ", " << val.second << ')';
   }