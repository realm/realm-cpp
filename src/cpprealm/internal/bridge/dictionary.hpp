#ifndef CPP_REALM_BRIDGE_DICTIONARY_HPP
#define CPP_REALM_BRIDGE_DICTIONARY_HPP

#include <memory>
#include <string>

namespace realm::object_store {
    class Dictionary;
}

namespace realm {
    class Dictionary;
    using CoreDictionary = Dictionary;
}

namespace realm::internal::bridge {
    using Dictionary = object_store::Dictionary;
    using CoreDictionary = CoreDictionary;

    struct mixed;
    struct binary;
    struct timestamp;
    struct obj_key;
    struct uuid;
    struct object_id;
    struct notification_token;
    struct collection_change_callback;
    struct obj;

    struct core_dictionary {
        core_dictionary();
        core_dictionary(const core_dictionary &other);
        core_dictionary &operator=(const core_dictionary &other);
        core_dictionary(core_dictionary &&other);
        core_dictionary &operator=(core_dictionary &&other);
        ~core_dictionary();

        core_dictionary(const CoreDictionary& v); //NOLINT(google-explicit-constructor)
        operator CoreDictionary () const; //NOLINT(google-explicit-constructor)
        void insert(const std::string& key, const mixed& value);
        void insert(const std::string& key, const std::string& value);
        obj create_and_insert_linked_object(const std::string& key);
        obj create_and_insert_linked_object(const std::string& key, const internal::bridge::mixed& pk);
        mixed get(const std::string& key) const;
        obj get_object(const std::string& key);
        std::pair<mixed, mixed> get_pair(size_t ndx) const;
        size_t find_any_key(const std::string& value) const noexcept;

        size_t size() const;
    private:
#ifdef __i386__
        std::aligned_storage<144, 4>::type m_dictionary[1];
#elif __x86_64__
        std::aligned_storage<144, 8>::type m_dictionary[1];
#elif __arm__
        std::aligned_storage<144, 4>::type m_dictionary[1];
#elif __aarch64__
        std::aligned_storage<144, 8>::type m_dictionary[1];
#elif _WIN32
        std::aligned_storage<144, 8>::type m_dictionary[1];
#endif
    };


    struct dictionary {
        dictionary();
        dictionary(const dictionary& other) ;
        dictionary& operator=(const dictionary& other) ;
        dictionary(dictionary&& other);
        dictionary& operator=(dictionary&& other);
        ~dictionary();
        dictionary(const Dictionary& v); //NOLINT(google-explicit-constructor)
        operator Dictionary() const; //NOLINT(google-explicit-constructor)
        void insert(const std::string& key, const mixed& value);
        void insert(const std::string &key, const std::string &value);
        [[nodiscard]] size_t size() const;
        void remove_all();
        void remove(const std::string&);
        std::pair<std::string, mixed> get_pair(size_t);
        [[nodiscard]] size_t get_key_index(const std::string&);
        void clear();
        [[nodiscard]] size_t find(const std::string&);
        obj insert_embedded(const std::string&);
        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>&& cb);
    private:
        template <typename T>
        friend T get(dictionary&, const std::string&);
#ifdef __i386__
        std::aligned_storage<40, 4>::type m_dictionary[1];
#elif __x86_64__
        std::aligned_storage<80, 8>::type m_dictionary[1];
#elif __arm__
        std::aligned_storage<40, 4>::type m_dictionary[1];
#elif __aarch64__
        std::aligned_storage<80, 8>::type m_dictionary[1];
#elif _WIN32
        std::aligned_storage<1, 1>::type m_dictionary[1];
#endif
    };

    template <typename T>
    [[nodiscard]] T get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] std::string get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] uuid get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] object_id get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] timestamp get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] binary get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] obj get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] obj_key get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] int64_t get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] double get(dictionary&, const std::string&);
}

#endif //CPP_REALM_BRIDGE_DICTIONARY_HPP
