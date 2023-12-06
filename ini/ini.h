#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>

namespace ini {


	struct section {
		std::string name;

		struct kv {
			std::string key;
			std::string value;
		};

		std::vector<kv> data;
	};

	struct ini {

		ini(std::ifstream& file) {
			std::string line;
			std::size_t line_counter = 0;
			section temp;
			section::kv temp_kv;
			bool first_section_found = false;
			

			constexpr char comment_begin = '#';
			constexpr char section_begin = '[';
			constexpr char section_end = ']';
			constexpr char separator = '=';
			constexpr std::size_t breacket_len = sizeof(section_begin);
			constexpr std::size_t separator_len = sizeof(separator);

			// String data validation
			while (std::getline(file, line)) {
				const std::size_t line_size = line.size();
				line_counter++;
				for (std::size_t i = 0; i < line_size; i++) {
					if (std::isspace(line[i])) continue;
					if (line[i] == comment_begin) break; // Looking for a comment
					if (line[i] == section_end) {
						throw std::invalid_argument{ "Invalid struct section: Closing bracket found before than openning bracket. Error in line " + std::to_string(line_counter) + ", position " + std::to_string(i) };
					}

					if (line[i] == section_begin) {
						if (first_section_found) {
							data.push_back(std::move(temp));
						}

						first_section_found = true;
						auto pos = line.find(']', i);
						if (pos == std::string::npos) {
							throw std::invalid_argument{ "Invalid section name: Closing bracket not found. Error in line " + std::to_string(line_counter) + ", position " + std::to_string(i) };
						}
						temp.name = line.substr(i + breacket_len, pos - i - breacket_len); // Saving the section name without brackets
						for (std::size_t j = pos + 1; j < line_size; j++) {
							if (line[j] == '#') break; // Looking for a comment after section name
							if (!std::isspace(line[j])) {
								throw std::invalid_argument{ "Invalid section name: Symbols after closing bracket. Error in line " + std::to_string(line_counter) + ", position " + std::to_string(j) + ". Founded symbols on tail: " + line.substr(j, line_size - j) };
							}
						}

						break;
					}

					const std::size_t sep_pos = line.find('=');
					if (sep_pos == std::string::npos) {
						throw std::invalid_argument{ "Invalid line: Value without seporator. Error in line " + std::to_string(line_counter) + ". Invalide line: '" + line + "'" };
					}
					if (line[i] == '=')
					{
						for (std::size_t j = i; j < line_size; j++) {
							if (line[j] == '#') {
								throw std::invalid_argument{ "Invalid line: Only seporator on line. Error in line " + std::to_string(line_counter) + ". Invalide line: '" + line + "'" };
							}
							if (!std::isspace(line[j])) {
								throw std::invalid_argument{ "Invalid line: Value without key. Error in line " + std::to_string(line_counter) + ". Invalide line: '" + line + "'" };
							}
							throw std::invalid_argument{ "Invalid line: Only seporator on line. Error in line " + std::to_string(line_counter) + ". Invalide line: '" + line + "'" };
						}
					}

					// TODO: заменить циклы на find_if и итераторы
					std::size_t key_begin = i, key_end = 0, value_begin = 0, value_end = 0;
					for (std::size_t j = sep_pos - separator_len; j > 0; j--) {
						if (!std::isspace(line[j])) {
							key_end = j;
							break;
						}
					}
					for (std::size_t j = sep_pos + separator_len; j < line_size; j++) {
						if (!std::isspace(line[j])) {
							if (value_begin == 0)
							{
								value_begin = j;
							}
							if (line[j] == '#') break;
							value_end = j; //WHY???
						}
					}

					// TODO: Сделать на итераторах
					temp_kv.key = line.substr(key_begin, key_end - key_begin);
					temp_kv.value = line.substr(value_begin, value_end - value_begin);


					temp.data.push_back(temp_kv);
					break;
				}

			}

			if (first_section_found) {
				data.push_back(std::move(temp));
			}
		}

		std::vector<section> data;

		section& operator [](std::string name) {
			using namespace std::string_literals;

			auto result = std::find_if(data.begin(), data.end(), [&name](const section& lhs) {
				return (lhs.name == name);
				});
			if (data.end() != result) {
				return *result;
			}
			else {
				throw std::out_of_range{ "Requested section '"s + name + "' not found"s };
			}


		}
	};
}