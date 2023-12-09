#include "ini.h"
#include <iostream>

namespace ini {
    ini::ini(ini&& rhs) noexcept : data{rhs.data} {}

    ini::ini(const fs::path& path) : ini(parse_from_file(path)) {}



    std::ostream& ini::dump(std::ostream& os) {
        for (const auto& i : data) {
            os << section_begin << i.name << section_end << '\n';
            for (const auto& [key, value] : i.data) {
                os << key << separator << value << '\n';
            }
        }
        return os;
    }

    section& ini::operator[](std::string name) {
        using namespace std::string_literals;

        auto result =
            std::find_if(data.begin(), data.end(), [&name](const section& lhs) { return (lhs.name == name); });
        if (data.end() != result) {
            return *result;
        } else {
            throw std::out_of_range{"Requested section '"s + name + "' not found"s};
        }
    }
    ini parse(std::string_view str) {
        std::string line;
        std::size_t line_counter = 0;
        section temp;

        bool first_section_found = false;
        
        ini result;



        // String data validation
        auto new_line = '\n';
        std::size_t head = 0;
        std::size_t tail = 0;
        
        while (tail != std::string_view::npos) {
            if (line_counter != 0) head = tail + 1;
            tail = str.find(new_line, head);
            
            line = str.substr(head, tail - head);
            auto line_begin = line.begin();
            auto line_end = line.end();

            const std::size_t line_size = line.size();
            line_counter++;

            // TODO ПЕРЕПИСАТЬ НА ФАЙНД
            for (auto i = line_begin; i < line_end; i++) {
                std::size_t current_position = 0;  // выпилить, заменить на i - line_begin
                if (std::isspace(*i)) continue;
                if (*i == comment_begin) break;  // Looking for a comment
                if (*i == section_end) {
                    throw std::invalid_argument{
                        "Invalid struct section: Closing bracket found before than openning bracket. Error "
                        "in line " +
                        std::to_string(line_counter) + ", position " + std::to_string(current_position)};
                }
                if (*i == section_begin) {
                    if (first_section_found) {
                        result.data.push_back(std::move(temp));
                    }

                    first_section_found = true;
                    auto pos = line.find(section_end, current_position);
                    if (pos == std::string::npos) {
                        throw std::invalid_argument{"Invalid section name: Closing bracket not found. Error in line " +
                                                    std::to_string(line_counter) + ", position " +
                                                    std::to_string(current_position)};
                    }
                    if (line.find(section_end, pos + breacket_len) != std::string::npos ||
                        line.find(section_begin, current_position + breacket_len) != std::string::npos) {
                        throw std::invalid_argument{
                            "Invalid section name: More than one opening or closing parenthesis on a line. "
                            "Error in line " +
                            std::to_string(line_counter) + ". Line: " + line};
                    }
                    temp.name = std::string{line.substr(
                        current_position + breacket_len,
                        pos - current_position - breacket_len)};  // Saving the section name without brackets

                    auto symbol_after_breaket_it =
                        std::find_if(line_begin + pos + breacket_len, line_end,
                                     [](char symbol) { return symbol == comment_begin || !std::isspace(symbol); });
                    if (!(symbol_after_breaket_it == line_end || *symbol_after_breaket_it == comment_begin)) {
                        throw std::invalid_argument{
                            "Invalid section name: Symbols after closing bracket. Error in line " +
                            std::to_string(line_counter) + ", position " +
                            std::to_string(symbol_after_breaket_it - line_begin) + ". Founded symbols on tail: " +
                            line.substr(symbol_after_breaket_it - line_begin,
                                        line_size - (symbol_after_breaket_it - line_begin))};
                    };

                    break;
                }

                const std::size_t sep_pos = line.find(separator);
                if (sep_pos == std::string::npos) {
                    throw std::invalid_argument{"Invalid line: Value without seporator. Error in line " +
                                                std::to_string(line_counter) + ". Invalide line: '" + line + "'"};
                }
                if (*i == separator) {
                    for (auto j = i + separator_len; j < line_end; j++) {
                        if (*j == comment_begin) {
                            throw std::invalid_argument{"Invalid line: Only seporator on line. Error in line " +
                                                        std::to_string(line_counter) + ". Invalide line: '" + line +
                                                        "'"};
                        }
                        if (!std::isspace(*j)) {
                            throw std::invalid_argument{"Invalid line: Value without key. Error in line " +
                                                        std::to_string(line_counter) + ". Invalide line: '" + line +
                                                        "'"};
                        }
                        throw std::invalid_argument{"Invalid line: Only seporator on line. Error in line " +
                                                    std::to_string(line_counter) + ". Invalide line: '" + line + "'"};
                    }
                }
                ////////// ТУТ НЕ ПОНЯЛ КАК
                // TODO: заменить циклы на find_if и итераторы
                auto r_sep_pos = std::reverse_iterator{line_begin + sep_pos};
                auto key_end = std::find_if(r_sep_pos + separator_len, line.rend(), [](char symbol) {
                                   return !std::isspace(symbol);
                               }).base();
                auto value_begin = std::find_if(line_begin + sep_pos + separator_len, line.end(),
                                                [](char symbol) { return !std::isspace(symbol); });

                auto value_end =
                    std::find_if(value_begin, line.end(), [](char symbol) { return symbol == comment_begin; });
                if (value_end == line_end) {
                    value_end = std::find_if(line.rbegin(), std::reverse_iterator{value_begin}, [](char symbol) {
                                    return !std::isspace(symbol);
                                }).base();
                } else {
                    value_end = std::find_if(std::reverse_iterator{value_end}, std::reverse_iterator{value_begin},
                                             [](char symbol) { return !std::isspace(symbol); })
                                    .base();
                }

                temp.data.insert({std::string{i, key_end}, std::string{value_begin, value_end}});
                break;
            }
        }

        if (first_section_found) {
            result.data.push_back(std::move(temp));
        }

        return result;
    }

    ini parse_from_file(const fs::path& path) {
        if (!fs::exists(path)) {
            throw fs::filesystem_error{"File not found", path, std::error_code{}};
        }
        std::ifstream file(path);
        std::string data{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        return parse(data);
    }

    

}  // namespace ini
