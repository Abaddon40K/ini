/*
 * TODO: fix build on windows
 */

#include <gtest/gtest.h>
#include <ini/ini.h>

#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

const std::string& get_raw_ini() {
  const static std::string data{ "[section] \n"
                                 "            domain = example.com #dfhs\n"
                                 "#ksjoi\n"
                                 " [section.subsection] \n"
                                 "foo = bar\n"
                                 "\n"
                                 "      [second section] \n"
                                 "first = second\n"
                                 "foo = bar\n"
                                 "[section] \n"
                                 "foo = bar\n"


  };
  return data;
}

TEST(section, emplace) {
  ini::ini::section s(std::string{ "name" });
  s.emplace(std::pair{ std::string{ "first" }, std::string{ "second" } });
  EXPECT_STREQ(s["first"].c_str(), "second");
//  s.emplace(std::pair{ std::string{ "first_1" }, std::string{ "second_1" } },
//            std::pair{ std::string{ "first_2" }, std::string{ "second_2" } },
//            std::pair{ std::string{ "first_3" }, std::string{ "second_3" } },
//            std::pair{ std::string{ "first_4" }, std::string{ "second_4" } });
//  EXPECT_STREQ(s["first_1"].c_str(), "second_1");
//  EXPECT_STREQ(s["first_2"].c_str(), "second_2");
//  EXPECT_STREQ(s["first_3"].c_str(), "second_3");
//  EXPECT_STREQ(s["first_4"].c_str(), "second_4");
}

//TEST(section, clear) {
//  ini::ini::section s(std::string{ "name" });
//  s.emplace(std::pair{ std::string{ "first" }, std::string{ "second" } },
//            std::pair{ std::string{ "first_1" }, std::string{ "second_1" } },
//            std::pair{ std::string{ "first_2" }, std::string{ "second_2" } },
//            std::pair{ std::string{ "first_3" }, std::string{ "second_3" } },
//            std::pair{ std::string{ "first_4" }, std::string{ "second_4" } });
//  s.clear();
//  EXPECT_EQ(s.empty(), 1);
//}

//TEST(section, erase) {
//  ini::ini::section s(std::string{ "name" });
//  s.emplace(std::pair{ std::string{ "first" }, std::string{ "second" } },
//            std::pair{ std::string{ "first_1" }, std::string{ "second_1" } });
//  s.erase("first");
//}

TEST(section, empty) {
  ini::ini::section s(std::string{ "name" });
  EXPECT_EQ(s.empty(), 1);
  s.emplace(std::pair{ std::string{ "first" }, std::string{ "second" } });
  EXPECT_EQ(s.empty(), 0);
}



TEST(ini, parse_from_string) {
  ini::ini ini = ini::parse(get_raw_ini());

  EXPECT_STREQ(ini["section"]["domain"].c_str(), "example.com");
  EXPECT_STREQ(ini["section.subsection"]["foo"].c_str(), "bar");
  EXPECT_STREQ(ini["second section"]["first"].c_str(), "second");
  EXPECT_STREQ(ini["second section"]["foo"].c_str(), "bar");
}

TEST(ini, parse_from_files) {
  for(auto& i: fs::directory_iterator{ fs::path{ TEST_DATA_DIR } })
    ASSERT_GT(ini::parse_from_file(i).size(), 0);
}

TEST(ini, range_based_for) {
  ini::ini ini = ini::parse(get_raw_ini());
  using vt     = ini::ini::section::value_type;
  for(ini::ini::section& i: ini) {
    EXPECT_FALSE(i.empty());
    EXPECT_FALSE(i.name.empty());
    for(vt& j: i) {
      EXPECT_FALSE(j.first.empty());
      EXPECT_FALSE(j.second.empty());
    }
  }
}

TEST(ini, size) {
  ini::ini ini = ini::parse(get_raw_ini());
  EXPECT_EQ(ini.size(), 4);
}

TEST(ini, clear) {
  ini::ini ini = ini::parse(get_raw_ini());
  ini.clear();
  EXPECT_EQ(ini.size(), 0);
}

TEST(ini, empty) {
  ini::ini ini = ini::parse(get_raw_ini());
  EXPECT_FALSE(ini.empty());
  ini.clear();
  EXPECT_TRUE(ini.empty());
}

TEST(ini, erase) {
  ini::ini ini = ini::parse(get_raw_ini());
  auto     b   = ini.begin();
  ini.erase(b);
  try {
    ini["section"];
  } catch(const std::out_of_range&) {}
  b      = ini.begin();
  auto e = ini.end() - 1;
  ini.erase(b + 1, e);
  try {
    ini["section.subsection"];
  } catch(const std::out_of_range&) {}
  try {
    ini["second section"];
  } catch(const std::out_of_range&) {}
}

TEST(ini, swap) {
  ini::ini ini_1             = ini::parse(get_raw_ini());
  ini::ini ini_2             = ini_1;
  ini_2["section"]["domain"] = "new value";
  ini_1.swap(ini_2);
  EXPECT_STREQ(ini_1["section"]["domain"].c_str(), "new value");
  EXPECT_STREQ(ini_2["section"]["domain"].c_str(), "example.com");
}

TEST(ini, insert) {
  ini::ini          ini = ini::parse(get_raw_ini());
  ini::ini::section s(std::string{ "Hello" });
  ini.insert(s);
  EXPECT_STREQ(ini["Hello"].name.c_str(), "Hello");
  // ini::ini::section s2(std::string{ "Hello" }, { std::string{ "first" }, std::string{ "second" } });
  s.name = std::string{ "Hello 2" };
  s.emplace(std::pair{ std::string{ "first" }, std::string{ "second" } });
  ini.insert(s);
  EXPECT_STREQ(ini["Hello 2"].name.c_str(), "Hello 2");
  EXPECT_STREQ(ini["Hello 2"]["first"].c_str(), "second");
  ini.insert(std::string{ "Hello 3" });
  EXPECT_STREQ(ini["Hello 3"].name.c_str(), "Hello 3");
}
