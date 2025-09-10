#include "KVick.hpp"
#include <fstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

KVick::ValueType KVick::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(store_mutex);
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


bool KVick::saveToFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(store_mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "{\n";
    bool first = true;
    for (const auto& [key, value] : store) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << key << "\": ";
        
        std::visit([&file](const auto& v) {
            file << JSONSerializer::serialize(v);
        }, value);
    }
    file << "\n}\n";
    
    file.close();
    std::cout << "Data saved to " << filename << " (" << store.size() << " entries)" << std::endl;
    return true;
}

bool KVick::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "No existing data file found: " << filename << std::endl;
        return false; 
    }
    
    std::lock_guard<std::mutex> lock(store_mutex);
    
    std::string line;
    size_t loaded_count = 0;
    
    while (std::getline(file, line)) {
        if (line.find("\"") == std::string::npos) continue;
        
        size_t colon_pos = line.find("\":");
        if (colon_pos == std::string::npos) continue;
        
        size_t key_start = line.find("\"") + 1;
        size_t key_end = line.find("\"", key_start);
        if (key_end == std::string::npos) continue;
        
        std::string key = line.substr(key_start, key_end - key_start);
        
        size_t value_start = colon_pos + 2;
        while (value_start < line.length() && line[value_start] == ' ') value_start++;
        
        std::string value_str = line.substr(value_start);
        while (!value_str.empty() && (value_str.back() == ',' || value_str.back() == ' ')) {
            value_str.pop_back();
        }
        
        if (value_str.front() == '"' && value_str.back() == '"') {
            std::string str_val = value_str.substr(1, value_str.length() - 2);
            store[key] = str_val;
            loaded_count++;
        } else if (value_str == "true" || value_str == "false") {
            store[key] = (value_str == "true");
            loaded_count++;
        } else if (value_str.find('.') != std::string::npos) {
            store[key] = std::stod(value_str);
            loaded_count++;
        } else if (std::isdigit(value_str.front()) || value_str.front() == '-') {
            store[key] = std::stoi(value_str);
            loaded_count++;
        }
    }
    
    file.close();
    std::cout << "Data loaded from " << filename << " (" << loaded_count << " entries)" << std::endl;
    return true;
}

void KVick::enableAutoPersist(const std::string& filename, int intervalSeconds) {
    persist_filename = filename;
    auto_persist_enabled = true;
    
    persist_thread = std::thread([this, intervalSeconds]() {
        while (auto_persist_enabled) {
            std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
            if (auto_persist_enabled) {
                saveToFile(persist_filename);
            }
        }
    });
}

void KVick::disableAutoPersist() {
    auto_persist_enabled = false;
    if (persist_thread.joinable()) {
        persist_thread.join();
    }
}