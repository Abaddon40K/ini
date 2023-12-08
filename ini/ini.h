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

			if (true)
			{

			}
			else {
				throw std::out_of_range{ "Requested section '"s + name + "' not found"s };
			}


		}
		std::unordered_map<std::string, std::string> data;
	};

	struct ini {

		ini(fs::path);

		ini(){}

		ini(ini&&) noexcept;

		std::vector<section> data{};

		section& operator [](std::string);

		std::ostream& dump(std::ostream&);

	};

	ini parse_from_file(fs::path);
}


