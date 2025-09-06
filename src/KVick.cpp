#include "KVick.hpp"

KVick::ValueType KVick::get(const std::string& key) const {
    auto it = store.find(key);
    if (it != store.end()) {
        return it->second;
    }
    throw std::runtime_error("Key not found: " + key);
}

bool KVick::exists(const std::string& key) const {
    return store.find(key) != store.end();
}

bool KVick::del(const std::string& key) {
    return store.erase(key) > 0;
}

std::string KVick::getType(const std::string& key) const {
    auto it = store.find(key);
    if (it == store.end()) {
        throw std::runtime_error("Key not found: " + key);
    }
    
    return std::visit([](const auto& value) -> std::string {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<bool>>) return "vector<bool>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else return "unknown";
    }, it->second);
}

void KVick::print(const std::string& key) const {
    auto it = store.find(key);
    if (it == store.end()) {
        std::cout << "Key '" << key << "' not found" << std::endl;
        return;
    }
    
    std::cout << key << ": ";
    printValue(it->second);
    std::cout << std::endl;
}

void KVick::printAll() const {
    if (store.empty()) {
        std::cout << "Store is empty" << std::endl;
        return;
    }
    
    for (const auto& [key, value] : store) {
        std::cout << key << ": ";
        printValue(value);
        std::cout << std::endl;
    }
}

size_t KVick::size() const {
    return store.size();
}

void KVick::clear() {
    store.clear();
}

std::vector<std::string> KVick::keys() const {
    std::vector<std::string> result;
    result.reserve(store.size());
    for (const auto& [key, value] : store) {
        result.push_back(key);
    }
    return result;
}

void KVick::printValue(const ValueType& value) const {
    std::visit([](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        
        if constexpr (std::is_same_v<T, bool>) {
            std::cout << (v ? "true" : "false");
        }
        else if constexpr (std::is_same_v<T, std::string> || 
                          std::is_same_v<T, int> || 
                          std::is_same_v<T, double>) {
            std::cout << v;
        }
        else if constexpr (std::is_same_v<T, std::vector<std::string>> ||
                          std::is_same_v<T, std::vector<int>> ||
                          std::is_same_v<T, std::vector<bool>> ||
                          std::is_same_v<T, std::vector<double>>) {
            std::cout << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    std::cout << (v[i] ? "true" : "false");
                } else {
                    std::cout << v[i];
                }
                if (i + 1 < v.size()) std::cout << ", ";
            }
            std::cout << "]";
        }
    }, value);
}
