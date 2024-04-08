////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_MANAGED_STRING_HPP
#define CPPREALM_MANAGED_STRING_HPP

#include <cpprealm/macros.hpp>

namespace realm {
    class rbool;
}

namespace realm {
    //MARK: - char reference
    struct char_reference {
        char_reference& operator =(char c);
    private:
        char_reference(managed<std::string>* parent, size_t idx);
        template <typename, typename> friend struct realm::managed;
        friend struct char_pointer;
        managed<std::string>* m_parent;
        size_t m_idx;
    };
    //MARK: - const char reference
    struct const_char_reference {
        operator char() const; //NOLINT(google-explicit-constructor)
    private:
        const_char_reference(const managed<std::string>* parent, size_t idx);
        template <typename, typename> friend struct realm::managed;
        friend struct const_char_pointer;
        const managed<std::string>* m_parent;
        size_t m_idx;
    };
    //MARK: - char pointer
    struct char_pointer {
        char_reference operator [](size_t idx);
    private:
        explicit char_pointer(managed<std::string>* parent);
        template <typename, typename> friend struct realm::managed;
        managed<std::string>* m_parent;
    };
    //MARK: - const char pointer
    struct const_char_pointer {
        const_char_reference operator [](size_t idx) const;
    private:
        explicit const_char_pointer(const managed<std::string>* parent);
        template <typename, typename> friend struct realm::managed;
        const managed<std::string>* m_parent;
    };

    //MARK: - managed string
    template <> struct managed<std::string> : managed_base {
        using value_type = std::string;
        using managed<std::string>::managed_base::managed_base;
        using managed<std::string>::managed_base::operator=;

        [[nodiscard]] std::string detach() const;

        using reference = char_reference;
        using const_reference = const_char_reference;

        managed& operator =(std::string&& v) { set(std::move(v)); return *this; }
        managed& operator =(const std::string& v) { set(v); return *this; }
        managed& operator =(const char* v) { set(v); return *this; }

        //MARK: -   element access
        /// accesses the specified character with bounds checking
        reference at(size_t idx);
        /// accesses the specified character
        reference operator[](size_t idx);

        /// accesses the first character
        [[nodiscard]] const_reference front() const;
        reference front();
        /// accesses the last character
        [[nodiscard]] const_reference back() const;
        reference back();
        /// returns a pointer to the first character of a string
        char_pointer data();
        [[nodiscard]] const_char_pointer data() const;

        /// returns a non-modifiable standard C character array version of the string
        [[nodiscard]] const_char_pointer c_str() const;

        /// returns a non-modifiable string_view into the entire string
        operator std::string_view() const = delete;
        operator std::string() const;


        [[nodiscard]] size_t size() const noexcept;
        //MARK: -   operations
        void clear() noexcept;
        void push_back(char c);
        /// appends additional characters to the string
        managed& append(const std::string&);
        /// appends additional characters to the string
        managed& operator+=(const std::string&);
        /// removes the last character from the string.
        void pop_back();

        //MARK: -   comparison operators
        rbool operator==(const std::string& rhs) const noexcept;
        rbool operator==(const char* rhs) const noexcept;
        rbool operator!=(const std::string& rhs) const noexcept;
        rbool operator!=(const char* rhs) const noexcept;
        rbool contains(const std::string &s, bool case_sensitive = true) const noexcept;
        rbool empty() const noexcept;
    private:
        friend struct char_reference;
        friend struct const_char_reference;
        void inline set(const std::string& v) { m_obj->template set<std::string>(m_key, v); }
        [[nodiscard]] inline std::string get() const { return m_obj->get<std::string>(m_key); }
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

    template <> struct managed<std::optional<std::string>> final : public managed<std::string> {
        using value_type = std::optional<std::string>;
        using managed<std::string>::operator=;
        managed& operator =(std::optional<std::string>&& v) { set(std::move(v)); return *this; }
        managed& operator =(const std::optional<std::string>& v) { set(v); return *this; }
        managed& operator =(const char* v) { set(v); return *this; }

        [[nodiscard]] std::optional<std::string> detach() const {
            return m_obj->template get_optional<std::string>(m_key);
        }

        [[nodiscard]] std::optional<std::string> operator *() const {
            return detach();
        }

        [[nodiscard]] operator std::optional<std::string>() const {
            return detach();
        }

        rbool operator==(const std::optional<std::string>& rhs) const noexcept;
        rbool operator!=(const std::optional<std::string>& rhs) const noexcept;
    private:
        void inline set(const std::optional<std::string>& v) { m_obj->template set<std::optional<std::string>>(m_key, v); }
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };
}

#endif //CPPREALM_MANAGED_STRING_HPP
