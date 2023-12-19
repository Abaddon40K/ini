/*
 * TODO: fix build on windows
 */

#include <filesystem>
#include <functional>
#include <gtest/gtest.h>
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

TEST(ini, parse_from_string) {
  auto ini = ini::parse(get_raw_ini());

  EXPECT_STREQ(ini["section"]["domain"].c_str(), "example.com");
  EXPECT_STREQ(ini["section.subsection"]["foo"].c_str(), "bar");
  EXPECT_STREQ(ini["second section"]["first"].c_str(), "second");
  EXPECT_STREQ(ini["second section"]["foo"].c_str(), "bar");
}

TEST(ini, parse_from_files) {
  for(auto& i: fs::directory_iterator { fs::path { TEST_DATA_DIR } })
    ASSERT_GT(ini::parse_from_file(i).data.size(), 0);
}
