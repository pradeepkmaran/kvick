#ifndef KVICK_HPP
#define KVICK_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <stdexcept>

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

public:
    template<typename T>
    void set(const std::string& key, const T& value) {
        store[key] = value;
    }
    
    ValueType get(const std::string& key) const;
    
    bool exists(const std::string& key) const;
    
    bool del(const std::string& key);
    
    std::string getType(const std::string& key) const;
    
    template<typename T>
    T getAs(const std::string& key) const {
        auto it = store.find(key);
        if (it == store.end()) {
            throw std::runtime_error("Key not found: " + key);
        }
        
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access& e) {
            throw std::runtime_error("Type mismatch for key: " + key + ". Expected different type than stored.");
        }
    }
    
    void print(const std::string& key) const;
    
    void printAll() const;
    
    size_t size() const;
    
    void clear();
    
    std::vector<std::string> keys() const;

private:
    void printValue(const ValueType& value) const;
};

#endif
