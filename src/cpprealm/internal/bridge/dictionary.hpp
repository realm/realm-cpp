#ifndef CPP_REALM_BRIDGE_DICTIONARY_HPP
#define CPP_REALM_BRIDGE_DICTIONARY_HPP

#include <string>

namespace realm {
    class Dictionary;
}

namespace realm::internal::bridge {
    struct mixed;
    struct binary;
    struct timestamp;
    struct obj_key;
    struct uuid;

    struct dictionary {
        dictionary();
        dictionary(const Dictionary& v);
        operator Dictionary() const;
        void insert(const std::string& key, const mixed& value);
        [[nodiscard]] size_t size() const;
        void remove_all();
        std::pair<std::string, mixed> get_pair(size_t);
        void clear();
        template <typename T>
        T get(const std::string&);
        template <>
        std::string get(const std::string&);
        template <>
        uuid get(const std::string&);
        template <>
        timestamp get(const std::string&);
        template <>
        binary get(const std::string&);
        template <>
        obj_key get(const std::string&);
        template <>
        int64_t get(const std::string&);
    private:
        unsigned char m_dictionary[128]{};
    };
}

#endif //CPP_REALM_BRIDGE_DICTIONARY_HPP
