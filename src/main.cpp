// main.cpp
#include "KVick.hpp"

int main() {
    KVick kv;
    
    // Set different types of values
    kv.set("name", std::string("Alice"));
    kv.set("age", 25);
    kv.set("height", 5.8);
    kv.set("active", true);
    kv.set("scores", std::vector<int>{95, 87, 92, 88});
    kv.set("tags", std::vector<std::string>{"developer", "cpp", "systems"});
    kv.set("flags", std::vector<bool>{true, false, true});
    kv.set("prices", std::vector<double>{19.99, 29.99, 39.99});
    
    std::cout << "=== All Values ===" << std::endl;
    kv.printAll();
    
    std::cout << "\n=== Type Information ===" << std::endl;
    for (const auto& key : kv.keys()) {
        std::cout << key << " is of type: " << kv.getType(key) << std::endl;
    }
    
    std::cout << "\n=== Type-safe Access ===" << std::endl;
    try {
        std::cout << "Name: " << kv.getAs<std::string>("name") << std::endl;
        std::cout << "Age: " << kv.getAs<int>("age") << std::endl;
        std::cout << "First score: " << kv.getAs<std::vector<int>>("scores")[0] << std::endl;
        
        // This will throw an exception due to type mismatch (uncomment to test)
        // std::cout << kv.getAs<int>("name") << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    std::cout << "\n=== Store Statistics ===" << std::endl;
    std::cout << "Store size: " << kv.size() << std::endl;
    
    return 0;
}
