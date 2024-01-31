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
#include <typeinfo>


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

const std::string sect_name_1 = "Section name_1";
const std::string sect_name_2 = "Section name_2";

const std::string                         key_1   = "Key_1";
std::string                               value_1 = "Value_1";
std::pair<const std::string, std::string> p_1{ key_1, value_1 };

const std::string                         key_2   = "Key_2";
std::string                               value_2 = "Value_2";
std::pair<const std::string, std::string> p_2{ key_2, value_2 };

TEST(section, constructors) {
  ini::ini::section s_1;
  EXPECT_STREQ(s_1.name.c_str(), std::string{}.c_str());
  ASSERT_TRUE(s_1.empty());

  ini::ini::section s_2(sect_name_1);
  EXPECT_STREQ(s_2.name.c_str(), sect_name_1.c_str());
  ASSERT_TRUE(s_2.empty());

  ini::ini::section s_3(sect_name_1, p_1);
  EXPECT_STREQ(s_3.name.c_str(), sect_name_1.c_str());
  ASSERT_FALSE(s_3.empty());
  EXPECT_STREQ(s_3[key_1].c_str(), value_1.c_str());

  ini::ini::section s_4(sect_name_1, p_1, p_2);
  EXPECT_STREQ(s_4.name.c_str(), sect_name_1.c_str());
  ASSERT_FALSE(s_4.empty());
  EXPECT_STREQ(s_4[key_1].c_str(), value_1.c_str());
  EXPECT_STREQ(s_4[key_2].c_str(), value_2.c_str());

  ini::ini::section s_5 = s_3;
  EXPECT_STREQ(s_5.name.c_str(), sect_name_1.c_str());
  EXPECT_STREQ(s_5[key_1].c_str(), value_1.c_str());

  ini::ini::section s_6 = { sect_name_1, p_1 };
  EXPECT_STREQ(s_6.name.c_str(), sect_name_1.c_str());
  EXPECT_STREQ(s_6[key_1].c_str(), value_1.c_str());
}

TEST(section, insert) {
  ini::ini::section s_1(sect_name_1, p_1);
  s_1.insert(p_2);
  EXPECT_STREQ(s_1[key_2].c_str(), value_2.c_str());

  std::string                   new_value = "New value";
  ini::ini::section::value_type p_n       = { p_1.first, new_value };
  ASSERT_FALSE(s_1.insert(p_n).second);
}

TEST(section, insert_or_assign) {
  ini::ini::section s_1(sect_name_1, p_1);
  s_1.insert_or_assign(p_2.first, p_2.second.c_str());
  EXPECT_STREQ(s_1[key_2].c_str(), value_2.c_str());

  std::string                   new_value = "New value";
  ini::ini::section::value_type p_n       = { p_1.first, new_value };
  ASSERT_FALSE(s_1.insert_or_assign(p_n.first, p_n.second.c_str()).second);
}

TEST(section, emplace) {
  ini::ini::section s(sect_name_1);
  s.emplace(key_1, std::string{ value_1 });
  EXPECT_STREQ(s[key_1].c_str(), value_1.c_str());

  s.clear();

  s.emplace(std::string{ key_1 }, std::string{ value_1 });
  EXPECT_STREQ(s[key_1].c_str(), value_1.c_str());
}

TEST(section, try_emplace) {
  ini::ini::section s(sect_name_1);
  s.try_emplace(key_1, std::string{ value_1 });
  EXPECT_STREQ(s[key_1].c_str(), value_1.c_str());

  s.try_emplace(key_1, std::string{ value_2 });
  EXPECT_STREQ(s[key_1].c_str(), value_1.c_str());
}

TEST(section, square_brackets_operator) {
  ini::ini::section s(sect_name_1, p_1);
  EXPECT_STREQ(s[key_1].c_str(), value_1.c_str());
  bool in_catch = false;
  try {
    s[key_2];
  } catch(const std::out_of_range& e) {
    ASSERT_TRUE(std::strstr(e.what(), "not found in section"));
    ASSERT_TRUE(std::strstr(e.what(), key_2.c_str()));
    in_catch = true;
  }
  ASSERT_TRUE(in_catch);
}

TEST(section, find) {
  ini::ini::section s(sect_name_1, p_1);
  EXPECT_EQ(s.find(key_1)->second, value_1);
  EXPECT_EQ(s.find(key_2), s.end());
}

TEST(section, unsafe_access) {
  ini::ini::section s(sect_name_1, p_1);
  EXPECT_STREQ(s.unsafe_access(key_1).c_str(), value_1.c_str());
}

