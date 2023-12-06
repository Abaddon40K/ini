#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include <ini.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]){
	fs::path path = "E:";
	path /= "zfile_i.ini";
	if (!fs::exists(path)){
		std::cerr << "Error: invalid file path" << std::endl;
		return -1;
	}
	std::ifstream file(path);
	std::string line;


	

	try
	{
		ini::ini i(file);
		ini::section& ref = i["second section"];
		std::cout << ref.data[1].key << std::endl;
		std::cout << ref.data[1].value << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	while (std::getline(file, line)) {
			std::cout << line << std::endl;
	}

	return 0;
}