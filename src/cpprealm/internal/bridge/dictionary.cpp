#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>

#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/results.hpp>

namespace realm::internal::bridge {

    core_dictionary::core_dictionary() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_dictionary) CoreDictionary();
#else
        m_dictionary = std::make_shared<CoreDictionary>();
#endif
    }
    core_dictionary::core_dictionary(const core_dictionary &other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_dictionary) CoreDictionary(*reinterpret_cast<const CoreDictionary *>(&other.m_dictionary));
#else
        m_dictionary = other.m_dictionary;
#endif
    }
    core_dictionary &core_dictionary::operator=(const core_dictionary &other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<CoreDictionary *>(&m_dictionary) = *reinterpret_cast<const CoreDictionary *>(&other.m_dictionary);
        }
#else
        m_dictionary = other.m_dictionary;
#endif
        return *this;
    }
    core_dictionary::core_dictionary(core_dictionary &&other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_dictionary) CoreDictionary(std::move(*reinterpret_cast<CoreDictionary *>(&other.m_dictionary)));
#else
        m_dictionary = std::move(other.m_dictionary);
#endif
    }
    core_dictionary &core_dictionary::operator=(core_dictionary &&other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<CoreDictionary *>(&m_dictionary) = std::move(*reinterpret_cast<CoreDictionary *>(&other.m_dictionary));
        }
#else
        m_dictionary = std::move(other.m_dictionary);
#endif
        return *this;
    }
    core_dictionary::~core_dictionary() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<CoreDictionary *>(&m_dictionary)->~CoreDictionary();
#endif
    }

    core_dictionary::core_dictionary(const CoreDictionary& v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_dictionary) CoreDictionary(v);
#else
        m_dictionary = std::make_shared<CoreDictionary>(v);
#endif
    }

    inline const CoreDictionary* core_dictionary::get_dictionary_const() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const CoreDictionary*>(&m_dictionary);
#else
        return m_dictionary.get();
#endif
    }

    inline CoreDictionary* core_dictionary::get_dictionary() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<CoreDictionary*>(&m_dictionary);
#else
        return m_dictionary.get();
#endif
    }

    core_dictionary::operator CoreDictionary() const {
        return *get_dictionary_const();
    }

    void core_dictionary::insert(const std::string& key, const mixed& value) {
        get_dictionary()->insert(key, value.operator Mixed());
    }

    void core_dictionary::insert(const std::string& key, const std::string& value) {
        get_dictionary()->insert(key, value);
    }

    obj core_dictionary::create_and_insert_linked_object(const std::string& key, const internal::bridge::mixed& pk) {
        Table& t = *get_dictionary()->get_target_table();
        auto o = t.create_object_with_primary_key(pk.operator Mixed());
        get_dictionary()->insert(key, o.get_key());
        return o;
    }

    obj core_dictionary::create_and_insert_linked_object(const std::string& key) {
        return get_dictionary()->create_and_insert_linked_object(key);
    }

    mixed core_dictionary::get(const std::string& key) const {
        return get_dictionary_const()->get(key);
    }

    void core_dictionary::erase(const std::string& key) {
        get_dictionary()->erase(key);
    }

    obj core_dictionary::get_object(const std::string& key) {
        return get_dictionary()->get_object(key);
    }

    size_t core_dictionary::size() const {
        return get_dictionary_const()->size();
    }

    std::pair<mixed, mixed> core_dictionary::get_pair(size_t ndx) const {
        return get_dictionary_const()->get_pair(ndx);
    }

    size_t core_dictionary::find_any_key(const std::string& value) const noexcept {
        return get_dictionary_const()->find_any_key(value);
    }

    dictionary::dictionary() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_dictionary) Dictionary();
#else
        m_dictionary = std::make_shared<Dictionary>();
#endif

    }
    dictionary::dictionary(const dictionary& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_dictionary) Dictionary(*reinterpret_cast<const Dictionary*>(&other.m_dictionary));
#else
        m_dictionary = other.m_dictionary;
#endif
    }

    dictionary& dictionary::operator=(const dictionary& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Dictionary*>(&m_dictionary) = *reinterpret_cast<const Dictionary*>(&other.m_dictionary);
        }
#else
        m_dictionary = other.m_dictionary;
#endif
        return *this;
    }

    dictionary::dictionary(dictionary&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_dictionary) Dictionary(std::move(*reinterpret_cast<Dictionary*>(&other.m_dictionary)));
#else
        m_dictionary = std::move(other.m_dictionary);
#endif
    }

    dictionary& dictionary::operator=(dictionary&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Dictionary*>(&m_dictionary) = std::move(*reinterpret_cast<Dictionary*>(&other.m_dictionary));
        }
#else
        m_dictionary = std::move(other.m_dictionary);
#endif
        return *this;
    }

    dictionary::~dictionary() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Dictionary*>(&m_dictionary)->~Dictionary();
#endif
    }
    
    dictionary::dictionary(const Dictionary &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_dictionary) Dictionary(v);
#else
        m_dictionary = std::make_shared<Dictionary>(v);
#endif
    }

    const Dictionary* dictionary::get_dictionary_const() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Dictionary*>(&m_dictionary);
