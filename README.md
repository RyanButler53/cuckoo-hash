# cuckoo-hash
This is a C++ header only template implementation of Cuckoo-Hash Map and Set with an iterator. 

## Interface for CuckooHashMap: 

### Constructor:

There are 2 constructors for the Cuckoo HashMap:

`CuckooHashMap():` Default constructor, sets $\epsilon$ to 0.4 and `downsizeThresh` to 0.2. These were values taken from the original paper on cuckoo hashing by Rasmus Pugh and Flemming Friche Rodler.

`CuckooHashMap(epsilon, downsizeThresh):` Allows the user to set the epsilon value and downsizing threshold. 

### Member Functions:

`bool contains(key):` Checks if the key is in the table

`void insert(key, value):` Insert an item into the hash table

`type lookup(key):` Finds the value associated with `key`. 

`void erase(key):` Removes a key-value pair from the hash table

`type operator[]:` looks up a value in the table. If the value already exists, supports reassignment, but not insertion. 

`size_t size():` Returns the number of elements in the map

`bool empty():` Checks if the map is empty or not. 

`double loadFactor():` Returns the load factor of the hash map

`void clear():` Clears the hashmap


## Interface for CuckooHashSet:

### Constructor:
There are 2 constructors for the Cuckoo HashSet, same as the Cuckoo HashMap

`CuckooHashSet():` Default constructor, sets $\epsilon$ to 0.4 and `downsizeThresh` to 0.2. 

`CuckooHashSet(epsilon, downsizeThresh)` allows the user to specify epsilon and downsize threshold. 

### Member Functions: 

`bool contains(key):` Checks if the key is in the set

`void insert(key):` Insert an item into the hash set

`void erase(key):` Removes a key from the set. Possibly downsizes the table

`size_t size():` Returns the number of elements in the set

`bool empty():` Checks if the set is empty or not. 

`double loadFactor():` Returns the load factor of the hash map

`void clear:` Clears the hash map. 

## Other Notes

- The iterator uses `begin()` and `end()` and works with the notation `for (auto& x : map)` to iterate over the entire map. 
- Iterator is invalidated when inserting, erasing or clearing. 
- Keys (and Values) must implement a copy constructor for insertion. 
- Both the hash set and hashmap allow for printing with the `<<` streaming operator. 
- The file `cuckoo-test.cpp` is a completely non-comprehensive test suite for both the CuckooHashSet and CuckooHashMap. 

## Asymtotic Runtimes (n items in table)

`contains(key)`: $\Theta(1)$ worst case. 

`lookup(key)`: $\Theta(1)$ worst case

`insert(key, value)`: $\Theta(1)$ expected amortized case, $O(n)$ worst case. 

`erase(key)`: $\Theta(1)$ expected case, $O(n)$ worst case. 

`empty(), size(), loadFactor():` $\Theta(1)$ worst case.

Insert and remove sometimes will resize the table and rehash all keys. Insertion triggers a rehash when $3 log_{1+\epsilon}(n)$ loops are reached when trying to find a place for a new key. Epsilon is set as a parameter in the second constructor, default value is 0.4. The downsize threshold is the minimum load factor to be reached before the table is downsized and all keys are rehashed. Defaults to 0.2 




