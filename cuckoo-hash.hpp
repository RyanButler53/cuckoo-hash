/**
 * @file cuckoo-hash.hpp
 * @author Ryan Butler (rbutler@g.hmc.edu)
 * @brief Implementation of a Cuckoo Hash Set and Hash Map 
 * @note Keys and Values must implement a copy constructor
 * @version 1.0
 * @date 2023-07-04
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstddef>
#include <string>
#include <cmath>
#include <vector>
#include <iterator>
#include <tuple>

#ifndef CUCKOO_HASH_HPP_INCLUDED
#define CUCKOO_HASH_HPP_INCLUDED

template <typename key_t, typename value_t>
class CuckooHashMap
{
  private:
    class const_iterator;

    struct Item {
        key_t key_;
        value_t value_;
        bool valid_; // Way to check if it is valid after deletions.

        Item(); // For invalid deleted items. 
        Item(key_t &key, value_t &value); // For valid items. 
        Item(const Item &other) = default;
        ~Item() = default;
    };

    // Data
    Item* table1_;
    Item* table2_;
    double epsilon_;
    size_t size_;
    size_t maxLoop_; // set to 3 log_1+e(n)
    size_t numBuckets_;
    std::hash<key_t> hash1_;
    std::hash<std::string> hash2_;
    float downsizeThresh_;

    // Helper Functions
    size_t getHash1(const key_t& key) const;
    size_t getHash2(size_t hash1) const;
    void rehash(size_t numBuckets);
    void insert(const key_t& key, const value_t& value, bool updateValues);

  public:
    // Constructors
    CuckooHashMap();
    CuckooHashMap(double epsilon, float downsizeThresh);
    ~CuckooHashMap();
    CuckooHashMap(const CuckooHashMap &other) = delete;

    // Modification and Lookup;
    bool contains(const key_t &key) const;
    void insert(const key_t& key, const value_t& value);
    void erase(const key_t& key); 
    value_t &lookup(const key_t& key) const;
    void clear();

    // Data Lookup
    bool empty() const;
    size_t size() const;
    double loadFactor() const;

    // Iterator Functions
    const_iterator begin() const;
    const_iterator end() const;

    value_t &operator[](const key_t& key);
    void printToStream(std::ostream &os) const;

  private: 
    class const_iterator {
        friend class CuckooHashMap;

  private: 
        Item *t1_;
        Item *t2_;
        size_t idx_;
        size_t tableSize_; // numBuckets_

        /**
         * @brief Iterates over a table until a new idx is found. 
         */
        void iterateTable();

  public:
        using value_type = std::tuple<key_t, value_t>;
        using reference = const value_type&;
        using pointer = const value_type*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        const_iterator() = default;
        const_iterator(size_t idx, size_t tableSize, Item* table1, Item* table2);
        const_iterator(const const_iterator &other) = delete;
        const_iterator &operator=(const const_iterator &other) = default;
        ~const_iterator() = default;

        value_type operator*() const;
        const_iterator &operator++();
        pointer operator->() const;
        bool operator==(const const_iterator &other) const;
        bool operator!=(const const_iterator &other) const;

    };
};

template<typename T>
class CuckooHashSet
{
  private:
    class const_iterator;

    // Data
    std::vector<bool> valid1_;
    std::vector<bool> valid2_;
    double epsilon_;
    size_t size_;
    T* table1_;
    T* table2_;
    size_t maxLoop_; // set to 3 log_1+e(n)
    size_t numBuckets_;
    std::hash<T> hash1_;
    std::hash<std::string> hash2_;
    float downsizeThresh_;

    // Helper Functions
    size_t getHash1(const T& key) const;
    size_t getHash2(size_t hash1) const;
    void rehash(size_t numBuckets);
    void insert(const T& key, bool updateValues);

  public:

    // Constructors
    CuckooHashSet();
    CuckooHashSet(double epsilon, float downsizeThresh);
    ~CuckooHashSet();
    CuckooHashSet(const CuckooHashSet&other) = delete;

    // Capacity
    bool empty() const;
    size_t size() const;
    double loadFactor() const;

    // Modification and Lookup
    bool contains(const T &key) const;
    void insert(const T& key);
    void erase(const T& key);
    void clear();

    // Iterators
    const_iterator begin() const;
    const_iterator end() const;

    void printToStream(std::ostream &os) const;
    
  private:

    class const_iterator {
        friend class CuckooHashSet;

    private:
        const std::vector<bool>& v1_;
        const std::vector<bool>& v2_;
        size_t idx_;
        T *t1_;
        T *t2_;

        void iterateTable();

    public:
        using value_type = T;
        using reference = const value_type &;
        using pointer = const value_type *;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        const_iterator() = default;
        const_iterator(size_t idx, T* table1, T* table2, const std::vector<bool>& valid1, const std::vector<bool>& valid2);
        const_iterator(const const_iterator &other) = delete;
        const_iterator &operator=(const const_iterator &other) = default;
        ~const_iterator() = default;

        value_type operator*() const;
        const_iterator &operator++();
        pointer operator->() const;
        bool operator==(const const_iterator &other) const;
        bool operator!=(const const_iterator &other) const;
    };
};

template<typename key_t,typename value_t>
std::ostream &operator<<(std::ostream& os, const CuckooHashMap<key_t, value_t> &ch );

template<typename T>
std::ostream &operator<<(std::ostream& os, const CuckooHashSet<T> &ch );
#include "cuckoo-hash-private.hpp"

#endif // CUCKOO_HASH_HPP_INCLUDED
