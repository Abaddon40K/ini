#include <filesystem>
#include <functional>
#include <ini.h>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

const std::string& get_raw_ini() {
  const static std::string data { "[section] \n"
                                  "            domain = example.com #dfhs\n"
                                  "#ksjoi\n"
                                  " [section.subsection] \n"
                                  "foo = bar\n"
                                  "\n"
                                  "      [second section] \n"
                                  "first = second\n"
                                  "foo = bar\n" };
  return data;
}

bool ini_parse_from_string() {
  bool all_is_OK  = false;
  auto ini        = ini::parse(get_raw_ini());
  all_is_OK      += (ini["section"]["domain"] != "example.com");
  all_is_OK      += (ini["section.subsection"]["foo"] != "bar");
  all_is_OK      += (ini["second section"]["first"] != "second");
  all_is_OK      += (ini["second section"]["foo"] != "bar");
  return all_is_OK == false;
}

bool ini_parse_from_files() {
  bool all_is_OK = false;
  std::cout << TEST_DATA_DIR << std::endl;
  for(auto& i: fs::directory_iterator { fs::path { TEST_DATA_DIR } }) {
    std::wcout << i.path().c_str() << std::endl;
    // all_is_OK += (ini.data.size() != 0);
  }

  return all_is_OK == false;
}

std::vector<std::function<bool()>> tests {
  ini_parse_from_string,
  ini_parse_from_files,
};

int main(int argc, char* argv[]) {
  std::size_t counter = 0;
  for(auto& i: tests) {
    ++counter;
    if(i()) std::cout << "Test #" << counter << " is completed" << std::endl;
    else std::cout << "Test #" << counter << " is failed" << std::endl;
  }


  return 0;
}
