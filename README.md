# cuckoo-hash
This is a header only implementation of Cuckoo-Hash table. 

## Interface: 

### Constructor:

There are 2 constructors for the cuckoo hash table:

`CuckooHash():` Default constructor, sets $\epsilon$ to 0.4 and `downsizeThresh` to 0.2. These were values taken from the original paper on cuckoo hashing by Rasmus Pugh and Flemming Friche Rodler.
 
`CuckooHash(epsilon, downsizeThresh):` Allows the user to set the epsilon value and downsizing threshold. 

### Member Functions:

`bool exists(key):` Checks if the key is in the table

`void insert(key, value):` Insert an item into the hash table

`type lookup(key):` Finds the value associated with `key`. 

`void remove(key):` Removes a key-value pair from the hash table

`type operator[]:` looks up a value in the table. If the value already exists, supports reassignment. 

## Asymtotic Runtimes (n items in table)

`exists(key)`: $\Theta(1)$ worst case. 

`lookup(key)`: $\Theta(1)$ worst case

`insert(key, value)`: $\Theta(1)$ expected amortized case, $O(n)$ worst case. 

`remove(key)`: $\Theta(1)$ expected case, $O(n)$ worst case. 

Insert and remove sometimes will resize the table and rehash all keys. Insertion triggers a rehash when $3 log_{1+\epsilon}(n)$ loops are reached when trying to find a place for a new key. Epsilon is set as a parameter in the second constructor, default value is 0.4. The downsize threshold is the minimum load factor to be reached before the table is downsized and all keys are rehashed. Defaults to 0.2 


