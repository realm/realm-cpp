#include <cpprealm/mixed.hpp>
#include <cpprealm/types.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/db.hpp>

namespace realm {

    mixed::mixed() {
        m_mixed = internal::bridge::mixed();
        m_storage_mode = storage_mode::mixed_bridge;
    };

    mixed::mixed(const internal::bridge::mixed& m) {
        m_storage_mode = storage_mode::managed;
        new (&m_mixed) internal::bridge::mixed(m);
    };

    mixed::mixed(internal::bridge::mixed&& m, internal::bridge::object&& o, internal::bridge::col_key c) {
        m_storage_mode = storage_mode::managed;
        new (&m_mixed) internal::bridge::mixed(m);
        m_obj = std::move(o);
        m_col_key = std::move(c);
    };

    mixed::mixed(std::monostate) {
        m_mixed = internal::bridge::mixed();
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(int64_t v) {
        new (&m_mixed) internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(bool v) {
        m_mixed = internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(const std::string& v) {
        m_mixed = internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(const char* v) {
        new (&m_mixed) internal::bridge::mixed(std::string(v));
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(double v) {
        new (&m_mixed) internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(const std::vector<uint8_t>& v) {
        new (&m_mixed) internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(std::chrono::time_point<std::chrono::system_clock> v) {
        new (&m_mixed) internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(uuid v) {
        new (&m_mixed) internal::bridge::mixed(serialize(v));
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(object_id v) {
        new (&m_mixed) internal::bridge::mixed(serialize(v));
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(decimal128 v) {
        new (&m_mixed) internal::bridge::mixed(serialize(v));
        m_storage_mode = storage_mode::mixed_bridge;
    }

    // MARK: Vector

    mixed::vector::vector(const std::vector<mixed>& o) {
        new (&m_unmanaged_vector) std::vector<mixed>(o);
        m_is_managed = false;
    }

    mixed::vector::vector(const internal::bridge::list& o, std::optional<internal::bridge::col_key> ck) {
        new (&m_managed_vector) internal::bridge::list(o);
        m_col_key = ck;
        m_is_managed = true;
    }

    mixed::vector& mixed::vector::operator=(const std::vector<mixed>& o) {
        if (m_is_managed) {
            m_managed_vector.remove_all();
            context ctx;
            ctx.insert_list_values(m_managed_vector, o);
        } else {
            m_unmanaged_vector.operator=(o);
        }

        return *this;
    }

    void mixed::vector::push_back(const realm::mixed& o) {
        if (m_is_managed) {
            context ctx;
            ctx.add_list_value(m_managed_vector, o);
        } else {
            m_unmanaged_vector.push_back(o);
        }
    }

    void mixed::vector::pop_back() {
        if (m_is_managed) {
            m_managed_vector.remove(m_managed_vector.size() - 1);
        } else {
            m_unmanaged_vector.pop_back();
        }
    }

    void mixed::vector::set(const realm::mixed& o, size_t i) {
        if (m_is_managed) {
            context ctx;
            ctx.set_list_value(m_managed_vector, i, o);
        } else {
            m_unmanaged_vector[i] = o;
        }
    }

    void mixed::vector::erase(size_t i) {
        if (m_is_managed) {
            m_managed_vector.remove(i);
        } else {
            m_unmanaged_vector.erase(m_unmanaged_vector.begin() + 0);
        }
    }

    bool mixed::vector::iterator::operator!=(const mixed::vector::iterator& other) const
    {
        return !(*this == other);
    }

    bool mixed::vector::iterator::operator==(const mixed::vector::iterator& other) const
    {
        return (m_i == other.m_i);
    }

    mixed mixed::vector::iterator::operator*() noexcept
    {
        if (m_is_managed) {
            return internal::bridge::get<internal::bridge::mixed>(m_managed_vector, m_i);
        } else {
            auto it = m_unmanaged_vector.begin();
            std::advance(it, m_i);
            return m_unmanaged_vector[m_i];
        }
    }

    mixed::vector::iterator& mixed::vector::iterator::operator++()
    {
        this->m_i++;
        return *this;
    }

    const mixed::vector::iterator& mixed::vector::iterator::operator++(int i)
    {
        this->m_i += i;
        return *this;
    }

    mixed::vector::iterator::iterator(size_t i, const std::vector<mixed>& o) {
        m_i = i;
        new (&m_unmanaged_vector) std::vector<mixed>(o);
        m_is_managed = false;
    }
    mixed::vector::iterator::iterator(size_t i, const internal::bridge::list& o, internal::bridge::col_key ck) {
        m_i = i;
        new (&m_managed_vector) internal::bridge::list(o);
        m_col_key = ck;
        m_is_managed = true;
    }

    mixed::vector::iterator::~iterator() {

    }

    size_t mixed::vector::size() const
    {
        if (m_is_managed) {
            return m_managed_vector.size();
        } else {
            return m_unmanaged_vector.size();
        }
    }

    mixed::vector::iterator mixed::vector::begin() const
    {
        if (m_is_managed) {
            return iterator(0, m_managed_vector, *m_col_key);
        } else {
            return iterator(0, m_unmanaged_vector);
        }
    }

    mixed::vector::iterator mixed::vector::end() const
    {
        if (m_is_managed) {
            return iterator(size(), m_managed_vector, *m_col_key);
        } else {
            return iterator(size(), m_unmanaged_vector);
        }
    }

    mixed mixed::vector::operator[] (size_t i) {
        if (m_is_managed) {
            return internal::bridge::get<internal::bridge::mixed>(m_managed_vector, i);
        } else {
            return m_unmanaged_vector[i];
        }
    }

    mixed::vector::operator std::vector<mixed>() {
        if (m_is_managed) {
            std::vector<mixed> m;
            for (size_t i = 0; i < m_managed_vector.size(); i++) {
                m.push_back(internal::bridge::get<internal::bridge::mixed>(m_managed_vector, i));
            }
            return m;
        } else {
            return m_unmanaged_vector;
        }
    }

    mixed::vector::~vector() {

    }

    // MARK: Map

    mixed::map::box::box(std::optional<std::map<std::string, mixed>> map, const std::string& key) {
        m_unmanaged_map = map;
        m_key = key;
        m_is_managed = false;
    }

    mixed::map::box::box(std::optional<internal::bridge::dictionary> map, const std::string& key, internal::bridge::col_key c) {
        m_managed_map = map;
        m_key = key;
        m_origin_col_key = c;
        m_is_managed = true;
    }

    mixed::map::box& mixed::map::box::operator=(const mixed&) {
        std::terminate();
    }

    mixed::map::box& mixed::map::box::operator=(mixed&& m) {
        if (m_is_managed) {
            context ctx;
            ctx.insert_dictionary_value(*m_managed_map, m_key, std::move(m));
        } else {
            std::terminate();
        }

        return *this;
    }

    mixed mixed::map::box::operator *() {
        if (m_is_managed) {
            if (m_managed_map->get(m_key).is_collection_type(internal::bridge::mixed::collection_type::dictionary)) {
                auto m = mixed();
                m.m_nested_dictionary_key = m_key;
                m.m_nested_managed_map = m_managed_map->get_dictionary(m_key);
                m.m_storage_mode = storage_mode::managed;
                return m;
            } else if (m_managed_map->get(m_key).is_collection_type(internal::bridge::mixed::collection_type::list)) {
                std::terminate();
            } else {
                return m_managed_map->get(m_key);
            }
        } else {
            return m_unmanaged_map->at(m_key);
        }
    }

    mixed::map::map(const std::map<std::string, mixed>& o) {
        new (&m_unmanaged_map) std::map<std::string, mixed>(o);
        m_is_managed = false;
    }

    mixed::map::map(const internal::bridge::dictionary& o, std::optional<internal::bridge::col_key> ck) {
        new (&m_managed_map) internal::bridge::dictionary(o);
        m_col_key = ck;
        m_is_managed = true;
    }

    mixed::map& mixed::map::operator=(const std::map<std::string, mixed>& o) {
        m_managed_map.clear();
        context ctx;
        ctx.insert_dictionary_values(m_managed_map, o);

        return *this;
    }


    bool mixed::map::iterator::operator!=(const iterator& other) const {
        return !(*this == other);
    }

    bool mixed::map::iterator::operator==(const iterator& other) const {
        return (m_i == other.m_i);
    }

    std::pair<std::string, mixed::map::box> mixed::map::iterator::operator*() noexcept
    {
        if (m_is_managed) {
            auto pair = m_managed_map.get_pair(m_i);
            return { pair.first, mixed::map::box(m_managed_map, pair.first, *m_col_key) };
        } else {
            auto it = m_unmanaged_map.begin();
            std::advance(it, m_i);
            return std::pair { it->first, mixed::map::box(m_unmanaged_map, it->first) };
        }
    }

    mixed::map::iterator& mixed::map::iterator::operator++() {
        this->m_i++;
        return *this;
    }

    const mixed::map::iterator& mixed::map::iterator::operator++(int i) {
        this->m_i += i;
        return *this;
    }

    mixed::map::iterator::iterator(size_t i, const std::map<std::string, mixed>& o) {
        m_i = i;
        new (&m_unmanaged_map) std::map<std::string, mixed>(o);
        m_is_managed = false;
    }

    mixed::map::iterator::iterator(size_t i, const internal::bridge::dictionary& o, internal::bridge::col_key ck) {
        m_i = i;
        new (&m_managed_map) internal::bridge::dictionary(o);
        m_col_key = ck;
        m_is_managed = true;
    }

    mixed::map::iterator::~iterator() {

    }

    size_t mixed::map::size() const {
        if (m_is_managed) {
            return m_managed_map.size();
        } else {
            return m_unmanaged_map.size();
        }
    }

    mixed::map::iterator mixed::map::begin() const {
        if (m_is_managed) {
            return iterator(0, m_managed_map, *m_col_key);
        } else {
            return iterator(0, m_unmanaged_map);
        }
    }

    mixed::map::iterator mixed::map::end() const {
        if (m_is_managed) {
            return iterator(size(), m_managed_map, *m_col_key);
        } else {
            return iterator(size(), m_unmanaged_map);
        }
    }

    mixed::map::iterator mixed::map::find(const std::string& k) {
        if (m_is_managed) {
            // Dictionary's `find` searches for the index of the value and not the key.
            auto i = m_managed_map.get_key_index(k);
            if (i == size_t(-1)) {
                return end();
            } else {
                return iterator(i, m_managed_map, *m_col_key);
            }
        } else {
            size_t loc = size();
            size_t i = 0;
            for (const auto& pair : m_unmanaged_map) {
                if (pair.first == k) {
                    loc = i;
                    break;
                }
                i++;
            }
            return iterator(loc, m_unmanaged_map);
        }
    }

    mixed::map::box mixed::map::operator[] (const std::string& k) {
        if (m_is_managed) {
            return box(m_managed_map, k, *m_col_key);
        } else {
            return box(m_unmanaged_map, k);
        }
    }

    mixed::map::operator std::map<std::string, mixed>() {
        if (m_is_managed) {
            std::map<std::string, mixed> m;
            for (size_t i = 0; i < m_managed_map.size(); i++) {
                auto pair = m_managed_map.get_pair(i);
                m[pair.first] = pair.second;
            }
            return m;
        } else {
            return m_unmanaged_map;
        }
    }

    mixed::map::~map() { }

    mixed::map mixed::get_dictionary() const {
        if (m_storage_mode == storage_mode::managed) {
            if (m_nested_managed_map) {
                return mixed::map(*m_nested_managed_map);
            }
            return mixed::map(m_obj->get_dictionary(*m_col_key), *m_col_key);
        } else if (m_storage_mode == storage_mode::unmanaged_dictionary) {
            return mixed::map(m_unmanaged_map);
        } else {
            throw std::runtime_error("mixed is not dictionary type");
        }
        return std::map<std::string, mixed>();
    }

    mixed::vector mixed::get_list() const {
        if (m_storage_mode == storage_mode::managed) {
            if (m_nested_managed_list) {
                return mixed::vector(*m_nested_managed_list);
            }
            return mixed::vector(m_obj->get_list(*m_col_key), *m_col_key);
        } else if (m_storage_mode == storage_mode::vector) {
            return mixed::vector(m_unmanaged_vector);
        } else {
            throw std::runtime_error("mixed is not vector type");
        }
        return std::vector<mixed>();
    }

    bool operator==(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed == rhs.m_mixed;
    }
    bool operator!=(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed != rhs.m_mixed;
    }
    bool operator>(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed > rhs.m_mixed;
    }
    bool operator>=(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed >= rhs.m_mixed;
    }
    bool operator<(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed < rhs.m_mixed;
    }
    bool operator<=(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed <= rhs.m_mixed;
    }

    namespace internal {
        internal::bridge::mixed get_mixed_bridge(const mixed& m) {
            return m.m_mixed;
        }
    }

    internal::bridge::dictionary context::create_nested_dictionary(internal::bridge::dictionary& d, const std::string& key) {
        d.insert_dictionary(key);
        return d.get_dictionary(key);
    }

    internal::bridge::dictionary context::create_nested_dictionary(internal::bridge::list& l, size_t idx) {
        l.create_nested_dictionary(idx);
        return l.get_dictionary(idx);
    }

    internal::bridge::list context::create_nested_list(internal::bridge::dictionary& d, const std::string& key) {
        d.insert_list(key);
        return d.get_list(key);
    }

    internal::bridge::list context::create_nested_list(internal::bridge::list& l, size_t idx) {
        l.create_nested_list(idx);
        return l.get_list(idx);
    }

    void context::insert_dictionary_value(internal::bridge::dictionary& d, const std::string& key, const mixed& value) {
        switch (value.m_storage_mode) {
            case mixed::storage_mode::uninitialized:
                break;
            case mixed::storage_mode::mixed_bridge:
                d.insert(key, value.m_mixed);
                break;
            case mixed::storage_mode::managed:
                break;
            case mixed::storage_mode::unmanaged_dictionary: {
                auto nested_dict = create_nested_dictionary(d, key);
                insert_dictionary_values(nested_dict, value.m_unmanaged_map);
                break;
            }
            case mixed::storage_mode::vector:
                auto nested_list = create_nested_list(d, key);
                insert_list_values(nested_list, value.m_unmanaged_vector);
                break;
        }
    }

    void context::insert_dictionary_values(internal::bridge::dictionary& d, const std::map<std::string, mixed>& values) {
        for (auto& [k, value] : values) {
            insert_dictionary_value(d, k, value);
        }
    }

    void context::set_list_value(internal::bridge::list& l, size_t idx, const mixed& value) {
        switch (value.m_storage_mode) {
            case mixed::storage_mode::uninitialized:
                break;
            case mixed::storage_mode::mixed_bridge:
                //l.set(idx, value.m_mixed);
                l.set(idx, value.m_mixed);
                break;
            case mixed::storage_mode::managed:
                break;
            case mixed::storage_mode::unmanaged_dictionary: {
                auto nested_dict = create_nested_dictionary(l, idx);
                insert_dictionary_values(nested_dict, value.m_unmanaged_map);
                break;
            }
            case mixed::storage_mode::vector:
                auto nested_list = create_nested_list(l, idx);
                insert_list_values(nested_list, value.m_unmanaged_vector);
                break;
        }
    }

    void context::add_list_value(internal::bridge::list& l, const mixed& value) {
        switch (value.m_storage_mode) {
            case mixed::storage_mode::uninitialized:
                break;
            case mixed::storage_mode::mixed_bridge:
                l.add(value.m_mixed);
                break;
            case mixed::storage_mode::managed:
                break;
            case mixed::storage_mode::unmanaged_dictionary: {
                auto nested_dict = create_nested_dictionary(l, l.size());
                insert_dictionary_values(nested_dict, value.m_unmanaged_map);
                break;
            }
            case mixed::storage_mode::vector:
                auto nested_list = create_nested_list(l, l.size());
                insert_list_values(nested_list, value.m_unmanaged_vector);
                break;
        }
    }

    void context::insert_list_values(internal::bridge::list& l, const std::vector<mixed>& values) {
        for (size_t i = 0; i < values.size(); i++) {
            add_list_value(l, values[i]);
        }
    }

    void context::set_managed(internal::bridge::obj& o,
                     const internal::bridge::col_key& c,
                     const internal::bridge::realm& r,
                     const realm::mixed& v) {

        switch (v.m_storage_mode) {
            case mixed::storage_mode::uninitialized:
                break;
            case mixed::storage_mode::mixed_bridge:
                o.set(c, internal::get_mixed_bridge(v));
                break;
            case mixed::storage_mode::managed:
                break;
            case mixed::storage_mode::unmanaged_dictionary: {
                o.set_dictionary(c);
                auto obj = internal::bridge::object(r, o);
                auto d = obj.get_dictionary(c);
                d.remove_all();
                insert_dictionary_values(d, v.m_unmanaged_map);
                break;
            }
            case mixed::storage_mode::vector:
                o.set_list(c);
                internal::bridge::list list(r, o, c);
                list.remove_all();
                insert_list_values(list, v.m_unmanaged_vector);
                break;
        }
    }


    internal::bridge::object get_stored_object(const mixed& m, const char* class_name, db& realm) {
        auto obj = internal::bridge::object(realm.m_realm, m.m_mixed.operator internal::bridge::obj_link());
        uint32_t alternative_key = realm.m_realm.table_for_object_type(class_name).get_key();
        uint32_t stored_table = obj.get_obj().get_table().get_key();
        if (alternative_key != stored_table) {
            throw std::runtime_error("Different link type stored in mixed type. Stored type: " + obj.get_object_schema().get_name());
        }
        return obj;
    }

    internal::bridge::obj get_obj(const internal::bridge::object& o) {
        return o.get_obj();
    }

    internal::bridge::realm get_realm(const internal::bridge::object& o) {
        return o.get_realm();
    }

    template <>
    std::string mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return o.m_mixed.operator std::string();
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not of type `std::string`");
    }

    template <>
    std::monostate mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                if (o.m_mixed.is_null()) {
                    return std::monostate();
                }
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not null");
    }

    template <>
    int64_t mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return o.m_mixed.operator int64_t();
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not of type `int64_t`");
    }

    template <>
    bool mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return o.m_mixed.operator bool();
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not of type `bool`");
    }

    template <>
    double mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return o.m_mixed.operator double();
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not of type `double`");
    }

    template <>
    std::vector<uint8_t> mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return o.m_mixed.operator internal::bridge::binary().operator std::vector<uint8_t>();
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not of type `std::vector<uint8_t>`");
    }

    template <>
    std::chrono::time_point<std::chrono::system_clock> mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return o.m_mixed.operator internal::bridge::timestamp().operator std::chrono::time_point<std::chrono::system_clock>();
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not of type `std::chrono::time_point<std::chrono::system_clock>`");
    }

    template <>
    uuid mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return uuid(o.m_mixed.operator internal::bridge::uuid());
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not of type `uuid`");
    }

    template <>
    object_id mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return object_id(o.m_mixed.operator internal::bridge::object_id());
            default:
                break;
        }
        throw std::runtime_error("Mixed object is not of type `object_id`");
    }

    template <>
    decimal128 mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
            return decimal128(o.m_mixed.operator internal::bridge::decimal128());
            default:
            break;
        }
        throw std::runtime_error("Mixed object is not of type `decimal128`");
    }

    template <>
    mixed::map mixed_cast(const mixed& o) {
        return o.get_dictionary();
    }

    template <>
    mixed::vector mixed_cast(const mixed& o) {
        return o.get_list();
    }

} // namespace realm