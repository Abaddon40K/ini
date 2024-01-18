#include <filesystem>
#include <fstream>
#include <ini/ini.h>
#include <iostream>
#include <string>
#include <thread>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  fs::path path  = "E:";
  path          /= "zfile_i.ini";

  std::string line;

  try {
    ini::ini iii;
    ini::ini i = ini::parse_from_file(path);
    // i.dump(std::cout);
    // std::cout << i["section"]["domain"] << std::endl;
    // i["section"]["domain"] = "xxx";
    // std::cout << i["section"]["domain"] << std::endl;
    // i.dump(std::cout);

    i.insert("MY NAME", std::pair { "Key", " Value" }, std::pair { "Second Key", "Second Value" });

    i.dump(std::cout);
  } catch(const std::exception& e) { std::cout << e.what() << std::endl; }

  return 0;
}
