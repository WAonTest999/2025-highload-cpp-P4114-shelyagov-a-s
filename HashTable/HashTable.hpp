#include <cstdint>
#include <vector>
#include <list>

#define INITIAL_SIZE (size_t)256

#define LOAD_FACTOR_THRESHOLD 0.75

template<typename Type>
class HashTable {
private:

    struct Node {
        uint64_t key;
        Type value;
        Node(uint64_t k, const Type& v) : key(k), value(v) {}
    };

    std::vector<std::list<Node>> buckets;
    size_t size;
    
    size_t hash(uint64_t key) const {
        return key % buckets.size();
    }

    void extend() {
        size_t newSize = buckets.size() << 1;
        std::vector<std::list<Node>> newBuckets(newSize);
        
        for (const auto& chain : buckets) {
            for (const auto& node : chain) {
                newBuckets[node.key % newSize].push_back(node);
            }
        }
        buckets = std::move(newBuckets);
    }

public:
    HashTable() : buckets(INITIAL_SIZE), size(0) {}
    
    void insert(uint64_t key, const Type& value) {
        if ((double)size / buckets.size() > LOAD_FACTOR_THRESHOLD) {
            extend();
        }

        size_t i = hash(key);
        for (auto& node : buckets[i]) {
            if (node.key == key) {
                node.value = value;
                return;
            }
        } 
        buckets[i].push_back(Node(key, value));
        size++;
    }
    
    bool find(uint64_t key, Type& value) const {
        size_t i = hash(key);
        
        for (const auto& node : buckets[i]) {
            if (node.key == key) {
                value = node.value;
                return true;
            }
        }
        return false;
    }
    
    bool remove(uint64_t key) {
        size_t i = hash(key);
        
        auto& chain = buckets[i];
        for (std::iterator j = chain.begin(); j != chain.end(); ++j) {
            if (j->key == key) {
                chain.erase(j);
                size--;
                return true;
            }
        }
        return false;
    }
    
    bool contains(uint64_t key) const {
        size_t i = hash(key);
        
        for (const auto& node : buckets[i]) {
            if (node.key == key) {
                return true;
            }
        }
        
        return false;
    }
    
    Type& get(uint64_t key) {
        size_t i = hash(key);
        
        for (const std::list<Node>& node : buckets[i]) {
            if (node.key == key) {
                return node.value;
            }
        }
        
        buckets[i].push_back(Node(key, Type()));
        size++;
        return buckets[i].back().value;
    }
    
    void clear() {
        for (std::list<Node>& chain : buckets) {
            chain.clear();
        }
        size = 0;
    }
    
    size_t getSize() const {
        return size;
    }
    
    bool isEmpty() const {
        return (bool)size;
    }
    
    size_t getBucketsAmmount() const {
        return buckets.size();
    }
};
