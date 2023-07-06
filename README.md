# cuckoo-hash
Implementation of Cuckoo-Hash table.

## Roadmap: 

Fix rehashing bug in Remove(). No bug, just extrememly rare case where all 3 keys map to the EXACT same set of buckets with each hash function mod 32. Unintentional Hash Flood. 

Need to have way in insert when being called from resize to detect maxLoops and increase the table size by 1. 

Implement Cuckoo-HashSet functionality (keys with no values)
