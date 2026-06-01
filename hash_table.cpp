#include "HashTable.h"
#include <functional>

HashTable::HashTable(size_t size) noexcept
    : _capacity(static_cast<int32_t>(size)), _filled(0), table(size)
{}

HashTable::~HashTable()
{
    table.clear();
}
size_t HashTable::hash_function(const KeyType &key) const 
{
    size_t hash = 0;
    
    for (char ch : key) {
        hash = hash * 131 + static_cast<size_t>(ch);
    }
    
    return hash % _capacity;
}

void HashTable::insert(const KeyType &key, const ValueType &value)
{
    if (getLoadFactor() > 0.75) {
        int32_t new_capacity = _capacity * 2;
        std::vector<std::list<std::pair<KeyType, ValueType>>> new_table(new_capacity);
        
        for (int32_t i = 0; i < _capacity; ++i) {
            for (auto& pair : table[i]) {
                int32_t old_capacity = _capacity;
                _capacity = new_capacity;
                size_t new_index = hash_function(pair.first);
                _capacity = old_capacity;
                
                new_table[new_index].push_back(std::move(pair));
            }
        }
        
        _capacity = new_capacity;
        table = std::move(new_table);
    }
    
    size_t index = hash_function(key);
    
    for (auto& pair : table[index]) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }
    
    table[index].push_back({key, value});
    _filled++;
}

bool HashTable::find(const KeyType &key, ValueType &value) const
{
    size_t index = hash_function(key);
    
    for (const auto& pair : table[index]) {
        if (pair.first == key) {
            value = pair.second;
            return true;
        }
    }
    
    return false;
}

void HashTable::remove(const KeyType &key)
{
    size_t index = hash_function(key);
    auto& chain = table[index];
    
    for (auto it = chain.begin(); it != chain.end(); ++it) {
        if (it->first == key) {
            chain.erase(it);
            _filled--;
            return;
        }
    }
}

ValueType& HashTable::operator[](const KeyType &key)
{
    size_t index = hash_function(key);
    
    for (auto& pair : table[index]) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    
    if (getLoadFactor() > 0.75) {
        int32_t new_capacity = _capacity * 2;
        std::vector<std::list<std::pair<KeyType, ValueType>>> new_table(new_capacity);
        
        for (int32_t i = 0; i < _capacity; ++i) {
            for (auto& pair : table[i]) {
                int32_t old_capacity = _capacity;
                _capacity = new_capacity;
                size_t new_index = hash_function(pair.first);
                _capacity = old_capacity;
                
                new_table[new_index].push_back(std::move(pair));
            }
        }
        
        _capacity = new_capacity;
        table = std::move(new_table);
        
        index = hash_function(key);
    }
    
    table[index].push_back({key, ValueType{}});
    _filled++;
    
    return table[index].back().second;
}

double HashTable::getLoadFactor()
{
    return static_cast<double>(_filled) / _capacity;
}