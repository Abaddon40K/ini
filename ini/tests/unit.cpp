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
#include <cstring>


namespace fs = std::filesystem;


const std::string raw_test_data{ "[section] \n"
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


TEST(section, emplace) {
  ini::ini::section s(std::string{ "name" });
  s.emplace(std::string{ "first" }, std::string{ "second" });
  EXPECT_STREQ(s["first"].c_str(), "second");

  s.emplace(std::string{ "first_1" }, std::string{ "second_1" });
  s.emplace(std::string{ "first_2" }, std::string{ "second_2" });
  s.emplace(std::string{ "first_3" }, std::string{ "second_3" });
  s.emplace(std::string{ "first_4" }, std::string{ "second_4" });

  EXPECT_STREQ(s["first_1"].c_str(), "second_1");
  EXPECT_STREQ(s["first_2"].c_str(), "second_2");
  EXPECT_STREQ(s["first_3"].c_str(), "second_3");
  EXPECT_STREQ(s["first_4"].c_str(), "second_4");
}

TEST(section, clear) {
  ini::ini::section s(std::string{ "name" });
  s.emplace(std::string{ "first" }, std::string{ "second" });
  s.emplace(std::string{ "first_1" }, std::string{ "second_1" });
  s.emplace(std::string{ "first_2" }, std::string{ "second_2" });
  s.emplace(std::string{ "first_3" }, std::string{ "second_3" });
  s.emplace(std::string{ "first_4" }, std::string{ "second_4" });
  EXPECT_EQ(s.size(), 5);
  s.clear();
  EXPECT_EQ(s.empty(), 1);
}

TEST(section, find) {
  ini::ini::section s(std::string{ "name" });
  s.emplace(std::string{ "first" }, std::string{ "second" });
  EXPECT_EQ(s.find("first"), s.begin());
  EXPECT_EQ(s.find("first_1"), s.end());
}

TEST(section, erase) {
  std::string name      = "name";
  std::string first_key = "first", second_key = "first_1";
  std::string first_value = "second", second_value = "second_1";

  ini::ini::section s(name);
  s.emplace(first_key, first_value);
  s.emplace(second_key, second_value);
  s.erase(first_key);
  EXPECT_EQ(s.find(first_key), s.end());
  EXPECT_NE(s.find(second_key), s.end());
  s.clear();

  s.emplace(first_key, first_value);
  s.emplace(second_key, second_value);
  auto it = s.find(first_key);
  s.erase(it);
  EXPECT_EQ(s.find(first_key), s.end());
  EXPECT_NE(s.find(second_key), s.end());
  s.clear();

  s.emplace(first_key, first_value);
  s.emplace(second_key, second_value);
  auto cit = s.find(first_key);
  s.erase(cit);
  EXPECT_EQ(s.find(first_key), s.end());
  EXPECT_NE(s.find(second_key), s.end());
  s.clear();
}

TEST(section, empty) {
  ini::ini::section s(std::string{ "name" });
  EXPECT_EQ(s.empty(), 1);
  s.emplace(std::string{ "first" }, std::string{ "second" });
  EXPECT_EQ(s.empty(), 0);
}



TEST(ini, parse_from_string) {
  ini::ini ini = ini::parse(raw_test_data);

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
  ini::ini ini = ini::parse(raw_test_data);
  for(const auto& [name, sect]: ini) {
    EXPECT_FALSE(sect.empty());
    EXPECT_FALSE(sect.name.empty());
    for(const auto& [key, value]: sect) {
      EXPECT_FALSE(key.empty());
      EXPECT_FALSE(value.empty());
    }
  }
}

TEST(ini, size) {
  ini::ini ini = ini::parse(raw_test_data);
  EXPECT_EQ(ini.size(), 3);
}

TEST(ini, clear) {
  ini::ini ini = ini::parse(raw_test_data);
  ini.clear();
  EXPECT_EQ(ini.size(), 0);
}

TEST(ini, empty) {
  ini::ini ini = ini::parse(raw_test_data);
  EXPECT_FALSE(ini.empty());
  ini.clear();
  EXPECT_TRUE(ini.empty());
}

TEST(ini, erase) {
  ini::ini ini = ini::parse(raw_test_data);

  auto        first_element = ini.begin();
  std::string name_first    = first_element->first;

  ini.erase(first_element);
  EXPECT_EQ(ini.find(name_first), ini.end());

  try {
    ini[name_first];
  } catch(const std::out_of_range& e) {
    ASSERT_TRUE(std::strstr(e.what(), "not found"));
    ASSERT_TRUE(std::strstr(e.what(), name_first.c_str()));
  }
}

TEST(ini, swap) {
  ini::ini ini_1             = ini::parse(raw_test_data);
  ini::ini ini_2             = ini_1;
  ini_2["section"]["domain"] = "new value";
  ini_1.swap(ini_2);
  EXPECT_STREQ(ini_1["section"]["domain"].c_str(), "new value");
  EXPECT_STREQ(ini_2["section"]["domain"].c_str(), "example.com");
}

TEST(ini, insert) {
  ini::ini          ini = ini::parse(raw_test_data);
  ini::ini::section s(std::string{ "Hello" });
  ini.insert(s);
  EXPECT_STREQ(ini["Hello"].name.c_str(), "Hello");
  // ini::ini::section s2(std::string{ "Hello" }, { std::string{ "first" }, std::string{ "second" } });
  s.name = std::string{ "Hello 2" };
  s.emplace(std::string{ "first" }, std::string{ "second" });
  ini.insert(s);
  EXPECT_STREQ(ini["Hello 2"].name.c_str(), "Hello 2");
  EXPECT_STREQ(ini["Hello 2"]["first"].c_str(), "second");
  ini.insert(std::string{ "Hello 3" });
  EXPECT_STREQ(ini["Hello 3"].name.c_str(), "Hello 3");
}
