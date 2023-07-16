#include "cuckoo-hash.hpp"
#include <iostream>
#include <vector>

using namespace std;


template <typename key_t, typename value_t>
CuckooHash<key_t, value_t>::CuckooHash():
    table1_{new Item[2]}, 
    table2_{new Item[2]}, 
    epsilon_{0.4}, // ??
    size_{0},
    maxLoop_{1}, // ??
    numBuckets_{2},
    downsizeThresh_{0.2}
    {
        // Nothing here
    }

template<typename key_t, typename value_t>
CuckooHash<key_t, value_t>::CuckooHash(double epsilon, float downsizeThresh):
    table1_{new Item[2]}, 
    table2_{new Item[2]}, 
    epsilon_{epsilon}, // ??
    size_{0},
    maxLoop_{2}, // ??
    numBuckets_{2},
    downsizeThresh_{downsizeThresh}
    {
        // Nothing here
    }

template<typename key_t, typename value_t>
CuckooHash<key_t, value_t>::~CuckooHash(){
    delete[] table1_;
    delete[] table2_;
}

template<typename key_t, typename value_t>
size_t CuckooHash<key_t, value_t>::getHash1(const key_t& key) const {
    return hash1_(key);
}

template<typename key_t, typename value_t>
size_t CuckooHash<key_t, value_t>::getHash2(size_t hash1) const{
    string key_str;
    for (size_t byte = 0; byte < 8; ++byte)
    {
        unsigned char c = hash1 & 255;
        key_str += c;
        hash1 >>= 8;
    }
    return  hash2_(key_str);
}

template<typename key_t, typename value_t>
double CuckooHash<key_t, value_t>::loadFactor() const{
    return double(size_) / (2 * numBuckets_);
}

template <typename key_t, typename value_t>
void CuckooHash<key_t, value_t>::rehash(size_t numBuckets){
    vector<Item> allItems;
    for (Item *item = table1_; item < table1_ + numBuckets_; ++item)
    {
        if (item->valid_){
            allItems.push_back(*item);
        }
    }
    for (Item *item = table2_; item < table2_ + numBuckets_; ++item){
        if(item->valid_){
            allItems.push_back(*item);
        }
    }
    delete[] table1_;
    delete[] table2_;

    // Rehash into new table;
    numBuckets_ = numBuckets;
    table1_ = new Item[numBuckets_];
    table2_ = new Item[numBuckets_];
    for (Item &item : allItems)
    {
        insert(item.key_, item.value_, false);
    }
    return;
}

template <typename key_t, typename value_t>
bool CuckooHash<key_t, value_t>::exists(const key_t& key) const {
    size_t hash1 = getHash1(key);
    Item &item1 = table1_[hash1 % numBuckets_];
    if (item1.valid_ and item1.key_ == key){
        return item1.value_;
    } else {
        // Only compute hash2 if not found in hash1. Hashing is expensive
        size_t hash2 = getHash2(hash1);
        Item &item2 = table2_[hash2 % numBuckets_];
        return item2.valid_ and item2.key_ == key;
    }
    return false;
}

template <typename key_t, typename value_t>
void CuckooHash<key_t, value_t>::insert(const key_t& key, const value_t& value, bool updateValues){
    key_t keyCopy = key;
    value_t valueCopy = value;

    Item newItem = Item(keyCopy, valueCopy); // Copy constructor is needed. 
    if (exists(key)) {
        return;
    } else {
        for (size_t loops = 0; loops < maxLoop_; ++loops){
            size_t h1 = getHash1(newItem.key_);
            // Empty spot, insert and finish
            if (!table1_[h1%numBuckets_].valid_){
                table1_[h1%numBuckets_] = newItem;
                if(updateValues){
                    ++size_;
                    maxLoop_ = 3*size_t(ceil(log(size_) / log(1 + epsilon_))) + 1;
                }
                return;
            }
            else {
                std::swap(newItem, table1_[h1 % numBuckets_]);
            }
            size_t h2 = getHash2(getHash1(newItem.key_)); // This is slow!
            if (!table2_[h2 % numBuckets_].valid_){
                table2_[h2 % numBuckets_] = newItem;
                if(updateValues){
                    ++size_;
                    maxLoop_ = 3*size_t(ceil(log(size_) / log(1 + epsilon_))) + 1;
                }
                return;
            } else {
                std::swap(newItem, table2_[h2 % numBuckets_]);
            }
        }
        // Rehash and insert the new item.
        rehash(numBuckets_ * 2);
        insert(newItem.key_, newItem.value_, true);
    }
    return;
}

