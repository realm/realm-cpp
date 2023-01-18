#ifndef CPP_REALM_STRING_HPP
#define CPP_REALM_STRING_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template <>
    struct persisted<std::string> : public persisted_primitive_base<std::string> {
        [[nodiscard]] rbool empty() const;
        [[nodiscard]] size_t size() const noexcept;
        [[nodiscard]] size_t length() const noexcept;
        void push_back(char);
        [[nodiscard]] rbool contains(const std::string&) const;
        persisted& operator =(const char*);
        persisted(const char*);
        persisted();
    protected:
        static std::string serialize(const std::string&);
        static std::string deserialize(const std::string&);

        __cpp_realm_friends
    };

#define __cpp_realm_generate_const_char_operator(type, op) \
    inline rbool operator op(const persisted<type>& a, const char* b) { \
        if (a.should_detect_usage_for_queries) { \
            auto query = internal::bridge::query(a.query->get_table()); \
            if constexpr (std::string_view(#op) == std::string_view("=="))                                   \
                query.equal(a.managed, persisted<std::string>::serialize(b));     \
            else if constexpr (std::string_view(#op) == std::string_view("!="))                                   \
                query.not_equal(a.managed, persisted<std::string>::serialize(b));   \
            return query; \
        } \
        return *a op b; \
    }

    __cpp_realm_generate_operator(std::string, ==, equal)
    __cpp_realm_generate_operator(std::string, !=, not_equal)
    __cpp_realm_generate_const_char_operator(std::string, ==)
    __cpp_realm_generate_const_char_operator(std::string, !=)
}

#endif //CPP_REALM_STRING_HPP
