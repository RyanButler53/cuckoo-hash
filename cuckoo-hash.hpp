/**
 * @file cuckoo-hash.hpp
 * @author Ryan Butler (rbutler@g.hmc.edu)
 * @brief Implementation of a Cuckoo Hash Table 
 * @note Keys and Values must implement a copy constructor
 * @version 0.1
 * @date 2023-07-04
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstddef>
#include <string>
#include <cmath>

#ifndef CUCKOO_HASH_HPP_INCLUDED
#define CUCKOO_HASH_HPP_INCLUDED

template <typename key_t, typename value_t>
class CuckooHash
{
  private:
    /* data */
    struct Item
    {
        key_t key_;
        value_t value_;
        bool valid_; // Way to check if it is valid after deletions.

        Item();
        Item(key_t& key, value_t& value);
        Item(const Item &other) = default;
        ~Item() = default;
    };

    Item* table1_;
    Item* table2_;
    double epsilon_;
    size_t size_;
    size_t maxLoop_; // set to 3 log_1+e(n)
    size_t numBuckets_;
    std::hash<key_t> hash1_;
    std::hash<std::string> hash2_;
    float downsizeThresh_;

    // bool set // For determining if it is a set or a dictionary

    size_t getHash1(const key_t& key) const;
    size_t getHash2(size_t hash1) const;
    void rehash(size_t numBuckets);
    void insert(const key_t& key, const value_t& value, bool updateValues);


  public:
    CuckooHash();
    CuckooHash(double epsilon, float downsizeThresh);
    ~CuckooHash();
    CuckooHash(const CuckooHash &other) = delete;

    bool exists(const key_t &key) const; 
    void insert(const key_t& key, const value_t& value);
    void remove(const key_t& key); 
    value_t &lookup(const key_t& key) const; 

    value_t &operator[](const key_t& key) const;
    void printToStream(std::ostream &os) const;
};

template<typename key_t,typename value_t>
std::ostream &operator<<(std::ostream& os, const CuckooHash<key_t, value_t> &ch );

#include "cuckoo-hash-private.hpp"

#endif // CUCKOO_HASH_HPP_INCLUDED
