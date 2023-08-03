#include "cuckoo-hash.hpp"
#include <iostream>

using namespace std;

/*******************
 * Cuckoo Hash Map *
 *******************/

template <typename key_t, typename value_t>
CuckooHashMap<key_t, value_t>::CuckooHashMap():
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
CuckooHashMap<key_t, value_t>::CuckooHashMap(double epsilon, float downsizeThresh):
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
CuckooHashMap<key_t, value_t>::~CuckooHashMap(){
    delete[] table1_;
    delete[] table2_;
}

template<typename key_t, typename value_t>
size_t CuckooHashMap<key_t, value_t>::getHash1(const key_t& key) const {
    return hash1_(key);
}

template<typename key_t, typename value_t>
size_t CuckooHashMap<key_t, value_t>::getHash2(size_t hash1) const {
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
double CuckooHashMap<key_t, value_t>::loadFactor() const{
    return double(size_) / (2 * numBuckets_);
}

template<typename key_t, typename value_t>
bool CuckooHashMap<key_t, value_t>::empty() const{
    return size_ == 0;
}

template<typename key_t, typename value_t>
size_t CuckooHashMap<key_t, value_t>::size() const{
    return size_;
}

template <typename key_t, typename value_t>
void CuckooHashMap<key_t, value_t>::clear(){
    delete[] table1_;
    delete[] table2_;
    table1_ = new Item[2];
    table2_ = new Item[2];
    numBuckets_ = 2;
    maxLoop_ = 1;
    size_ = 0;
}

template <typename key_t, typename value_t>
void CuckooHashMap<key_t, value_t>::rehash(size_t numBuckets){
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
bool CuckooHashMap<key_t, value_t>::contains(const key_t& key) const {
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
void CuckooHashMap<key_t, value_t>::insert(const key_t& key, const value_t& value, bool updateValues){
    key_t keyCopy = key;
    value_t valueCopy = value;

    Item newItem = Item(keyCopy, valueCopy); // Copy constructor is needed. 
    if (contains(key)) {
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
void CuckooHashMap<key_t, value_t>::insert(const key_t& key, const value_t& value){
    insert(key, value, true);
}

template <typename key_t, typename value_t>
void CuckooHashMap<key_t, value_t>::erase(const key_t& key){
    if (contains(key)){
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
value_t& CuckooHashMap<key_t, value_t>::lookup(const key_t& key)  const {
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
value_t& CuckooHashMap<key_t, value_t>::operator[](const key_t& key) {
    return lookup(key);
}

template <typename key_t, typename value_t>
void CuckooHashMap<key_t, value_t>::printToStream(ostream& out) const {
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
CuckooHashMap<key_t, value_t>::Item::Item():valid_{false}{
}

template <typename key_t, typename value_t>
CuckooHashMap<key_t, value_t>::Item::Item(key_t& key, value_t& value):
key_{key}, value_{value},valid_{true}{}

template <typename key_t, typename value_t>
ostream& operator<<(ostream& os, const CuckooHashMap<key_t, value_t>& ch){
    ch.printToStream(os);
    return os;
}

// Iterator Functions

template <typename key_t, typename value_t>
typename CuckooHashMap<key_t, value_t>::const_iterator CuckooHashMap<key_t, value_t>::begin() const {
    return const_iterator(0, numBuckets_, table1_, table2_);
}

template <typename key_t, typename value_t>
typename CuckooHashMap<key_t, value_t>::const_iterator CuckooHashMap<key_t, value_t>::end() const {
    return const_iterator(2 * numBuckets_, numBuckets_, table1_, table2_);
}

template <typename key_t, typename value_t>
CuckooHashMap<key_t, value_t>::const_iterator::const_iterator(size_t idx, size_t tableSize, Item* t1, Item* t2):
    t1_{t1}, t2_{t2}, idx_{idx}, tableSize_{tableSize}{
    iterateTable();
}

template <typename key_t, typename value_t>
typename CuckooHashMap<key_t, value_t>::const_iterator& CuckooHashMap<key_t, value_t>::const_iterator::operator++() {
    ++idx_;
    iterateTable();
    return *this;
}

template <typename key_t,typename value_t>
void CuckooHashMap<key_t, value_t>::const_iterator::iterateTable(){
    // First Table
    while (idx_ < tableSize_) {
        if (t1_[idx_].valid_){
            return;
        } else {
            ++idx_;
        }
    }
    // Second Table
    while (idx_ < 2 * tableSize_){
        if (t2_[idx_-tableSize_].valid_){
            return;
        } else {
            ++idx_;
        }
    }
}

template <typename key_t, typename value_t>
typename CuckooHashMap<key_t, value_t>::const_iterator::value_type CuckooHashMap<key_t, value_t>::const_iterator::operator*() const{
    if (idx_ < tableSize_) {
        return {t1_[idx_].key_, t1_[idx_].value_};

    } else {
        return {t2_[idx_ - tableSize_].key_, t2_[idx_-tableSize_].value_};

    }
}

template <typename key_t, typename value_t>
bool CuckooHashMap<key_t, value_t>::const_iterator::operator==(const const_iterator& other) const {
    return (idx_ == other.idx_) and (t1_ == other.t1_) and (t2_ == other.t2_);
}

template <typename key_t, typename value_t>
bool CuckooHashMap<key_t, value_t>::const_iterator::operator!=(const const_iterator& other) const{
    return !(*this == other);
}

template <typename key_t, typename value_t>
typename CuckooHashMap<key_t, value_t>::const_iterator::pointer CuckooHashMap<key_t, value_t>::const_iterator::operator->() const{
    return &(**this);
}

/*******************
 * Cuckoo Hash Set *
 *******************/

template <typename T>
CuckooHashSet<T>::CuckooHashSet():valid1_{false, false}, valid2_{false, false},
    epsilon_{0.4}, size_{0}, table1_{new T[2]}, table2_{new T[2]}, maxLoop_{1},
    numBuckets_{2}, downsizeThresh_{0.2}{
    // Nothing here
}

template <typename T>
CuckooHashSet<T>::CuckooHashSet(double epsilon, float downsizeThresh):
    valid1_{false, false}, valid2_{false, false}, epsilon_{epsilon}, 
    size_{0}, table1_{new T[2]}, table2_{new T[2]}, maxLoop_{1},
    numBuckets_{2}, downsizeThresh_{downsizeThresh} {

}

template <typename T>
CuckooHashSet<T>::~CuckooHashSet(){
    delete[] table1_;
    delete[] table2_;
}

template <typename T>
size_t CuckooHashSet<T>::getHash1(const T& key) const {
    return hash1_(key);
}

template <typename T>
size_t CuckooHashSet<T>::getHash2(size_t hash1) const{
    string key_str;
    for (size_t byte = 0; byte < 8; ++byte)
    {
        unsigned char c = hash1 & 255;
        key_str += c;
        hash1 >>= 8;
    }
    return  hash2_(key_str);
}

template <typename T>
double CuckooHashSet<T>::loadFactor() const {
    return double(size_) / (2 * numBuckets_);
}

template <typename T>
void CuckooHashSet<T>::rehash(size_t numBuckets){
    vector<T> allKeys;
    for (size_t i = 0; i < numBuckets_;++i)
    {
        if (valid1_[i]){
            allKeys.push_back(table1_[i]);
        }
    }
    for (size_t i = 0; i < numBuckets_;++i)
    {
        if (valid2_[i]){
            allKeys.push_back(table2_[i]);
        }
    }

    // Clear old tables
    delete[] table1_;
    delete[] table2_;

    // Rehash into new table;
    numBuckets_ = numBuckets;
    table1_ = new T[numBuckets_];
    table2_ = new T[numBuckets_];
    valid1_.resize(numBuckets_);
    valid2_.resize(numBuckets_);
    std::fill(valid1_.begin(), valid1_.end(), false);
    std::fill(valid2_.begin(), valid2_.end(), false);

    // Re-insert all items
    for (const T& key : allKeys)
    {
        insert(key, false);
    }
}

template<typename T>
void CuckooHashSet<T>::insert(const T&key, bool updateValues){
    T keyCopy = key;
    T newKey = key;
    if (contains(key))
    {
        return;
    }
    else
    {
        for (size_t loops = 0; loops < maxLoop_; ++loops){
            size_t h1 = getHash1(newKey);
            // Empty spot, insert and finish
            if (!valid1_[h1%numBuckets_]){
                table1_[h1%numBuckets_] = newKey;
                valid1_[h1 % numBuckets_] = true;
                if (updateValues)
                {
                    ++size_;
                    maxLoop_ = 3*size_t(ceil(log(size_) / log(1 + epsilon_))) + 1;
                }
                return;
            } else {
                std::swap(newKey, table1_[h1 % numBuckets_]);
            }
            size_t h2 = getHash2(getHash1(newKey)); // This is slow!
            if (!valid2_[h2 % numBuckets_]){
                table2_[h2 % numBuckets_] = newKey;
                valid2_[h2 % numBuckets_] = true;
                if(updateValues){
                    ++size_;
                    maxLoop_ = 3*size_t(ceil(log(size_) / log(1 + epsilon_))) + 1;
                }
                return;
            } else {
                std::swap(newKey, table2_[h2 % numBuckets_]);
            }
        }
        // Rehash and insert the new item.
        rehash(numBuckets_ * 2);
        insert(newKey, true);
    }
    return;
}

template<typename T>
bool CuckooHashSet<T>::empty() const {
    return size_ == 0;
}

template <typename T>
size_t CuckooHashSet<T>::size() const {
    return size_ == 0;
}

template <typename T>
void CuckooHashSet<T>::insert(const T &key){
    insert(key, true);
}

template <typename T>
void CuckooHashSet<T>::erase(const T& key){
    if (contains(key)){
        size_t hash1 = getHash1(key);
        size_t table1Ind = hash1%numBuckets_;
        if (valid1_[table1Ind] and table1_[table1Ind] == key) [[likely]]{
            valid1_[table1Ind] = false;
        } else [[unlikely]]{
            // Only compute hash2 if not found in hash1. Hashing is expensive
            size_t hash2 = getHash2(hash1);
            size_t table2Ind = hash2 % numBuckets_;
            if (valid2_[table2Ind] and table2_[table2Ind] == key)
            {
                valid2_[table2Ind] = false;
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

template <typename T>
bool CuckooHashSet<T>::contains(const T& key)const {
    size_t hash1 = getHash1(key);
    size_t ind1 = hash1 % numBuckets_;
    if (valid1_[ind1] and table1_[ind1] == key) {
        return true;
    } else {
        size_t hash2 = getHash2(hash1) % numBuckets_;
        size_t ind2 = hash2 % numBuckets_;
        return valid2_[ind2] and table2_[ind2] == key;
    }
}

template<typename T>
void CuckooHashSet<T>::clear(){
    delete[] table1_;
    delete[] table2_;
    table1_ = new T[2];
    table2_ = new T[2];
    valid1_ = {false, false};
    valid2_ = {false, false};
    numBuckets_ = 2;
    maxLoop_ = 1;
    size_ = 0;
}

template <typename T>
void CuckooHashSet<T>::printToStream(ostream &out) const{
    out << "Table 1: [ ";
    for (size_t i = 0; i < numBuckets_; ++i) {
        if (valid1_[i]){
            out << table1_[i] << ", ";
        } else {
            out << " ,";
        }
    }
    out << "]\nTable 2: [ ";

    for (size_t i = 0; i < numBuckets_; ++i)
    {
       if (valid2_[i]){
            out << table2_[i] << ", ";
        } else {
            out << " ,";
        }
    }
    out << "]\n Epsilon: " << epsilon_ << " Num Buckets: " << numBuckets_ << " Size: " << size_ << " Max Loops: " << maxLoop_;
}

template <typename T>
typename CuckooHashSet<T>::const_iterator CuckooHashSet<T>::begin() const {
    return const_iterator(0, table1_, table2_, valid1_, valid2_);
}

template <typename T>
typename CuckooHashSet<T>::const_iterator CuckooHashSet<T>::end() const {
    return const_iterator(2 * numBuckets_, table1_, table2_, valid1_, valid2_);
}

template <typename T>
CuckooHashSet<T>::const_iterator::const_iterator(size_t idx, T* t1, T* t2, const vector<bool>& valid1, const vector<bool>& valid2):
    v1_{valid1}, v2_{valid2},idx_{idx},t1_{t1}, t2_{t2}{
    iterateTable();
}

template <typename T>
typename CuckooHashSet<T>::const_iterator& CuckooHashSet<T>::const_iterator::operator++() {
    ++idx_;
    iterateTable();
    return *this;
}

template <typename T>
void CuckooHashSet<T>::const_iterator::iterateTable(){
    // First Table
    size_t tableSize = v1_.size(); // Table size is the size of the valid vec
    while (idx_ < tableSize)
    {
        if (v1_[idx_]){
            return;
        } else {
            ++idx_;
        }
    }
    // Second Table
    while (idx_ < 2 * tableSize){
        if (v2_[idx_-tableSize]){
            return;
        } else {
            ++idx_;
        }
    }
}

template <typename T>
typename CuckooHashSet<T>::const_iterator::value_type CuckooHashSet<T>::const_iterator::operator*() const{
    size_t tableSize = v2_.size();
    if (idx_ < tableSize)
    {
        return t1_[idx_];
    } else {
        return t2_[idx_ - tableSize];
    }
}

template <typename T>
bool CuckooHashSet<T>::const_iterator::operator==(const const_iterator& other) const {
    return (idx_ == other.idx_) and (t1_ == other.t1_) and (t2_ == other.t2_);
}

template <typename T>
bool CuckooHashSet<T>::const_iterator::operator!=(const const_iterator& other) const{
    return !(*this == other);
}

template <typename T>
typename CuckooHashSet<T>::const_iterator::pointer CuckooHashSet<T>::const_iterator::operator->() const{
    return &(**this);
}

template <typename T>
ostream& operator<<(ostream& os, const CuckooHashSet<T>& cs){
    cs.printToStream(os);
    return os;
}
