#include <iostream>
#include <string>
#include "cuckoo-hash.hpp"


using namespace std;


int main()
{
    CuckooHash<string, int> ch = CuckooHash<string, int>(0.3, 0.2);
    string keys[30] = {"a", "z", "c", "d", "e", "g", "s", "f", "h", "k", "j", "i", "b", "l", "t", "p", "n", "o", "r", "q", "ab", "ac", "ad", "ae", "aq", "aa", "ag", "ah", "ai", "aj"};
    int values[30] = {1, 4, 6, 5, 9, 12, 22, 43, 47, 41, 50, 40, 20, 8, 7, 15, 13, 19, 21, 90, 104, 102, 103, 201, 105, 203, 254, 291, 221, 210};
    for (size_t i = 0; i < 30; ++i){
        cout << "Inserting key: " << keys[i] << " value " << values[i] << endl;
        ch.insert(keys[i], values[i]);
    }
    cout << ch << endl;
    for (size_t i = 0; i < 15; ++i)
    {
        cout << "Removing key: " << keys[i] << " value " << values[i] << " " << ch[keys[i]]<< endl;
        ch.remove(keys[i]);
    }
    cout << ch << endl;

    ch.insert("string", 155);
    cout << ch << endl;
    ch["string"] = 144;

    cout << ch << endl;
}