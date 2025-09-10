#ifndef KVICK_HPP
#define KVICK_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <stdexcept>

#include <thread>
#include <mutex>
#include <atomic>

#include "JSONSerializer.hpp"  

class KVick {
public:
    using Primitive = std::variant<std::string, int, bool, double>;
    using ValueType = std::variant<
        std::string, 
        int, 
        bool, 
        double,
        std::vector<std::string>,
        std::vector<int>,
        std::vector<bool>,
        std::vector<double>
    >;

private:
    std::unordered_map<std::string, ValueType> store;
    std::string persist_filename;
    std::thread persist_thread;
        
    mutable std::mutex store_mutex;  
    
public:
    template<typename T>
    void set(const std::string& key, const T& value);

    ValueType get(const std::string& key) const;
    bool exists(const std::string& key) const;
    bool del(const std::string& key);
    std::string getType(const std::string& key) const;
    
    template<typename T>
    T getAs(const std::string& key) const;
    
    void print(const std::string& key) const;
    void printAll() const;
    size_t size() const;
    void clear();
    std::vector<std::string> keys() const;

public:
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    void enableAutoPersist(const std::string& filename, int intervalSeconds = 30);
    void disableAutoPersist();

private:
    std::string serializeStore() const;
    bool deserializeStore(const std::string& data);
    std::atomic<bool> auto_persist_enabled{false};

private:
    void printValue(const ValueType& value) const;
};

template<typename T>
T KVick::getAs(const std::string& key) const {
    std::lock_guard<std::mutex> lock(store_mutex);
    auto it = store.find(key);
    if (it == store.end()) {
        throw std::runtime_error("Key not found: " + key);
    }
    return std::get<T>(it->second);
}

template<typename T>
void KVick::set(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lock(store_mutex);
    store[key] = value;
}


#endif
