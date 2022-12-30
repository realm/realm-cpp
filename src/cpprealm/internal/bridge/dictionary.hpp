#ifndef CPP_REALM_BRIDGE_DICTIONARY_HPP
#define CPP_REALM_BRIDGE_DICTIONARY_HPP

#include <string>

namespace realm::object_store {
    class Dictionary;
}

namespace realm::internal::bridge {
    using Dictionary = object_store::Dictionary;

    struct mixed;
    struct binary;
    struct timestamp;
    struct obj_key;
    struct uuid;
    struct notification_token;
    struct collection_change_callback;
    struct obj;

    struct dictionary {
        dictionary();
        dictionary(const Dictionary& v); //NOLINT(google-explicit-constructor)
        operator Dictionary() const; //NOLINT(google-explicit-constructor)
        void insert(const std::string& key, const mixed& value);
        void insert(const std::string &key, const std::string &value);
        [[nodiscard]] size_t size() const;
        void remove_all();
        void remove(const std::string&);
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
        obj get(const std::string&);
        template <>
        obj_key get(const std::string&);
        template <>
        int64_t get(const std::string&);
        obj insert_embedded(const std::string&);
        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>&& cb);
    private:
        unsigned char m_dictionary[80]{};
    };
}

#endif //CPP_REALM_BRIDGE_DICTIONARY_HPP
