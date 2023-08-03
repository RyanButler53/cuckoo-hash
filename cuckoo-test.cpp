#include <iostream>
#include <string>
#include "cuckoo-hash.hpp"


using namespace std;


int main()
{

    CuckooHashMap<string, int> ch = CuckooHashMap<string, int>(0.3, 0.2);
    string keys[30] = {"a", "z", "c", "d", "e", "g", "s", "f", "h", "k", "j", "i", "b", "l", "t", "p", "n", "o", "r", "q", "ab", "ac", "ad", "ae", "aq", "aa", "ag", "ah", "ai", "aj"};
    int values[30] = {1, 4, 6, 5, 9, 12, 22, 43, 47, 41, 50, 40, 20, 8, 7, 15, 13, 19, 21, 90, 104, 102, 103, 201, 105, 203, 254, 291, 221, 210};
    for (size_t i = 0; i < 30; ++i){
        ch.insert(keys[i], values[i]);
    }
    cout << ch << endl;
    for (size_t i = 0; i < 20; ++i)
    {
        assert(values[i] == ch[keys[i]]);
        ch.erase(keys[i]);
    }

    ch.insert("string", 155);
    
    // Reassignment, not insertion. 
    ch["string"] = 144;
    assert(ch["string"] == 144);

    // Test Iterator:
    for (auto [k,v] : ch){
        cout << k << " : " << v << " ";
    }
    cout << endl;

    // CUCKOO SET
    CuckooHashSet<string> cs = CuckooHashSet<string>(0.3, 0.2);
    for (size_t i = 0; i < 30; ++i)
    {
        cs.insert(keys[i]);
    }
    cout << cs << endl;
    for (size_t i = 0; i < 25; ++i) 
    {
        cs.erase(keys[i]);
    }
    cout << cs << endl;
    cs.clear();
    cout << cs << endl;
    for (size_t i = 10; i < 15; ++i){
        cs.insert(keys[i]);
    }
    cout << cs << endl;

    for (string s : cs){
        cout << s << " ";
    }
    cout << endl;
}