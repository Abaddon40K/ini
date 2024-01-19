#pragma once
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

/*
 *  TODO: Добавить политики для ini
 */
namespace ini {
  constexpr inline char        comment_begin = '#';
  constexpr inline char        section_begin = '[';
  constexpr inline char        section_end   = ']';
  constexpr inline char        separator     = '=';
  constexpr inline std::size_t breacket_len  = sizeof(section_begin);
  constexpr inline std::size_t separator_len = sizeof(separator);
  namespace fs                               = std::filesystem;

  struct section;

  struct ini {
    ///
    /// Надо добавить во все вставки перезапись или проверку наличия такого же
    /// ключа
    ///
    struct section {
     public:
      using data_type      = std::unordered_map<std::string, std::string>;
      using value_type     = data_type::value_type;
      using iterator       = data_type::iterator;
      using const_iterator = data_type::const_iterator;

     public:
      section() {}
      explicit section(std::string_view string_name) : name{ string_name } {}
      explicit section(std::string&& string_name) noexcept : name{ string_name } {}

      iterator       begin() noexcept { return data.begin(); }
      const_iterator begin() const noexcept { return data.begin(); }

      iterator       end() noexcept { return data.end(); }
      const_iterator end() const noexcept { return data.end(); }

      const_iterator cbegin() const noexcept { return data.cbegin(); }
      const_iterator cend() const noexcept { return data.cend(); }

      void clear() { data.clear(); }

      bool empty() const noexcept { return data.empty(); }

      std::size_t erase(const std::string& key) { return data.erase(key); }
      iterator    erase(iterator it) { return data.erase(it); }
      iterator    erase(iterator begin, iterator end) { return data.erase(begin, end); }
      iterator    erase(const_iterator it) { return data.erase(it); }
      iterator    erase(const_iterator first_it, const_iterator second_it) { return data.erase(first_it, second_it); }

      // написать тесты
      std::pair<std::string&, std::string&> extract(const_iterator position) {
        auto node = data.extract(position);
        return { node.key(), node.mapped() };
      }

      iterator       find(const std::string& key) { return data.find(key); }
      const_iterator find(const std::string& key) const { return data.find(key); }

      // overwrite
      std::pair<iterator, bool> insert(const value_type& pair) { return data.insert(pair); }
      // overwrite
      std::pair<iterator, bool> insert(value_type&& pair) { return data.insert(pair); }

      std::pair<iterator, bool> insert_or_assign(std::string&& key, std::string&& val) {
        return data.insert_or_assign(key, val);
      }
      std::pair<iterator, bool> insert_or_assign(const std::string& key, std::string&& val) {
        return data.insert_or_assign(key, val);
      }

      void reserve(std::size_t count) { data.reserve(count); }

      std::size_t size() const noexcept { return data.size(); }

      void swap(section& rhs) noexcept {
        std::swap(data, rhs.data);
        std::swap(name, rhs.name);
      }

      template<typename... Args>
      std::pair<iterator, bool> emplace(Args&&... args) {
        return data.emplace(std::forward<Args>(args)...);
      }

      template<typename... Args>
      std::pair<iterator, bool> try_emplace(const std::string& key, Args&&... args) {
        return data.try_emplace(key, std::forward<Args>(args)...);
      }
      template<typename... Args>
      std::pair<iterator, bool> try_emplace(std::string&& key, Args&&... args) {
        return data.try_emplace(key, std::forward<Args>(args)...);
      }

      std::string& operator[](const std::string&);

      section& operator=(const section&);
      // section&     operator=(section&&);

      bool operator==(const section&);
      bool operator==(section&&);

      bool operator!=(const section& rhs) { return !(*this == rhs); }
      bool operator!=(section&& rhs) { return !(*this == rhs); }

     public:
      std::string name;

     private:
      data_type data;
    };

   public:
    using data_type = std::vector<section>;
    // using data_type = std::unordered_map<std::string, section>;
    using iterator       = data_type::iterator;
    using const_iterator = data_type::const_iterator;

   public:
    ini() {}
    ini(const fs::path&);
    ini(ini&& rhs) noexcept : data{ std::move(rhs.data) } {}

    iterator       begin() noexcept { return data.begin(); }
    const_iterator begin() const noexcept { return data.begin(); }

    iterator       end() noexcept { return data.end(); }
    const_iterator end() const noexcept { return data.end(); }

    const_iterator cbegin() const noexcept { return data.cbegin(); }
    const_iterator cend() const noexcept { return data.cend(); }

    void clear() noexcept { data.clear(); }

    bool empty() const noexcept { return data.empty(); }

    iterator erase(const_iterator begin, const_iterator end) { return data.erase(begin, end); }
    iterator erase(iterator it) { return data.erase(it); }

    void swap(ini& rhs) noexcept { std::swap(data, rhs.data); }

    void insert(const section& s) { data.push_back(s); }
    void insert(section&& s) { data.push_back(s); }
    template<typename... Args>
    void insert(std::string section_name, Args... args);

    std::size_t size() { return data.size(); }

    // section& unsafe_access(std::size_t element) { return data[element]; }

    section& operator[](std::string);

    ini& operator=(const ini&);
    ini& operator=(ini&&);

    bool operator==(const ini&);
    bool operator==(ini&&);

    bool operator!=(const ini& rhs) { return !(*this == rhs); }
    bool operator!=(ini&& rhs) { return !(*this == rhs); }

    std::ostream& dump(std::ostream&);

   private:
    data_type data{};
  };

  namespace detail {
    inline auto insert_helper(ini::section&) {}
    template<typename Head, typename... Args>
    inline auto insert_helper(ini::section& temp, Head head, Args... args) {
      temp.insert(head);
      insert_helper(temp, args...);
    }
  }  // namespace detail

  template<typename... Args>
  inline void ini::insert(std::string section_name, Args... args) {
    section temp{ section_name };
    detail::insert_helper(temp, args...);
    data.push_back(std::move(temp));
  }

  ini parse(std::string_view);
  ini parse_from_file(const fs::path&);

}  // namespace ini
