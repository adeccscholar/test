


            std::optional<size_t> Scan(EDirection dir) {
               if (dir == EDirection::horizontal) {
                  auto horiz = std::views::iota(0u, iWidth - 1)
                     | std::views::filter([this](auto col) {
                     auto rows = std::views::iota(0u, iHeight) | std::ranges::to<std::vector>();
                     return std::all_of(rows.begin(), rows.end(), [this, col](auto row) {
                        return data[Index(col, row)] == data[Index(col + 1, row)];
                        });
                        })
                     | std::ranges::to<std::vector>();
                        if (horiz.size() == 0) return {};
                        else return horiz[0];
               }
               else if (dir == EDirection::vertical) {
                  auto vert = std::views::iota(0u, iHeight) | std::views::slide(2)
                     | std::views::filter([this](auto subrange) {
                     size_t a = subrange[0];
                     size_t b = subrange[1];
                     std::span<EElements> row1(data.data() + a * iWidth, iWidth);
                     std::span<EElements> row2(data.data() + b * iWidth, iWidth);
                     std::cerr << "1: ";
                     for (auto c : row1) std::cerr << static_cast<char>(c);
                     std::cerr << "\n2: ";
                     for (auto c : row2) std::cerr << static_cast<char>(c);
                     std::cerr << "\n";
                     bool ret = std::ranges::equal(row1, row2, [](auto l, auto r) {
                        return l == r; });
                     return ret;
                        })
                     | std::views::transform([](auto const& subrange) {
                           return static_cast<size_t>(subrange[0]);
                        })
                           | std::ranges::to<std::vector>();

                        if (vert.size() == 0) return {};
                        else return vert[0];
               }
               else throw std::runtime_error("unexpected direction for scan");
               }
