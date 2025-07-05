// LRUCache.h
#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <unordered_map>
#include <list>
#include <mutex>
#include <optional>

/**
 * @brief Thread-safe, templated Least Recently Used (LRU) Cache.
 * Stores up to `capacity` key-value pairs, evicting the least recently used item on overflow.
 *
 * @tparam Key      Type of the cache key (must be Hashable).
 * @tparam Value    Type of the cache value.
 * @tparam Hash     Hash functor for Key.
 * @tparam KeyEqual Equality comparator for Key.
 */
template <
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>
>
class LRUCache {
public:
    /**
     * @brief Construct an LRUCache with given capacity.
     * @param capacity Maximum number of entries to hold.
     */
    explicit LRUCache(size_t capacity)
        : capacity_(capacity) {}

    /**
     * @brief Retrieve value for key, marking it as most-recently used.
     * @param key Key to look up.
     * @return optional<Value>: engaged if found, disengaged otherwise.
     */
    std::optional<Value> get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = items_map_.find(key);
        if (it == items_map_.end()) {
            return std::nullopt;
        }
        // Move accessed item to front of list
        items_list_.splice(items_list_.begin(), items_list_, it->second);
        return it->second->second;
    }

    /**
     * @brief Insert or update key-value pair.
     * If key exists, updates value and moves to front.
     * If new and capacity reached, evicts LRU item.
     * @param key   Key to insert.
     * @param value Value to associate.
     */
    void put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = items_map_.find(key);
        if (it != items_map_.end()) {
            // Update existing
            it->second->second = value;
            items_list_.splice(items_list_.begin(), items_list_, it->second);
            return;
        }
        // Evict if full
        if (items_list_.size() >= capacity_) {
            auto lru = items_list_.back();
            items_map_.erase(lru.first);
            items_list_.pop_back();
        }
        // Insert new at front
        items_list_.emplace_front(key, value);
        items_map_[key] = items_list_.begin();
    }

    /**
     * @brief Check if key exists in cache.
     * @param key Key to check.
     * @return true if present, false otherwise.
     */
    bool exists(const Key& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return items_map_.find(key) != items_map_.end();
    }

    /**
     * @brief Get current size of the cache.
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return items_list_.size();
    }

private:
    size_t capacity_;
    mutable std::mutex mutex_;  // Protects items_list_ and items_map_

    // List of (key, value) pairs; front = most recently used
    std::list<std::pair<Key, Value>> items_list_;

    // Map from key to list iterator for O(1) access
    std::unordered_map<Key,
                       typename std::list<std::pair<Key, Value>>::iterator,
                       Hash,
                       KeyEqual> items_map_;
};

#endif // LRU_CACHE_H

// Example usage:
#ifdef LRU_CACHE_EXAMPLE
#include <iostream>
#include <string>
int main() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    // Access 2, now order: 2,3,1
    if (auto val = cache.get(2)) std::cout << *val << '\n';
    cache.put(4, "four"); // evicts key=1
    std::cout << "Exists(1)? " << cache.exists(1) << '\n'; // 0
    std::cout << "Exists(3)? " << cache.exists(3) << '\n'; // 1
    return 0;
}
#endif
