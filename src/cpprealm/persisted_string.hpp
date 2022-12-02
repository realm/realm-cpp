#ifndef CPP_REALM_STRING_HPP
#define CPP_REALM_STRING_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template <>
    struct persisted<std::string> : public persisted_base<std::string> {
        [[nodiscard]] rbool empty() const;
        [[nodiscard]] size_t size() const noexcept;
        [[nodiscard]] size_t length() const noexcept;
        void push_back(char);
        [[nodiscard]] rbool contains(const std::string&) const;
        persisted& operator =(const char*);
        persisted(const char*);
        persisted();
    };

    inline rbool operator==(const persisted<std::string>& a, const char* b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            query.equal(a.managed, internal::type_info::serialize<std::string>(b));
            return query;
        }
        return *a == b;
    }
    inline rbool operator!=(const persisted<std::string>& a, const char* b)
    {
        return !(a == b);
    }
}

#endif //CPP_REALM_STRING_HPP