#else
        return m_dictionary.get();
#endif
    }
    Dictionary* dictionary::get_dictionary() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Dictionary*>(&m_dictionary);
#else
        return m_dictionary.get();
#endif
    }

    dictionary::operator Dictionary() const {
        return *get_dictionary_const();
    }
    size_t dictionary::size() const {
        return get_dictionary_const()->size();
    }

    void dictionary::insert(const std::string &key, const mixed &value) {
        get_dictionary()->insert_any(key, static_cast<Mixed>(value.operator ::realm::Mixed()));
    }

    void dictionary::insert(const std::string &key, const std::string &value) {
        get_dictionary()->insert(key, StringData(value));
    }
    void dictionary::clear() {
        get_dictionary()->remove_all();
    }

    size_t dictionary::find(const std::string& key) {
        return get_dictionary()->find_any(key);
    }

    void dictionary::remove_all() {
        clear();
    }

    void dictionary::remove(const std::string& key) {
        get_dictionary()->try_erase(key);
    }

    std::pair<std::string, mixed> dictionary::get_pair(size_t idx) {
        auto pair = get_dictionary()->get_pair(idx);
        return { pair.first, static_cast<mixed>(pair.second) };
    }

    size_t dictionary::get_key_index(const std::string& key) {
        Results keys = get_dictionary()->get_keys();
        return keys.index_of(StringData(key));
    }

    inline Dictionary * get_dictionary(dictionary& d) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Dictionary*>(&d.m_dictionary);
#else
        return d.m_dictionary.get();
#endif
    }

    inline const Dictionary * get_dictionary_const(const dictionary& d) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Dictionary*>(&d.m_dictionary);
#else
        return d.m_dictionary.get();
#endif
    }

    template <>
    std::string get(dictionary& dict, const std::string &key) {
        return get_dictionary_const(dict)->get<StringData>(key);
    }
    template <>
    uuid get(dictionary& dict, const std::string &key) {
        return get_dictionary_const(dict)->get<UUID>(key);
    }
    template <>
    object_id get(dictionary& dict, const std::string &key) {
        return get_dictionary_const(dict)->get<ObjectId>(key);
    }
    template <>
    timestamp get(dictionary& dict, const std::string &key) {
        return get_dictionary_const(dict)->get<Timestamp>(key);
    }
    template <>
    binary get(dictionary& dict, const std::string &key) {
        return get_dictionary_const(dict)->get<BinaryData>(key);
    }
    template <>
    int64_t get(dictionary& dict, const std::string &key) {
        return get_dictionary_const(dict)->get<Int>(key);
    }
    template <>
    double get(dictionary& dict, const std::string &key) {
        return get_dictionary_const(dict)->get<Double>(key);
    }
    template <>
    obj_key get(dictionary& dict, const std::string &key) {
        return get_dictionary_const(dict)->get<ObjKey>(key);
    }
    template <>
    obj get(dictionary& dict, const std::string &key) {
        return get_dictionary(dict)->get_object(key);
    }
    template <>
    mixed get(dictionary& dict, const std::string &key) {
        return get_dictionary(dict)->get_any(key);
    }
    template <>
    bool get(dictionary& dict, const std::string &key) {
        return get_dictionary(dict)->get<Bool>(key);
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
        return get_dictionary()->add_notification_callback(ccb);
    }

    notification_token dictionary::add_key_based_notification_callback(std::shared_ptr<dictionary_callback_wrapper>&& cb) {
        struct wrapper {
            std::shared_ptr<dictionary_callback_wrapper> m_cb;
            explicit wrapper(std::shared_ptr<dictionary_callback_wrapper>&& c) : m_cb(std::move(c)) {}

            void operator()(DictionaryChangeSet const& changes) {
                if (changes.deletions.empty() && changes.insertions.empty() && changes.modifications.empty()) {
                    m_cb->handler.operator()(dictionary_collection_change());
                }
                else {
                    std::vector<std::string> insertions;
                    insertions.reserve(changes.insertions.size());
                    std::transform(changes.insertions.begin(), changes.insertions.end(), std::back_inserter(insertions), [](const Mixed& v) { return v.get_string(); });
                    std::vector<std::string> modifications;
                    modifications.reserve(changes.modifications.size());
                    std::transform(changes.modifications.begin(), changes.modifications.end(), std::back_inserter(modifications), [](const Mixed& v) { return v.get_string(); });
                    std::vector<std::string> deletions;
                    deletions.reserve(changes.deletions.size());
                    std::transform(changes.deletions.begin(), changes.deletions.end(), std::back_inserter(deletions), [](const Mixed& v) { return v.get_string(); });
                    m_cb->handler.operator()(dictionary_collection_change(std::move(insertions),
                                                                          std::move(modifications),
                                                                          std::move(deletions),
                                                                          changes.collection_root_was_deleted));
                }
            }
        } wrapper(std::move(cb));
        return get_dictionary()->add_key_based_notification_callback(wrapper);
    }

    obj dictionary::insert_embedded(const std::string &v) {
        return get_dictionary()->insert_embedded(v);
    }
}
