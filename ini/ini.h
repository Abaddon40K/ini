#pragma once
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <functional>
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
      template<typename... Args>
      section(std::string_view string_name, Args&&... args) : name{ string_name } {
        data.emplace(std::forward<Args>(args)...);
      }
      section(const section& rhs) : name{ rhs.name }, data{ rhs.data } {}
      section(section&& rhs) noexcept : name{ std::move(rhs.name) }, data{ std::move(rhs.data) } {}

      section& operator=(const section&);
      section& operator=(section&&);

      ~section() {}

      iterator       begin() noexcept { return data.begin(); }
      const_iterator begin() const noexcept { return data.begin(); }

      iterator       end() noexcept { return data.end(); }
      const_iterator end() const noexcept { return data.end(); }

      const_iterator cbegin() const noexcept { return data.cbegin(); }
      const_iterator cend() const noexcept { return data.cend(); }

      void clear() {
        name = {};
        data.clear();
      }

      std::size_t erase(const std::string& key) { return data.erase(key); }
      iterator    erase(iterator it) { return data.erase(it); }
      iterator    erase(const_iterator it) { return data.erase(it); }

      // написать тесты
      std::pair<std::string&, std::string&> extract(const_iterator position) {
        auto node = data.extract(position);
        return { node.key(), node.mapped() };
      }
      // extract by key

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

      bool empty() const noexcept { return data.empty(); }

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

      std::string&       operator[](const std::string&);
      const std::string& operator[](const std::string&) const;

      bool operator==(const section&) const noexcept;
      bool operator!=(const section& rhs) const noexcept { return !(*this == rhs); }

     public:
      std::string name;

     private:
      data_type data;
    };


   public:
    using data_type      = std::unordered_map<std::string, section>;
    using iterator       = data_type::iterator;
    using const_iterator = data_type::const_iterator;

   public:
    ini() {}
    ini(const fs::path&);
    ini(const ini& rhs) : data{ rhs.data } {}
    ini(ini&& rhs) noexcept : data{ std::move(rhs.data) } {}

    ini& operator=(const ini&);
    ini& operator=(ini&&);

    ~ini() {}

    iterator       begin() noexcept { return data.begin(); }
    const_iterator begin() const noexcept { return data.begin(); }

    iterator       end() noexcept { return data.end(); }
    const_iterator end() const noexcept { return data.end(); }

    const_iterator cbegin() const noexcept { return data.cbegin(); }
    const_iterator cend() const noexcept { return data.cend(); }

    void clear() noexcept { data.clear(); }

    bool empty() const noexcept { return data.empty(); }

    std::size_t erase(const std::string& name) { return data.erase(name); }
    iterator    erase(iterator it) { return data.erase(it); }

    void swap(ini& rhs) noexcept { std::swap(data, rhs.data); }

    void insert(const section& s) { data.insert({ s.name, s }); }
    void insert(section&& s) {
      std::string name = s.name;
      data.insert({ std::move(name), s });
    }
    template<typename... Args>
    void insert(std::string section_name, Args... args);

    std::size_t size() { return data.size(); }

    section&       unsafe_access(const std::string&);
    const section& unsafe_access(const std::string&) const;

    section&       operator[](const std::string&);
    const section& operator[](const std::string&) const;

    iterator       find(const std::string& name) { return data.find(name); }
    const_iterator find(const std::string& name) const { return data.find(name); }

    bool operator==(const ini&) const noexcept;
    bool operator!=(const ini& rhs) const noexcept { return !(*this == rhs); }

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



  ini parse(std::string_view);
  ini parse_from_file(const fs::path&);

  template<typename... Args>
  inline void ini::insert(std::string section_name, Args... args) {
    section temp{ section_name };
    detail::insert_helper(temp, args...);
    data.insert({ section_name, std::move(temp) });
  }
}  // namespace ini
