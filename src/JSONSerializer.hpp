#pragma once
#include <string>
#include <sstream>
#include <variant>
#include <vector>

class JSONSerializer {
public:
    template<typename T>
    static std::string serialize(const T& value) {
        using Type = std::decay_t<T>;
        if constexpr (std::is_same_v<Type, std::string>) {
            return "\"" + value + "\"";
        } else if constexpr (std::is_same_v<Type, int>) {
            return std::to_string(value);
        } else if constexpr (std::is_same_v<Type, double>) {
            return std::to_string(value);
        } else if constexpr (std::is_same_v<Type, bool>) {
            return value ? "true" : "false";
        } else if constexpr (std::is_same_v<Type, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < value.size(); ++i) {
                result += "\"" + value[i] + "\"";
                if (i < value.size() - 1) result += ",";
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<Type, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < value.size(); ++i) {
                result += std::to_string(value[i]);
                if (i < value.size() - 1) result += ",";
            }
            result += "]";
            return result;
        }
        return "null";
    }
};
