#include <cpprealm/experimental/managed_string.hpp>
#include <cpprealm/experimental/db.hpp>
#include <cpprealm/rbool.hpp>

namespace realm::experimental {
    using managed_string = managed<std::string>;
    using reference = managed_string::reference;
    using const_reference = managed_string::const_reference;

    //MARK: - char reference
    char_reference::char_reference(managed<std::string> *parent, size_t idx)
        : m_parent(parent), m_idx(idx) {}
    reference& reference::operator=(char c) {
        std::string value = m_parent->get();
        value[m_idx] = c;
        m_parent->set(value);
        return *this;
    }

    //MARK: - const char reference
    const_char_reference::const_char_reference(const managed<std::string> *parent, size_t idx)
        : m_parent(parent), m_idx(idx) {}
    const_reference::operator char() const {
        return m_parent->get()[m_idx];
    }

    //MARK: - char pointer
    char_pointer::char_pointer(managed<std::string> *parent)
    : m_parent(parent) {}
    char_reference char_pointer::operator[](size_t idx) {
        return { m_parent, idx };
    }
    const_char_pointer::const_char_pointer(const managed<std::string> *parent)
            : m_parent(parent) {}
    const_char_reference const_char_pointer::operator[](size_t idx) const {
        return { m_parent, idx };
    }

    //MARK: - element access
    reference managed_string::at(size_t idx) {
        return { this, idx };
    }
    reference managed_string::operator[](size_t idx) {
        return { this, idx };
    }
    reference managed_string::front() {
        return { this, 0 };
    }
    const_reference managed_string::front() const {
        return { this, 0 };
    }
    reference managed_string::back() {
        return { this, size() - 1 };
    }
    const_reference managed_string::back() const {
        return { this, size() - 1 };
    }
    char_pointer managed_string::data() {
        return char_pointer(this);
    }
    const_char_pointer managed_string::data() const {
        return const_char_pointer(this);
    }
    const_char_pointer managed_string::c_str() const {
        return const_char_pointer(this);
    }
    // MARK: - operations
    void managed_string::clear() noexcept {
        set("");
    }
    void managed_string::push_back(char c) {
        std::string val = get();
        val.push_back(c);
        set(val);
    }
    size_t managed_string::size() const noexcept {
        return get().size();
    }
    managed<std::string> &managed_string::append(const std::string &v) {
        set(get().append(v));
        return *this;
    }
    managed<std::string> &managed_string::operator+=(const std::string &v) {
        set(get().append(v));
        return *this;
    }
    void managed_string::pop_back() {
        auto value = get();
        value.pop_back();
        set(value);
    }

    std::string managed_string::detach() const {
        return get();
    }

    managed_string::operator std::string() const {
        return detach();
    };

    rbool managed_string::operator==(const char* rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.equal(this->m_key, std::string(rhs));
            return query;
        }
        return detach() == rhs;
    }

    rbool managed_string::operator!=(const char* rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.not_equal(this->m_key, std::string(rhs));
            return query;
        }
        return detach() != rhs;
    }

    rbool managed_string::contains(const std::string &rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.contains(this->m_key, std::string(rhs));
            return query;
        }
        return detach().find(rhs) != realm::npos;
    }

    rbool managed_string::empty() const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.equal(this->m_key, std::string());
            return query;
        } else {
            return detach().empty();
        }
    }

    __cpprealm_build_experimental_query(==, equal, std::string)
    __cpprealm_build_experimental_query(!=, not_equal, std::string)

    __cpprealm_build_optional_experimental_query(==, equal, std::string)
    __cpprealm_build_optional_experimental_query(!=, not_equal, std::string)
#ifdef __cpp_lib_starts_ends_with
    bool managed_string::starts_with(std::string_view v) const noexcept {
        return get().starts_with(v);
    }
    bool managed_string::ends_with(std::string_view v) const noexcept {
        return get().ends_with(v);
    }
#endif
}
