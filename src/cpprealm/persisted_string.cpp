#include <cpprealm/persisted_string.hpp>

namespace realm {
    persisted<std::string>::persisted() {
        unmanaged = "";
    }
    persisted<std::string>::persisted(const char *v)
    {
        unmanaged = v;
    }
    rbool persisted<std::string>::contains(const std::string &s) const {
        if (is_managed()) {
            return m_object->obj().get<std::string>(managed).find(s) != npos;
        } else {
            return unmanaged.find(s) != npos;
        }
    }

    rbool persisted<std::string>::empty() const {
        if (m_object) {
            return m_object->obj().get<std::string>(managed).empty();
        } else {
            return unmanaged.empty();
        }
    }

    size_t persisted<std::string>::size() const noexcept {
        if (m_object) {
            return m_object->obj().get<std::string>(managed).size();
        } else {
            return unmanaged.size();
        }
    }

    size_t persisted<std::string>::length() const noexcept {
        if (this->is_managed()) {
            return m_object->obj().get<std::string>(managed).length();
        } else {
            return unmanaged.length();
        }
    }

    void persisted<std::string>::push_back(char c) {
        if (this->is_managed()) {
            m_object->obj().set(managed,
                                m_object->obj().get<std::string>(managed) + c);
        } else {
            unmanaged.push_back(c);
        }
    }

    std::string persisted<std::string>::serialize(const std::string &v) {
        return v;
    }
    std::string persisted<std::string>::deserialize(const std::string &v) {
        return v;
    }

    persisted<std::string> &persisted<std::string>::operator=(const char *v) {
        if (this->is_managed()) {
            m_object->obj().set(managed, std::string(v));
        } else {
            unmanaged = v;
        }
        return *this;
    }
}
