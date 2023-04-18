#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>

#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/results.hpp>

namespace realm::internal::bridge {
    dictionary::dictionary() {
        m_dictionary = std::make_shared<Dictionary>(Dictionary());
    }

    dictionary::dictionary(const Dictionary &v) {
        m_dictionary = std::make_shared<Dictionary>(v);
    }

    dictionary::operator Dictionary() const {
        return *m_dictionary;
    }
    size_t dictionary::size() const {
        return m_dictionary->size();
    }

    void dictionary::insert(const std::string &key, const mixed &value) {
        m_dictionary->insert_any(key, static_cast<Mixed>(value));
    }

    void dictionary::insert(const std::string &key, const std::string &value) {
        m_dictionary->insert(key, StringData(value));
    }
    void dictionary::clear() {
        m_dictionary->remove_all();
    }

    size_t dictionary::find(const std::string& key) {
        return m_dictionary->find_any(key);
    }

    void dictionary::remove_all() {
        clear();
    }

    void dictionary::remove(const std::string& key) {
        m_dictionary->try_erase(key);
    }

    std::pair<std::string, mixed> dictionary::get_pair(size_t idx) {
        auto pair = m_dictionary->get_pair(idx);
        return { pair.first, static_cast<mixed>(pair.second) };
    }

    size_t dictionary::get_key_index(const std::string& key) {
        Results keys = m_dictionary->get_keys();
        return keys.index_of(StringData(key));
    }

    template <>
    std::string get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<StringData>(key);
    }
    template <>
    uuid get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<UUID>(key);
    }
    template <>
    object_id get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<ObjectId>(key);
    }
    template <>
    timestamp get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<Timestamp>(key);
    }
    template <>
    binary get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<BinaryData>(key);
    }
    template <>
    int64_t get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<Int>(key);
    }
    template <>
    double get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<Double>(key);
    }
    template <>
    obj_key get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<ObjKey>(key);
    }
    template <>
    obj get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get_object(key);
    }
    template <>
    mixed get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get_any(key);
    }
    template <>
    bool get(dictionary& dict, const std::string &key) {
        return dict.m_dictionary->get<Bool>(key);
    }

    notification_token dictionary::add_notification_callback(std::shared_ptr<collection_change_callback>&& cb) {
        struct wrapper : CollectionChangeCallback {
            std::shared_ptr<collection_change_callback> m_cb;
            explicit wrapper(std::shared_ptr<collection_change_callback>&& cb)
                    : m_cb(std::move(cb)) {}
            void before(const CollectionChangeSet& v) const {
                m_cb->before(v);
            }
            void after(const CollectionChangeSet& v) const {
                m_cb->after(v);
            }
        } ccb(std::move(cb));
        return m_dictionary->add_notification_callback(ccb);
    }

    obj dictionary::insert_embedded(const std::string &v) {
        return m_dictionary->insert_embedded(v);
    }
}
