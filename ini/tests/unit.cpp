#include <ini.h>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

std::string& get_raw_ini() { 
	static std::string data{
		"[section] \n"
		"            domain = example.com #dfhs\n"
		"#ksjoi\n"
		" [section.subsection] \n"
		"foo = bar\n"
		"\n"
		"      [second section] \n"
		"first = second\n"
		"foo = bar\n"
	};
    return data;
    }

int main(int argc, char* argv[]) { 
	auto cur = fs::current_path();
    std::cout << fs::canonical(cur) << std::endl;

	return 0; 
}