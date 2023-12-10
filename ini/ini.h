#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <unordered_map>

namespace ini {
	constexpr inline char comment_begin = '#';
	constexpr inline char section_begin = '[';
	constexpr inline char section_end = ']';
	constexpr inline char separator = '=';
	constexpr inline std::size_t breacket_len = sizeof(section_begin);
	constexpr inline std::size_t separator_len = sizeof(separator);

	namespace fs = std::filesystem;

	struct section {
		std::string name;

		std::string& operator [](const std::string& key){
			using namespace std::string_literals;
            auto it = data.find(key);
			if (it != data.end()){
                return it->second;
			}
			else {
                throw std::out_of_range{"Requested key '"s + key + "' not found in section '"s + name + "'"s};
			}
		}
		std::unordered_map<std::string, std::string> data;
	};

	struct ini {

		ini(const fs::path&);

		ini(){}

		ini(ini&&) noexcept;

		std::vector<section> data {};

		section& operator [](std::string);

		std::ostream& dump(std::ostream&);

	};
    ini parse(std::string_view);
	ini parse_from_file(const fs::path&);
}


