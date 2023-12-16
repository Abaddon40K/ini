#pragma once
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

/*
 *  TODO: Добавить политики для ini
 */

namespace ini {
  constexpr inline char        comment_begin = '#';
  constexpr inline char        section_begin = '[';
  constexpr inline char        section_end   = ']';
  constexpr inline char        separator     = '=';
  constexpr inline std::size_t breacket_len  = sizeof(section_begin);
  constexpr inline std::size_t separator_len = sizeof(separator);

  namespace fs = std::filesystem;

  struct section {
    std::string name;

    std::string& operator[](const std::string&);

    std::unordered_map<std::string, std::string> data;
  };

  namespace detail {
    inline auto fublya(section&) {}

    template<typename Head, typename... Args>
    inline auto fublya(section& temp, Head head, Args... args) {
      temp.data.insert(head);

      fublya(temp, args...);
    }
  }  // namespace detail

  struct ini {
    ini(const fs::path&);

    ini() {}

    ini(ini&&) noexcept;

    std::vector<section> data {};

    section& operator[](std::string);

    std::ostream& dump(std::ostream&);

    template<typename... Args>
    void insert(std::string section_name, Args... args) {
      section temp { section_name };
      detail::fublya(temp, args...);
      data.push_back(std::move(temp));
    }
  };

  ini parse(std::string_view);
  ini parse_from_file(const fs::path&);
}  // namespace ini
