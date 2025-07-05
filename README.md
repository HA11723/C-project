# C-project


Stores up to N key–value pairs.

Whenever you get(key), it returns the value (if present) and marks that entry as “most recently used.”

When you put(key,value) and the cache is already full, it automatically evicts the least recently accessed entry to make room.


Under the hood it combines two fundamental structures:

A doubly-linked list (std::list) to track usage order (so we can quickly move items to the front or remove the back).

A hash table (std::unordered_map) to map keys to list nodes in O(1) time.
