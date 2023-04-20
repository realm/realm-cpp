#include <realm/object_id.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/persisted_object_id.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<12, sizeof(::realm::ObjectId)>{});
    static_assert(SizeCheck<1, alignof(::realm::ObjectId)>{});
#elif __x86_64__
    static_assert(SizeCheck<12, sizeof(::realm::ObjectId)>{});
    static_assert(SizeCheck<1, alignof(::realm::ObjectId)>{});
#elif __arm__
    static_assert(SizeCheck<12, sizeof(::realm::ObjectId)>{});
    static_assert(SizeCheck<1, alignof(::realm::ObjectId)>{});
#elif __aarch64__
    static_assert(SizeCheck<12, sizeof(::realm::ObjectId)>{});
    static_assert(SizeCheck<1, alignof(::realm::ObjectId)>{});
#endif

    object_id::object_id() {
        new(&m_object_id) ObjectId();
    }
    object_id::object_id(const object_id& other) {
        new (&m_object_id) ObjectId(*reinterpret_cast<const ObjectId*>(other.m_object_id));
    }

    object_id& object_id::operator=(const object_id& other) {
        if (this != &other) {
            *reinterpret_cast<ObjectId*>(m_object_id) = *reinterpret_cast<const ObjectId*>(other.m_object_id);
        }
        return *this;
    }

    object_id::object_id(object_id&& other) {
        new (&m_object_id) ObjectId(std::move(*reinterpret_cast<ObjectId*>(other.m_object_id)));
    }

    object_id& object_id::operator=(object_id&& other) {
        if (this != &other) {
            *reinterpret_cast<ObjectId*>(m_object_id) = std::move(*reinterpret_cast<ObjectId*>(other.m_object_id));
        }
        return *this;
    }

    object_id::~object_id() {
        reinterpret_cast<ObjectId*>(m_object_id)->~ObjectId();
    }

    object_id::object_id(const std::string &v) {
        new(&m_object_id) ObjectId(v);
    }

    object_id::object_id(const ::realm::object_id &v) {
        new(&m_object_id) ObjectId(v.to_string());
    }

    object_id::object_id(const ObjectId &v) {
        new(&m_object_id) ObjectId(v);
    }

    std::string object_id::to_string() const {
        return reinterpret_cast<const ObjectId *>(m_object_id)->to_string();
    }

    object_id object_id::generate() {
        return ObjectId::gen();
    }

    object_id::operator ::realm::object_id() const {
        return ::realm::object_id(*this);
    }

    object_id::operator ObjectId() const {
        return *reinterpret_cast<const ObjectId *>(m_object_id);
    }

#define __cpp_realm_gen_object_id_op(op) \
    bool operator op(const object_id& a, const object_id& b) { \
        return *reinterpret_cast<const ObjectId*>(a.m_object_id) op *reinterpret_cast<const ObjectId*>(b.m_object_id);                                        \
    }

    __cpp_realm_gen_object_id_op(==)
    __cpp_realm_gen_object_id_op(!=)
    __cpp_realm_gen_object_id_op(>)
    __cpp_realm_gen_object_id_op(<)
    __cpp_realm_gen_object_id_op(>=)
    __cpp_realm_gen_object_id_op(<=)
}