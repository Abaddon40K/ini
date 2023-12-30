#pragma once
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <type_traits>

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

  namespace detail {
    inline auto fublya(ini::section&);
    template<typename Head, typename... Args>
    inline auto fublya(ini::section& temp, Head head, Args... args);
  }  // namespace detail

  struct ini {
    struct section {
    public:
      using key_type       = std::string;
      using mapped_type    = std::string;
      using data_type      = std::unordered_map<key_type, mapped_type>;
      using value_type     = data_type::value_type;
      using iterator       = data_type::iterator;
      using const_iterator = data_type::const_iterator;
      std::string name;

      static constexpr inline const std::size_t _Bucket_size = data_type::_Bucket_size;
      static constexpr inline const std::size_t _Min_buckets = data_type::_Min_buckets;

      // Стоит ли делать указателем? Вдруг пользователь создаст отдельно сам такую же маппу, повлияет ли это на работу?
      // А если не создаст, но ему понадобится доступ к этой переменной? 
      // Так много вопросов и так мало ответов...
      static constexpr inline bool _Multi = data_type::_Multi;

      section() {}

      explicit section(std::string_view string_name) : name { string_name } {}
      explicit section(std::string&& string_name) : name { string_name } {}

      inline mapped_type       at(const key_type& key) { return data.at(key); }
      inline const mapped_type at(const key_type& key) const { return data.at(key); }

      iterator       begin() { return data.begin(); }
      const_iterator begin() const { return data.begin(); }
      iterator       end() { return data.end(); }
      const_iterator end() const { return data.end(); }
      const_iterator cbegin() { return data.cbegin(); }
      const_iterator cend() { return data.cend(); }

      inline std::size_t bucket(const key_type& key) const { return data.bucket(key); }
      inline std::size_t bucket_count() const { return data.bucket_count(); }
      inline std::size_t bucket_size(std::size_t bucket) const { return data.bucket_size(bucket); }
      inline void        clear() { data.clear(); }
      inline std::size_t count(const key_type& key) { return data.count(key); }
      template<typename... Args>
      std::pair<iterator, bool> emplace(Args&&... args) {
        return data.emplace(std::forward<Args>(args)...);
      }
      template<typename... Args>
      iterator emplace_hint(Args&&... args) {
        return data.emplace_hint(std::forward<Args>(args)...);
      }
      template<typename... Args>
      const_iterator emplace_hint(const Args&&... args) const {
        return data.emplace_hint(std::forward<Args>(args)...);
      }
      inline bool                          empty() { return data.empty(); }
      inline std::pair<iterator, iterator> equal_range(const key_type& key) { return data.equal_range(key); }
      inline std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return data.equal_range(key);
      }
      inline std::size_t erase(const key_type& key) { return data.erase(key); }
      iterator           erase(iterator it) { return data.erase(it); }
      iterator           erase(iterator begin, iterator end) { return data.erase(begin, end); }
      iterator           erase(const_iterator it) { return data.erase(it); }
      iterator erase(const_iterator first_it, const_iterator second_it) { return data.erase(first_it, second_it); }

      // Уточнить что за возвращаемый тип
      data_type::node_type extract(const_iterator position) { return data.extract(position); }

      inline iterator       find(const key_type& key) { return data.find(key); }
      inline const_iterator find(const key_type& key) const { return data.find(key); }

      inline std::hash<key_type> hash_function() { return data.hash_function(); }
      std::pair<iterator, bool>  insert(const value_type& lhs) { return data.insert(lhs); }
      std::pair<iterator, bool>  insert(value_type&& rhs) { return data.insert(rhs); }

      inline std::pair<iterator, bool> insert_or_assign(key_type&& key, mapped_type&& val) {
        return data.insert_or_assign(key, val);
      }
      inline std::pair<iterator, bool> insert_or_assign(const key_type& key, mapped_type&& val) {
        return data.insert_or_assign(key, val);
      }
      inline iterator insert_or_assign(const_iterator hint, key_type&& key, mapped_type&& val) {
        return data.insert_or_assign(hint, key, val);
      }
      inline iterator insert_or_assign(const_iterator hint, const key_type& key, mapped_type&& val) {
        return data.insert_or_assign(hint, key, val);
      }

      inline data_type::key_equal key_eq() const { return data.key_eq(); }
      inline float                load_factor() const noexcept { return data.load_factor(); }
      inline iterator             lower_bound(const key_type& key) { return data.lower_bound(key); }
      inline const_iterator       lower_bound(const key_type& key) const { return data.lower_bound(key); }
      inline std::size_t          max_bucket_count() const noexcept { return data.max_bucket_count(); }
      inline float                max_load_factor() const noexcept { return data.max_load_factor(); }
      inline void                 max_load_factor(float new_max) noexcept { data.max_load_factor(new_max); }
      inline std::size_t          max_size() { return data.max_size(); }

      // Требуется объяснение
      template<class other_traits>
      inline void merge(data_type::_Hash<other_traits>& lhs) {
        data.merge(lhs);
      }
      template<class other_traits>
      inline void merge(data_type::_Hash<other_traits>&& rhs) {
        data.merge(rhs);
      }

      inline void rehash(std::size_t buckets) { data.rehash(buckets); }

      inline void reserve(std::size_t count) { data.reserve(count); }

      std::size_t size() { return data.size(); }

      inline void swap(data_type& rhs) { data.swap(rhs); }

      // Вероятны косяки
      template<typename... Args>
      std::pair<iterator, bool> try_emplace(key_type& key, Args&&... args) {
        return data.try_emplace(key, args);
      }
      template<typename... Args>
      std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args) {
        return data.try_emplace(key, args);
      }
      template<typename... Args>
      iterator try_emplace(const_iterator hint, key_type& key, Args&&... args) {
        return data.try_emplace(hint, key, args);
      }
      template<typename... Args>
      iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args) {
        return data.try_emplace(hint, key, args);
      }

      inline iterator       upper_bound(const key_type& key) { return data.upper_bound(key); }
      inline const_iterator upper_bound(const key_type& key) const { return data.upper_bound(key); }

      data_type::_Unchecked_iterator _Unchecked_begin() noexcept { return data._Unchecked_begin(); }
      data_type::_Unchecked_iterator _Unchecked_end() noexcept { return data._Unchecked_end(); }

      void foo() {
        data._Bucket_size;
        data._Min_buckets;
        data._Multi;
      }

      std::string& operator[](const std::string&);
      section&     operator=(const ini&);
      section&     operator=(ini&&);



    private:
      data_type data;
    };

  public:
    using data_type              = std::vector<section>;
    using value_type             = data_type::value_type;
    using iterator               = data_type::iterator;
    using const_iterator         = data_type::const_iterator;
    using reverse_iterator       = data_type::reverse_iterator;
    using const_reverse_iterator = data_type::const_reverse_iterator;


  public:
    ini() {}
    ini::ini(const fs::path& path) : data { ini(parse_from_file(path)).data } {}
    ini(ini&& rhs) noexcept : data { rhs.data } {}

    inline void assign(std::initializer_list<value_type> ilist) { data.assign(ilist); }
    inline void assign(const std::size_t new_size, const value_type& value) { data.assign(new_size, value); }
    inline void assign(iterator begin, iterator end) { data.assign(begin, end); }

    inline value_type&       at(const std::size_t pos) { return data.at(pos); }
    inline const value_type& at(const std::size_t pos) const { return data.at(pos); }

    inline value_type&       back() { return data.back(); }
    inline const value_type& back() const { return data.back(); }

    inline iterator       begin() noexcept { return data.begin(); }
    inline const_iterator begin() const noexcept { return data.begin(); }
    inline iterator       end() noexcept { return data.end(); }
    inline const_iterator end() const noexcept { return data.end(); }
    inline const_iterator cbegin() const noexcept { return data.cbegin(); }
    inline const_iterator cend() const noexcept { return data.cend(); }

    inline reverse_iterator       rbegin() noexcept { return data.rbegin(); }
    inline const_reverse_iterator rbegin() const noexcept { return data.rbegin(); }
    inline reverse_iterator       rend() noexcept { return data.rend(); }
    inline const_reverse_iterator rend() const noexcept { return data.rend(); }
    inline const_reverse_iterator crbegin() const noexcept { return data.crbegin(); }
    inline const_reverse_iterator crend() const noexcept { return data.crend(); }

    inline std::size_t capacity() const noexcept { return data.capacity(); }

    inline void clear() noexcept { data.clear(); }

    // Заняться конфликтом имён
    section*       ddata() noexcept { return data.data(); }
    const section* ddata() const { return data.data(); }

    template<typename... Args>
    inline iterator emplace(const_iterator it, Args&&... args) {
      return data.emplace(it, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline decltype(auto) emplace_back(Args&&... args) {
      return data.emplace_back(std::forward<Args>(args)...);
    }

    inline bool empty() const noexcept { return data.empty(); }

    inline iterator erase(iterator begin, iterator end) { return data.erase(begin, end); }
    inline iterator erase(iterator it) { return data.erase(it); }

    inline value_type& front() noexcept { return data.front(); }

    inline data_type::allocator_type get_allocator() const noexcept { return data.get_allocator(); }

    inline iterator insert(const_iterator it, const value_type& val) { return data.insert(it, val); }
    inline iterator insert(const_iterator it, value_type&& val) { return data.insert(it, val); }
    inline iterator insert(const_iterator it, const std::size_t count, const value_type& val) {
      return data.insert(it, count, val);
    }
    inline iterator insert(const_iterator it, iterator begin, iterator end) { return data.insert(it, begin, end); }
    inline iterator insert(const_iterator it, std::initializer_list<value_type> list) { return data.insert(it, list); }

    inline std::size_t max_size() { return data.max_size(); }

    inline void pop_back() noexcept { data.pop_back(); }

    inline void push_back(const value_type& lhs) { data.push_back(lhs); }
    inline void push_back(value_type&& rhs) noexcept(std::is_nothrow_move_constructible_v<value_type>) {
      data.push_back(rhs);
    }

    inline void reserve(std::size_t new_cap) { data.reserve(new_cap); }

    inline void resize(const std::size_t new_size) { data.resize(new_size); }
    inline void resize(const std::size_t new_size, const value_type& val) { data.resize(new_size, val); }

    inline void shrink_to_fit() { data.shrink_to_fit(); }

    inline void swap(ini& right) noexcept { data.swap(right.data); }
    inline void swap(data_type& right) noexcept { data.swap(right); }

    void foo() {}

    // Это что и нахуя мы написали?
    template<typename... Args>
    void insert(std::string section_name, Args... args) {
      value_type temp { section_name };
      detail::fublya(temp, args...);
      data.push_back(std::move(temp));
    }



    inline std::size_t ini::size() { return data.size(); }
    value_type& operator[](std::string);
    ini&          operator=(const ini&);
    ini&          operator=(ini&&);
    std::ostream& dump(std::ostream&);


    static ini parse(std::string_view);
    static ini parse_from_file(const fs::path&);

  private:
    data_type data {};
  };


  namespace detail {
    inline auto fublya(ini::section&) {}

    template<typename Head, typename... Args>
    inline auto fublya(ini::section& temp, Head head, Args... args) {
      temp.insert(head);
      fublya(temp, args...);
    }
  }  // namespace detail
}  // namespace ini
