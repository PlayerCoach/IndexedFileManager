#pragma once 
#include <optional>


template <typename T>
    static void serializeOptional(const std::optional<T>& value, char*& ptr) {
        bool exists = value.has_value();
        memcpy(ptr, &exists, sizeof(bool));
        ptr += sizeof(bool);
        if (exists) {
            memcpy(ptr, &value.value(), sizeof(T));
        } else {
            T nullValue = 0;
            memcpy(ptr, &nullValue, sizeof(T));
        }
        ptr += sizeof(T);
    }

    template <typename T>
    static std::optional<T> deserializeOptional(char*& ptr) {
        bool exists;
        memcpy(&exists, ptr, sizeof(bool));
        ptr += sizeof(bool);
        if (exists) {
            T value;
            memcpy(&value, ptr, sizeof(T));
            ptr += sizeof(T);
            return value;
        }
        ptr += sizeof(T);
        return std::nullopt;
    }