template <typename key_t, typename value_t>
void CuckooHash<key_t, value_t>::insert(const key_t& key, const value_t& value){
    insert(key, value, true);
}

template <typename key_t, typename value_t>
void CuckooHash<key_t, value_t>::remove(const key_t& key){
    if (exists(key)){
        size_t hash1 = getHash1(key);
        Item &item1 = table1_[hash1 % numBuckets_];
        if (item1.valid_ and item1.key_ == key) [[likely]]{
            item1.valid_ = false;
        } else [[unlikely]]{
            // Only compute hash2 if not found in hash1. Hashing is expensive
            size_t hash2 = getHash2(hash1);
            Item &item2 = table2_[hash2 % numBuckets_];
            if(item2.valid_ and item2.key_ == key){
                item2.valid_ = false;
            }
        }
        // Find the new maximum loop size
        --size_;
        maxLoop_ = 3*size_t(ceil(log(size_) / log(1 + epsilon_)));

        // Check if resizing is needed. Critical threshold is (1+e)n/4
        if (downsizeThresh_ > loadFactor())
        {
            rehash(numBuckets_ / 2);
        }
    }
    return;
}

template <typename key_t, typename value_t>
value_t& CuckooHash<key_t, value_t>::lookup(const key_t& key)  const {
    // Assume that exists has been called
    size_t hash1 = getHash1(key);
    Item& item1 = table1_[hash1 % numBuckets_];
    if (item1.valid_ and item1.key_ == key)
    {
        return item1.value_;
    } else {
        size_t hash2 = getHash2(hash1);
        Item &item2 = table2_[hash2 % numBuckets_];
        // If the key is not in the table, this is wrong!
        return item2.value_;
    }
}

template <typename key_t, typename value_t>
value_t& CuckooHash<key_t, value_t>::operator[](const key_t& key) {
    return lookup(key);
}

template <typename key_t, typename value_t>
void CuckooHash<key_t, value_t>::printToStream(ostream& out) const {
    out << "Table 1: [ ";
    for (Item *item = table1_; item < table1_ + numBuckets_; ++item)
    {
        if (item->valid_){
            out << "(" << item->key_ << ": " << item->value_ << ") ";
        } else {
            out << "(-:-) ";
        }
    }
    out << "]\nTable 2: [ ";

    for (Item *item = table2_; item < table2_ + numBuckets_; ++item)
    {
        if (item->valid_){
            out << "(" << item->key_ << ": " << item->value_ << ") ";
        } else {
            out << "(-:-) ";
        }
    }
    out << "]\n Epsilon: " << epsilon_ << " Num Buckets: " << numBuckets_ << " Size: " << size_ << " Max Loops: " << maxLoop_;
}

template <typename key_t, typename value_t>
CuckooHash<key_t, value_t>::Item::Item():valid_{false}{}

template <typename key_t, typename value_t>
CuckooHash<key_t, value_t>::Item::Item(key_t& key, value_t& value):
key_{key}, value_{value},valid_{true}{}

template <typename key_t, typename value_t>
ostream& operator<<(ostream& os, const CuckooHash<key_t, value_t>& ch){
    ch.printToStream(os);
    return os;
}
