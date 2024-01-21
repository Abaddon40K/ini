#include "ini.h"

#include <iostream>
#include <string>

#include "ini.h"

namespace ini {

  std::ostream& ini::dump(std::ostream& os) {
    for(const auto& i: data) {
      os << section_begin << i.name << section_end << '\n';
      for(const auto& [key, value]: i)
        os << key << separator << value << '\n';
    }
    return os;
  }

  ini::section& ini::operator[](std::string name) {
    using namespace std::string_literals;

    auto result = std::find_if(data.begin(), data.end(), [&name](const section& lhs) {
      return (lhs.name == name);
    });

    if(data.end() != result) return *result;
    else throw std::out_of_range{ "Requested section '"s + name + "' not found"s };
  }

  ini& ini::operator=(const ini& rhs) {
    if(this != &rhs) data = rhs.data;

    return *this;
  }
  // Why warning?!?
  ini& ini::operator=(ini&& rhs) {
    if(this != &rhs) data = rhs.data;

    return *this;
  }

  bool ini::operator==(const ini& rhs) {
    if(data.size() == rhs.data.size()) {
      int size = data.size();
      for(std::size_t i = 0; i < size; ++i)
        if(data[i] != rhs.data[i]) return false;
      return true;
    }
    return false;
  }
  bool ini::operator==(ini&& rhs) {
    if(data.size() == rhs.data.size()) {
      int size = data.size();
      for(std::size_t i = 0; i < size; ++i)
        if(data[i] != rhs.data[i]) return false;
      return true;
    }
    return false;
  }

