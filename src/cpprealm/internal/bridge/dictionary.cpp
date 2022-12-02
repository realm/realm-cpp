#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/dictionary.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<128, sizeof(Dictionary)>{});

    size_t dictionary::size() const {
        return reinterpret_cast<const Dictionary*>(m_dictionary)->size();
    }

    void dictionary::insert(const std::string &key, const realm::internal::bridge::mixed &value) {
        reinterpret_cast<Dictionary*>(m_dictionary)->insert(key, static_cast<realm::Mixed>(value));
    }

    void dictionary::clear() {
        reinterpret_cast<Dictionary*>(m_dictionary)->clear();
    }

    void dictionary::remove_all() {
        clear();
    }

    std::pair<std::string, mixed> dictionary::get_pair(size_t idx) {
        auto pair = reinterpret_cast<Dictionary*>(m_dictionary)->get_pair(idx);
        return { pair.first.get<StringData>(), static_cast<mixed>(pair.second) };
    }

    template <>
    std::string dictionary::get(const std::string &key) {
        return reinterpret_cast<Dictionary*>(m_dictionary)->get(key).get<StringData>();
    }
    template <>
    uuid dictionary::get(const std::string &key) {
        return reinterpret_cast<Dictionary*>(m_dictionary)->get(key).get<UUID>();
    }
    template <>
    timestamp dictionary::get(const std::string &key) {
        return reinterpret_cast<Dictionary*>(m_dictionary)->get(key).get<Timestamp>();
    }
    template <>
    binary dictionary::get(const std::string &key) {
        return reinterpret_cast<Dictionary*>(m_dictionary)->get(key).get<BinaryData>();
    }
    template <>
    int64_t dictionary::get(const std::string &key) {
        return reinterpret_cast<Dictionary*>(m_dictionary)->get(key).get<Int>();
    }
}