TEST(section, swap) {
  ini::ini::section s_1(sect_name_1, p_1);
  ini::ini::section s_2(sect_name_2, p_2);
  s_1.swap(s_2);

  EXPECT_STREQ(s_1[key_2].c_str(), value_2.c_str());
  EXPECT_STREQ(s_1.name.c_str(), sect_name_2.c_str());
  EXPECT_STREQ(s_2[key_1].c_str(), value_1.c_str());
  EXPECT_STREQ(s_2.name.c_str(), sect_name_1.c_str());

  using ini::swap;
  using std::swap;
  swap(s_1, s_2);
  EXPECT_STREQ(s_1[key_1].c_str(), value_1.c_str());
  EXPECT_STREQ(s_1.name.c_str(), sect_name_1.c_str());
  EXPECT_STREQ(s_2[key_2].c_str(), value_2.c_str());
  EXPECT_STREQ(s_2.name.c_str(), sect_name_2.c_str());
}

TEST(section, size) {
  ini::ini::section s(sect_name_1);
  EXPECT_EQ(s.size(), 0);
  s.insert(p_1);
  EXPECT_EQ(s.size(), 1);
  s.insert(p_2);
  EXPECT_EQ(s.size(), 2);
}

TEST(section, erase) {
  ini::ini::section s(sect_name_1);
  s.emplace(key_1, std::string{ value_1 });
  s.emplace(key_2, std::string{ value_2 });
  s.erase(key_1);
  EXPECT_EQ(s.find(key_1), s.end());
  EXPECT_NE(s.find(key_2), s.end());
  s.clear();

  s.emplace(key_1, std::string{ value_1 });
  s.emplace(key_2, std::string{ value_2 });
  auto it = s.find(key_1);
  s.erase(it);
  EXPECT_EQ(s.find(key_1), s.end());
  EXPECT_NE(s.find(key_2), s.end());
  s.clear();

  s.emplace(key_1, std::string{ value_1 });
  s.emplace(key_2, std::string{ value_2 });
  auto cit   = s.cbegin();
  auto b_key = cit->first;
  s.erase(cit);
  EXPECT_EQ(s.find(b_key), s.end());
  s.clear();
}

TEST(section, extract) {
  ini::ini::section s(sect_name_1);
  s.emplace(key_1, std::string{ value_1 });
  s.emplace(key_2, std::string{ value_2 });
  auto it = s.find(key_1);
  std::string key = it->first, value = it->second;

  auto node = s.extract(it);
  EXPECT_STREQ(node.key().c_str(), key.c_str());
  EXPECT_STREQ(node.mapped().c_str(), value.c_str());
  s.clear();

  s.emplace(key_1, std::string{ value_1 });
  s.emplace(key_2, std::string{ value_2 });
  node = s.extract(key_1);
  EXPECT_STREQ(node.key().c_str(), key.c_str());
  EXPECT_STREQ(node.mapped().c_str(), value.c_str());
}

TEST(section, clear_and_empty) {
  ini::ini::section s(sect_name_1);
  EXPECT_EQ(s.size(), 0);
  ASSERT_TRUE(s.empty());
  s.insert(p_1, p_2);
  EXPECT_EQ(s.size(), 2);
  s.clear();
  EXPECT_EQ(s.size(), 0);
  ASSERT_TRUE(s.empty());
}

TEST(section, equality_and_inequality_operators) {
  ini::ini::section s_1(sect_name_1, p_1);
  ini::ini::section s_2(sect_name_1, p_1);
  ini::ini::section s_3(sect_name_2, p_2);
  ASSERT_TRUE(s_1 == s_2);
  ASSERT_FALSE(s_1 == s_3);

  ASSERT_TRUE(s_1 != s_3);
  ASSERT_FALSE(s_1 != s_2);
}

