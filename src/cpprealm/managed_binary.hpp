#ifndef CPPREALM_MANAGED_BINARY_HPP
#define CPPREALM_MANAGED_BINARY_HPP

#include "macros.hpp"
#include "types.hpp"

namespace realm {
    class rbool;
}

namespace realm {

    template<>
    struct managed<std::vector<uint8_t>> : managed_base {
        using managed<std::vector<uint8_t>>::managed_base::operator=;

        [[nodiscard]] std::vector<uint8_t> detach() const;
        [[nodiscard]]  operator std::vector<uint8_t>() const;

        std::vector<uint8_t> operator*() const;
        void push_back(uint8_t v);
        uint8_t operator[](uint8_t idx) const;
        size_t size() const;

        //MARK: -   comparison operators
        rbool operator==(const std::vector<uint8_t>& rhs) const noexcept;
        rbool operator!=(const std::vector<uint8_t>& rhs) const noexcept;
    };

    template<>
    struct managed<std::optional<std::vector<uint8_t>>> : managed_base {
        using managed<std::optional<std::vector<uint8_t>>>::managed_base::operator=;

        [[nodiscard]] std::optional<std::vector<uint8_t>> detach() const;
        [[nodiscard]]  operator std::optional<std::vector<uint8_t>>() const;

        struct box {
            std::optional<std::vector<uint8_t>> operator*() const;
            void push_back(uint8_t v);
            uint8_t operator[](uint8_t idx) const;
            size_t size() const;
        private:
            box(managed& parent) : m_parent(parent) { }
            std::reference_wrapper<managed<std::optional<std::vector<uint8_t>>>> m_parent;
            friend struct managed<std::optional<std::vector<uint8_t>>>;
        };

        std::unique_ptr<box> operator->()
        {
            return std::make_unique<box>(box(*this));
        }
        [[nodiscard]] box operator*() {
            return box(*this);
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<std::vector<uint8_t>>& rhs) const noexcept;
        rbool operator!=(const std::optional<std::vector<uint8_t>>& rhs) const noexcept;
    };
}


#endif//CPPREALM_MANAGED_BINARY_HPP