  ini parse(std::string_view str) {
    std::string  line;
    std::size_t  line_counter = 0;
    ini::section temp;
    bool         first_section_found = false;
    ini          result;

    // String data validation
    auto        new_line  = '\n';
    std::size_t line_head = 0;
    std::size_t line_tail = 0;

    while(line_tail != std::string_view::npos) {
      // Finding the beginning and end of the line
      if(line_counter != 0) line_head = line_tail + 1;
      line_tail = str.find(new_line, line_head);
      line      = str.substr(line_head, line_tail - line_head);

      auto              line_begin = line.begin();
      auto              line_end   = line.end();
      const std::size_t line_size  = line.size();

      line_counter++;

      // TODO: заменить for на find
      for(auto i = line_begin; i < line_end; i++) {
        std::size_t current_position = i - line_begin;
        if(std::isspace(*i)) continue;  // Scipping space
        if(*i == comment_begin) break;  // Looking for a comment
        if(*i == section_end)
          throw std::invalid_argument{ "Invalid struct section: Closing bracket found before than openning bracket. "
                                       "Error in line "
                                       + std::to_string(line_counter) + ", position "
                                       + std::to_string(current_position) };
        if(*i == section_begin) {
          if(first_section_found) {
            result.insert(std::move(temp));
            temp.erase(temp.begin(), temp.end());
          }
          first_section_found = true;
          auto pos            = line.find(section_end, current_position);
          if(pos == std::string::npos)
            throw std::invalid_argument{ "Invalid section name: Closing bracket not found. Error in line "
                                         + std::to_string(line_counter) + ", position "
                                         + std::to_string(current_position) };

          if(line.find(section_end, pos + breacket_len) != std::string::npos
             || line.find(section_begin, current_position + breacket_len) != std::string::npos) {
            throw std::invalid_argument{
              "Invalid section name: More than one opening or closing parenthesis on a line. Error in line "
              + std::to_string(line_counter) + ". Line: " + line
            };
          }

          temp.name = std::string{
            line.substr(current_position + breacket_len, pos - current_position - breacket_len)
          };  // Saving the section name without brackets

          auto symbol_after_breaket_it = std::find_if(line_begin + pos + breacket_len, line_end, [](char symbol) {
            return symbol == comment_begin || !std::isspace(symbol);
          });
          if(!(symbol_after_breaket_it == line_end || *symbol_after_breaket_it == comment_begin))
            throw std::invalid_argument{
              "Invalid section name: Symbols after closing bracket. Error in line " + std::to_string(line_counter)
              + ", position " + std::to_string(symbol_after_breaket_it - line_begin) + ". Founded symbols on tail: "
              + line.substr(symbol_after_breaket_it - line_begin, line_size - (symbol_after_breaket_it - line_begin))
            };

          break;
        }

        const std::size_t sep_pos = line.find(separator);
        if(sep_pos == std::string::npos)
          throw std::invalid_argument{ "Invalid line: Value without seporator. Error in line "
                                       + std::to_string(line_counter) + ". Invalide line: '" + line + "'" };
        if(*i == separator) {
          for(auto j = i + separator_len; j < line_end; j++) {
            if(*j == comment_begin)
              throw std::invalid_argument{ "Invalid line: Only seporator on line. Error in line "
                                           + std::to_string(line_counter) + ". Invalide line: '" + line + "'" };
            if(!std::isspace(*j))
              throw std::invalid_argument{ "Invalid line: Value without key. Error in line "
                                           + std::to_string(line_counter) + ". Invalide line: '" + line + "'" };
          }
          throw std::invalid_argument{ "Invalid line: Only seporator on line. Error in line "
                                       + std::to_string(line_counter) + ". Invalide line: '" + line + "'" };
        }

        auto r_sep_pos   = std::reverse_iterator{ line_begin + sep_pos };
        auto key_end     = std::find_if(r_sep_pos + separator_len, line.rend(), [](char symbol) {
                         return !std::isspace(symbol);
                       }).base();
        auto value_begin = std::find_if(line_begin + sep_pos + separator_len, line.end(), [](char symbol) {
          return !std::isspace(symbol);
        });

        auto value_end = std::find_if(value_begin, line.end(), [](char symbol) {
          return symbol == comment_begin;
        });
        if(value_end == line_end)
          value_end = std::find_if(line.rbegin(), std::reverse_iterator{ value_begin }, [](char symbol) {
                        return !std::isspace(symbol);
                      }).base();
        else
          value_end =
              std::find_if(std::reverse_iterator{ value_end }, std::reverse_iterator{ value_begin }, [](char symbol) {
                return !std::isspace(symbol);
              }).base();

        std::string key{ i, key_end };

        if(temp.find(key) != temp.end())
          throw std::invalid_argument{ "In section " + temp.name + " already exist key '" + key
                                       + "'. Repetitive key in line " + std::to_string(line_counter) };

        temp.insert({
            std::move(key), std::string{value_begin, value_end}
        });
        break;
      }
    }

    if(first_section_found) result.insert(std::move(temp));


    return result;
  }

  ini parse_from_file(const fs::path& path) {
    if(!fs::exists(path)) throw fs::filesystem_error{ "File not found", path, std::error_code{} };
    std::ifstream file(path);
    std::string   data{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
    return parse(data);
  }

  ini::ini(const fs::path& path) { data = std::move(parse_from_file(path).data); }

  ini::section::section(const ini::section& rhs) {
    name = rhs.name;
    data = rhs.data;
  }
  ini::section::section(section&& rhs) noexcept {
    name = rhs.name;
    data = rhs.data;
  }

  std::string& ini::section::operator[](const std::string& key) {
    using namespace std::string_literals;
    auto it = data.find(key);
    if(it != data.end()) return it->second;
    else throw std::out_of_range{ "Requested key '"s + key + "' not found in section '"s + name + "'"s };
  }

  ini::section& ini::section::operator=(const section& rhs) {
    if(this != &rhs) {
      data = rhs.data;
      name = rhs.name;
    }
    return *this;
  }
  //  ini::section& ini::section::operator=(section&& rhs) {
  //    if(this != &rhs) {
  //    data = rhs.data;
  //    name = rhs.name;
  //    }
  //    return *this;
  //  }

  bool ini::section::operator==(const section& rhs) { return ((name == rhs.name) && (data == rhs.data)); }
  bool ini::section::operator==(section&& rhs) { return ((name == rhs.name) && (data == rhs.data)); }

}  // namespace ini
