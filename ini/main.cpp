#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include <ini.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]){
	fs::path path = "E:";
	path /= "zfile_i.ini";
	
	std::string line;

	try {
        ini::ini iii;
        ini::ini i = ini::parse_from_file(path);
        /* ini::section& ref = i["section"];
        std::cout << "'" << ref.data.cbegin()->first << "'" << std::endl;
        std::cout << "'" << ref.data.cbegin()->second << "'" << std::endl;
        std::cout << ref["domainn"] << std::endl;*/
        i.dump(std::cout);
        std::cout << i["section"]["domain"] << std::endl;
        i["section"]["domain"] = "xxx";
        std::cout << i["section"]["domain"] << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}