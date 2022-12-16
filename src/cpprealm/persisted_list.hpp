#ifndef CPP_REALM_LIST_HPP
#define CPP_REALM_LIST_HPP

#include <cpprealm/persisted.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/object.hpp>
#include <cpprealm/persisted_string.hpp>

namespace realm {
    namespace internal {

        template <typename T>
        struct persisted_list_base : persisted_base<T> {
            class iterator {
            public:
                using difference_type = typename T::size_type;
                using pointer = typename T::value_type*;
                using reference = typename T::value_type&;
                using iterator_category = std::input_iterator_tag;

                bool operator!=(const iterator& other) const
                {
                    return !(*this == other);
                }

                bool operator==(const iterator& other) const
                {
                    return (m_parent == other.m_parent) && (m_idx == other.m_idx);
                }

                reference operator*() noexcept
                {
                    value = (*m_parent)[m_idx];
                    return value;
                }

                pointer operator->() const noexcept
                {
                    value = (*m_parent)[m_idx];
                    return &value;
                }

                iterator& operator++()
                {
                    m_idx++;
                    return *this;
                }

                iterator& operator++(int i)
                {
                    m_idx += i;
                    return *this;
                }
            private:
                friend struct persisted_list_base<T>;

                iterator(size_t idx, persisted_list_base<T>* parent)
                        : m_idx(idx)
                        , m_parent(parent)
                {
                }

                size_t m_idx;
                persisted_list_base<T>* m_parent;
                typename T::value_type value;
            };
            [[nodiscard]] bool empty() const
            {
                if (this->is_managed()) return m_list.size() == 0;
                else return this->unmanaged.empty();
            }
            [[nodiscard]] size_t size() const noexcept
            {
                if (this->is_managed()) return m_list.size();
                else return this->unmanaged.size();
            }
            iterator begin()
            {
                return iterator(0, this);
            }

            iterator end()
            {
                return iterator(size(), this);
            }
            void pop_back() {
                if (this->is_managed()) m_list.remove(size() - 1);
                else this->unmanaged.pop_back();
            }
            void erase(size_t idx) {
                if (this->is_managed()) m_list.remove(idx);
                else this->unmanaged.erase(this->unmanaged.begin() + idx);
            }
            void clear() {
                if (this->is_managed()) m_list.remove_all();
                else this->unmanaged.clear();
            }
            persisted_list_base(const persisted_list_base& v) {
                *this = v;
            }
            persisted_list_base& operator=(const persisted_list_base& v) {
                if (v.is_managed()) {
                    this->m_object = v.m_object;
                    m_list = v.m_list;
                } else {
                    unmanaged = v.unmanaged;
                }
                return *this;
            }
            persisted_list_base() {
                new (&unmanaged) std::vector<T>();
            }
            ~persisted_list_base() {
                if (this->m_object) {
                    this->m_list.~list();
                } else {
                    using namespace std;
                    this->unmanaged.~decltype(this->unmanaged)();
                }
            }
            virtual typename T::value_type operator[](size_t idx) const = 0;
            notification_token observe(std::function<void(collection_change<T>)> fn) {
                abort();
            }
        protected:
            union {
                T unmanaged;
                internal::bridge::list m_list;
            };
            void manage(internal::bridge::object *object,
                        internal::bridge::col_key &&col_key) final {
                object->obj().set_list_values(col_key, unmanaged);
                assign_accessor(object, std::move(col_key));
            }
            void assign_accessor(internal::bridge::object *object,
                                 internal::bridge::col_key &&col_key) final {
                this->m_object = object;
                m_list = object->get_list(col_key);
            }

            __cpp_realm_friends
        };
    }

    template <typename T>
    struct persisted<std::vector<T>, std::enable_if_t<internal::type_info::is_primitive<T>::value>>
            : internal::persisted_list_base<std::vector<T>> {
        void push_back(const T& value)
        {
            if (this->is_managed()) this->m_list.template add(value);
            else this->unmanaged.push_back(value);
        }
        T operator[](size_t idx) const override
        {
            if (this->is_managed()) return persisted<T>::deserialize(this->m_list.template
                    get<typename internal::type_info::type_info<T>::internal_type>(idx));
            else return this->unmanaged[idx];
        }
        size_t find(const T& a) {
            if (this->is_managed()) {
                return this->m_list.find(a);
            } else {
                // unmanaged objects in vectors aren't equatable.
                auto it = std::find(this->unmanaged.begin(), this->unmanaged.end(), a);
                if (it != this->unmanaged.end()) {
                    return it - this->unmanaged.begin();
                } else {
                    return npos;
                }
            }
        }
        void set(size_t pos, const T& a) {
            if (this->is_managed()) {
                this->m_list.set(pos, a);
            } else {
                this->unmanaged[pos] = a;
            }
        }
    protected:
        __cpp_realm_friends
    };