TEST(section, dump) {
    ini::ini::section s(sect_name_1, p_1);
    std::ostringstream buff;
    std::string str;
    str += key_1;
    str += ini::separator;
    str += value_1;
    str += '\n';

    s.dump(buff);
    EXPECT_STREQ(buff.str().c_str(), str.c_str());
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

// Напомните, что это за тест?
TEST(ini, range_based_for) {
  ini::ini ini = ini::parse(raw_test_data);
  for(const auto& [name, sect] : ini) {
    EXPECT_FALSE(sect.empty());
    EXPECT_FALSE(sect.name.empty());
    for(const auto& [key, value] : sect) {
      EXPECT_FALSE(key.empty());
      EXPECT_FALSE(value.empty());
    }
  }
}

TEST(ini, constructors) {
  ini::ini ini_1{};

  ini::ini ini_2(fs::path{ TEST_DATA_DIR });
  EXPECT_TRUE(ini_2 == ini::parse_from_file(fs::path{ TEST_DATA_DIR }));

  ini::ini ini_3(ini_2);
  EXPECT_TRUE(ini_3 == ini_2);

  ini::ini ini_4(ini::ini{ ini_3 });
  EXPECT_TRUE(ini_4 == ini_3);

  ini::ini ini_5 = ini_4;
  EXPECT_TRUE(ini_5 == ini_4);

  ini::ini ini_6 = ini::ini{ ini_5 };
  EXPECT_TRUE(ini_6 == ini_5);
}

TEST(ini, insert) {
  ini::ini ini{};

  ini.insert(ini::ini::section{ sect_name_1, p_1, p_2 });
  ASSERT_TRUE(ini[sect_name_1][key_1] == value_1);
  ASSERT_TRUE(ini[sect_name_1][key_2] == value_2);

  ini.clear();

  ini::ini::section s = ini::ini::section{ sect_name_1, p_1, p_2 };
  ini.insert(s);
  ASSERT_TRUE(ini[sect_name_1][key_1] == value_1);
  ASSERT_TRUE(ini[sect_name_1][key_2] == value_2);

  ini.clear();

  ini.insert(sect_name_1, p_1, p_2);
  ASSERT_TRUE(ini[sect_name_1][key_1] == value_1);
  ASSERT_TRUE(ini[sect_name_1][key_2] == value_2);
}

TEST(ini, insert_or_assign) {
  ini::ini ini{};
  ini::ini::section s = ini::ini::section{ sect_name_1, p_1, p_2 };
  ini.insert_or_assign(ini::ini::section{s});
  EXPECT_EQ(ini[sect_name_1], s);
  ASSERT_FALSE(ini.insert_or_assign(ini::ini::section{ sect_name_1 }).second);
}

TEST(ini, emplace) {
  ini::ini          ini{};
  ini::ini::section s_1{ sect_name_1, p_1 };
  auto              result = ini.emplace(s_1);
  ASSERT_TRUE(result.second);
  EXPECT_EQ(result.first->second, s_1);

  //
  //  Не понимаю как проверить возвращается ли верный итераторю Тоже самое и в следующем тесте на трай
  //

  s_1.insert(p_2);
  result = ini.emplace(s_1);
  ASSERT_FALSE(result.second);

  ini::ini::section s_2{ sect_name_2, p_2 };
  result = ini.emplace(ini::ini::section{ s_2 });
  ASSERT_TRUE(result.second);

  s_2.insert(p_1);
  result = ini.emplace(ini::ini::section{ s_2 });
  ASSERT_FALSE(result.second);
}

TEST(ini, try_emplace) {
  ini::ini          ini{};
  ini::ini::section s_1{ sect_name_1, p_1 };
  auto              result = ini.try_emplace(s_1);
  ASSERT_TRUE(result.second);

  s_1.insert(p_2);
  result = ini.try_emplace(s_1);
  ASSERT_FALSE(result.second);

  ini::ini::section s_2{ sect_name_2, p_2 };
  result = ini.try_emplace(ini::ini::section{ s_2 });
  ASSERT_TRUE(result.second);

  s_2.insert(p_1);
  result = ini.try_emplace(ini::ini::section{ s_2 });
  ASSERT_FALSE(result.second);
}

TEST(ini, square_brackets_operator) {
  ini::ini          ini{};
  ini::ini::section s_1{ sect_name_1, p_1 };
  bool in_catch = false;

  ini.emplace(s_1);
  EXPECT_EQ(ini[sect_name_1], s_1);
  try {
    ini[sect_name_2];
  } catch(const std::out_of_range& e) {
    ASSERT_TRUE(std::strstr(e.what(), "not found"));
    ASSERT_TRUE(std::strstr(e.what(), sect_name_2.c_str()));
    in_catch = true;
  }
  ASSERT_TRUE(in_catch);
}

TEST(ini, find) {
  ini::ini          ini{};
  ini::ini::section s_1{ sect_name_1, p_1 };
  ini.emplace(s_1);
  EXPECT_EQ(ini.find(sect_name_1)->second, s_1);
  // not found
}

TEST(ini, unsafe_access) {
  ;
  ;
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