    template <class T>
    struct persisted<std::vector<T>, std::enable_if_t<std::is_base_of_v<object, T>>> :
            internal::persisted_list_base<std::vector<T>> {
        void push_back(T& a) {
            if (this->m_object) {
                if (!a.m_object) {
                    a.manage(this->m_object->get_realm().table_for_object_type(T::schema.name),
                             this->m_list.get_realm(),
                             T::schema);
                }
                this->m_list.add(a.m_object->obj().get_key());
            } else {
                this->unmanaged.push_back(a);
            }
        }
        void push_back(T&& a) {
            if (this->is_managed()) {
                if (!a.m_object) {
                    a.manage(this->m_object->get_realm().table_for_object_type(T::schema.name),
                             this->m_list.get_realm(),
                             T::schema);
                }
                this->m_list.add(a.m_object->obj().get_key());
            } else {
                this->unmanaged.push_back(a);
            }
        }
        void set(size_t pos, T& a) {
            if (this->m_obj) {
                if (!a.m_obj) {
                    T::schema::add(a, this->m_object->get_realm().table_for_object_type(T::schema.name), nullptr);
                }
                this->m_backing_list.set(pos, a.m_obj->get_key());
            } else {
                this->unmanaged[pos] = a;
            }
        }

        void set(size_t pos, T&& a) {
            if (this->m_obj) {
                set(pos, a);
            } else {
                this->unmanaged[pos] = std::move(a);
            }
        }

        size_t find(const T& a) {
            if (this->m_object && this->m_object->is_valid() && a.m_object && a.m_object->is_valid()) {
                return this->m_list.find(a.m_object->obj().get_key());
            } else {
                // unmanaged objects in vectors aren't equatable.
                return realm::npos;
            }
        }

        T operator[](size_t idx) const override {
            if (this->m_object) {
                T cls;
                cls.assign_accessors(internal::bridge::object(this->m_list.get_realm(),
                                                              this->m_list.template get<internal::bridge::obj>(idx)),
                                     T::schema);
                return cls;
            } else {
                return this->unmanaged[idx];
            }
        }
    protected:
        __cpp_realm_friends
    };

    template <class T>
    struct persisted<std::vector<T>, std::enable_if_t<std::is_base_of_v<embedded_object, T>>> :
            internal::persisted_list_base<std::vector<T>> {
        void push_back(T& a) {
            if (this->m_object) {
                if (!a.m_object) {
                    a.manage(internal::bridge::object(this->m_list.get_realm(),
                                                      this->m_list.add_embedded()),
                             T::schema);
                }
            } else {
                this->unmanaged.push_back(a);
            }
        }
        void push_back(T&& a) {
            if (this->m_object) {
                if (!a.m_object) {
                    a.m_object = internal::bridge::object(this->m_list.get_realm(),
                                                          this->m_list.add_embedded());
                }
            } else {
                this->unmanaged.push_back(a);
            }
        }
        void set(size_t pos, T& a) {
            if (this->m_obj) {
                if (!a.m_obj) {
                    T::schema::add(a, this->m_obj->get_table()->get_link_target(this->managed), nullptr);
                }
                this->m_backing_list.set(pos, a.m_obj->get_key());
            } else {
                this->unmanaged[pos] = a;
            }
        }

        void set(size_t pos, T&& a) {
            if (this->m_obj) {
                set(pos, a);
            } else {
                this->unmanaged[pos] = std::move(a);
            }
        }

        size_t find(const T& a) {
            if (this->m_object && this->m_object->is_valid() && a.m_object && a.m_object->is_valid()) {
                return this->m_list.find(a.m_object->obj().get_key());
            } else {
                // unmanaged objects in vectors aren't equatable.
                return realm::npos;
            }
        }

        T operator[](size_t idx) const override {
            if (this->m_object) {
                T cls;
                cls.assign_accessors(internal::bridge::object(this->m_list.get_realm(),
                                                              this->m_list.template get<internal::bridge::obj>(idx)),
                                     T::schema);
                return cls;
            } else {
                return this->unmanaged[idx];
            }
        }
    protected:
        __cpp_realm_friends
    };

    namespace {
        struct static_object : object {
            persisted<int64_t> int_col;

            static constexpr auto schema = realm::schema("static_object",
                                                         realm::property<&static_object::int_col>("int_col"));
        };
        struct static_embedded_object : embedded_object {
            persisted<int64_t> int_col;

            static constexpr auto schema = realm::schema("static_embedded_object",
                                         realm::property<&static_embedded_object::int_col>("int_col"));
        };
        static_assert(sizeof(persisted<std::vector<std::string>>));
        static_assert(sizeof(persisted<std::vector<static_object>>));
        static_assert(sizeof(persisted<std::vector<static_embedded_object>>));
    }
}

#endif //CPP_REALM_LIST_HPP
