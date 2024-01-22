#include <cpprealm/bson.hpp>

#include <realm/util/bson/bson.hpp>

namespace realm {

    /*
    ===========================
    bsoncxx
    ===========================
    */

    bsoncxx::bsoncxx() noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_bson) realm::bson::Bson();
#else
        m_bson = std::make_shared<realm::bson::Bson>();
#endif
    }

    bsoncxx::~bsoncxx() noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<bson::Bson*>(&m_bson)->~Bson();
#endif
    }

    bsoncxx::bsoncxx(const bsoncxx& v) noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<bson::Bson*>(&m_bson) = *reinterpret_cast<const bson::Bson*>(&v.m_bson);
#else
        m_bson = v.m_bson;
#endif
    }

    bsoncxx::bsoncxx(bsoncxx&& v) noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<bson::Bson*>(&m_bson) = std::move(*reinterpret_cast<bson::Bson*>(&v.m_bson));
#else
        m_bson = std::move(v.m_bson);
#endif
    }

    bsoncxx& bsoncxx::operator=(const bsoncxx& v) noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<bson::Bson*>(&m_bson) = *reinterpret_cast<const bson::Bson*>(&v.m_bson);
#else
        m_bson = v.m_bson;
#endif
        return *this;
    }

    bsoncxx& bsoncxx::operator=(bsoncxx&& v) noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<bson::Bson*>(&m_bson) = std::move(*reinterpret_cast<bson::Bson*>(&v.m_bson));
#else
        m_bson = std::move(v.m_bson);
#endif
        return *this;
    }

    bsoncxx::bsoncxx(realm::bson::Bson& b) noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_bson) realm::bson::Bson(b);
#else
        m_bson = std::make_shared<realm::bson::Bson>(b);
#endif
    }

#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
#define INIT_BSON(value) new (&m_bson) realm::bson::Bson(value);
#else
#define INIT_BSON(value) m_bson = std::make_shared<realm::bson::Bson>(value);
#endif

    bsoncxx::bsoncxx(int32_t v) noexcept
    {
        INIT_BSON(v);
    }
    bsoncxx::bsoncxx(int64_t v) noexcept
    {
        INIT_BSON(v);
    }
    bsoncxx::bsoncxx(bool v) noexcept
    {
        INIT_BSON(v);
    }
    bsoncxx::bsoncxx(double v) noexcept
    {
        INIT_BSON(v);
    }
    bsoncxx::bsoncxx(min_key) noexcept
    {
        INIT_BSON(realm::bson::MinKey());
    }
    bsoncxx::bsoncxx(max_key) noexcept
    {
        INIT_BSON(realm::bson::MaxKey());
    }
    bsoncxx::bsoncxx(const timestamp& v) noexcept
    {
        INIT_BSON(realm::bson::MongoTimestamp(v.m_seconds, v.m_increment));
    }
    bsoncxx::bsoncxx(const date& v) noexcept
    {
        INIT_BSON(realm::Timestamp(v.m_date));
    }
    bsoncxx::bsoncxx(const decimal128& v) noexcept
    {
        INIT_BSON(serialize(v).operator Decimal128());
    }
    bsoncxx::bsoncxx(const object_id& v) noexcept
    {
        INIT_BSON(serialize(v).operator ObjectId());
    }
    bsoncxx::bsoncxx(const uuid& v) noexcept
    {
        INIT_BSON(serialize(v).operator UUID());
    }
    bsoncxx::bsoncxx(const regular_expression& v) noexcept
    {
        INIT_BSON(v.operator bson::RegularExpression());
    }
    bsoncxx::bsoncxx(const std::vector<uint8_t>& v) noexcept
    {
        INIT_BSON(std::vector<char>(v.begin(), v.end()));
    }
    bsoncxx::bsoncxx(const std::string& v) noexcept
    {
        INIT_BSON(v);
    }
    bsoncxx::bsoncxx(const char* v) noexcept
    {
        INIT_BSON(v);
    }
    bsoncxx::bsoncxx(const document& v) noexcept
    {
        INIT_BSON(v.operator document::CoreDocument());
    }
    bsoncxx::bsoncxx(const std::vector<bsoncxx>& v) noexcept
    {
        std::vector<realm::bson::Bson> core_bson;
        std::transform(v.begin(), v.end(), std::back_inserter(core_bson), [](const bsoncxx& b) {
            return b.operator realm::bson::Bson();
        });
        INIT_BSON(std::move(core_bson));
    }

#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
#define CONST_CORE_BSON reinterpret_cast<const bson::Bson*>(&m_bson)
#else
#define CONST_CORE_BSON m_bson
#endif

    bsoncxx::operator realm::bson::Bson() const
    {
        return *CONST_CORE_BSON;
    }

    bool operator==(const bsoncxx& lhs, const bsoncxx& rhs)
    {
        return lhs.operator realm::bson::Bson() == rhs.operator realm::bson::Bson();
    }
    bool operator!=(const bsoncxx& lhs, const bsoncxx& rhs)
    {
        return lhs.operator realm::bson::Bson() != rhs.operator realm::bson::Bson();
    }

    bsoncxx::operator std::nullopt_t() const
    {
        return CONST_CORE_BSON->operator util::None();
    }
    bsoncxx::operator int32_t() const
    {
        return CONST_CORE_BSON->operator int32_t();
    }
    bsoncxx::operator int64_t() const
    {
        return CONST_CORE_BSON->operator int64_t();
    }
    bsoncxx::operator bool() const
    {
        return CONST_CORE_BSON->operator bool();
    }
    bsoncxx::operator double() const
    {
        return CONST_CORE_BSON->operator double();
    }
    bsoncxx::operator min_key() const
    {
        CONST_CORE_BSON->operator bson::MinKey();
        return min_key();
    }
    bsoncxx::operator max_key() const
    {
        CONST_CORE_BSON->operator bson::MaxKey();
        return max_key();
    }
    bsoncxx::operator timestamp() const
    {
        auto ts = CONST_CORE_BSON->operator realm::bson::MongoTimestamp();
        return timestamp(ts.seconds, ts.increment);
    }
    bsoncxx::operator date() const
    {
        auto d = CONST_CORE_BSON->operator realm::Timestamp();
        return date(d.get_time_point());
    }
    bsoncxx::operator decimal128() const
    {
        return deserialize(internal::bridge::decimal128(CONST_CORE_BSON->operator Decimal128()));
    }
    bsoncxx::operator object_id() const
    {
        return deserialize(internal::bridge::object_id(CONST_CORE_BSON->operator ObjectId()));
    }
    bsoncxx::operator uuid() const
    {
        return deserialize(internal::bridge::uuid(CONST_CORE_BSON->operator UUID()));
    }
    bsoncxx::operator regular_expression() const
    {
        return regular_expression(CONST_CORE_BSON->operator const realm::bson::RegularExpression &());
    }
    bsoncxx::operator std::vector<uint8_t>() const
    {
        auto data = CONST_CORE_BSON->operator const std::vector<char>&();
        return std::vector<uint8_t>(data.begin(), data.end());
    }
    bsoncxx::operator std::string() const
    {
        return CONST_CORE_BSON->operator const std::string &();
    }
    bsoncxx::operator document() const
    {
        auto doc = CONST_CORE_BSON->operator const realm::bson::IndexedMap<realm::bson::Bson>&();
        return bsoncxx::document(doc);
    }
    bsoncxx::operator std::vector<bsoncxx>() const
    {
        std::vector<bsoncxx> ret;
        for(auto v : CONST_CORE_BSON->operator const std::vector<realm::bson::Bson>&()) {
            ret.push_back(v);
        }
        return ret;
    }

    bsoncxx::type bsoncxx::get_type() const {
        switch(CONST_CORE_BSON->type()){
            case realm::bson::Bson::Type::Null:
                return bsoncxx::type::b_null;
            case realm::bson::Bson::Type::Int32:
                return bsoncxx::type::b_int32;
            case realm::bson::Bson::Type::Int64:
                return bsoncxx::type::b_int64;
            case realm::bson::Bson::Type::Bool:
                return bsoncxx::type::b_bool;
            case realm::bson::Bson::Type::Double:
                return bsoncxx::type::b_double;
            case realm::bson::Bson::Type::String:
                return bsoncxx::type::b_string;
            case realm::bson::Bson::Type::Binary:
                return bsoncxx::type::b_binary;
            case realm::bson::Bson::Type::Timestamp:
                return bsoncxx::type::b_timestamp;
            case realm::bson::Bson::Type::Datetime:
                return bsoncxx::type::b_datetime;
            case realm::bson::Bson::Type::ObjectId:
                return bsoncxx::type::b_objectId;
            case realm::bson::Bson::Type::Decimal128:
                return bsoncxx::type::b_decimal128;
            case realm::bson::Bson::Type::MaxKey:
                return bsoncxx::type::b_max_key;
            case realm::bson::Bson::Type::MinKey:
                return bsoncxx::type::b_min_key;
            case realm::bson::Bson::Type::Document:
                return bsoncxx::type::b_document;
            case realm::bson::Bson::Type::Array:
                return bsoncxx::type::b_array;
            case realm::bson::Bson::Type::Uuid:
                return bsoncxx::type::b_uuid;
            case realm::bson::Bson::Type::RegularExpression:
                return bsoncxx::type::b_regular_expression;
        }

        throw std::runtime_error("BSON type not supported");
    }

    std::string bsoncxx::to_string() const
    {
        return CONST_CORE_BSON->to_string();
    }

    std::string bsoncxx::to_json() const
    {
        return CONST_CORE_BSON->toJson();
    }

    /*
    ===========================
    bsoncxx::document
    ===========================
    */

#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
#define CONST_CORE_DOCUMENT reinterpret_cast<const CoreDocument*>(&m_document)
#define CORE_DOCUMENT reinterpret_cast<CoreDocument*>(&m_document)
#else
#define CONST_CORE_DOCUMENT m_document
#define CORE_DOCUMENT CONST_CORE_DOCUMENT
#endif

    bsoncxx::document::document() noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_document) CoreDocument();
#else
        m_document = std::make_shared<CoreDocument>();
#endif
    }
    bsoncxx::document::document(const std::initializer_list<std::pair<std::string, bsoncxx>>& v) noexcept
    {
        CoreDocument core_document;
        for (auto& [k, v] : v) {
            core_document[k] = v.operator realm::bson::Bson();
        }
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_document) CoreDocument(std::move(core_document));
#else
        m_document = std::make_shared<CoreDocument>(std::move(core_document));
#endif
    }
    bsoncxx::document::document(const document& v)
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_document) CoreDocument(*reinterpret_cast<const CoreDocument*>(&v.m_document));
#else
        m_document = v.m_document;
#endif
    }
    bsoncxx::document::document(document&& v)
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_document) CoreDocument(std::move(*reinterpret_cast<CoreDocument*>(&v.m_document)));
#else
        m_document = std::move(v.m_document);
#endif
    }
    bsoncxx::document::~document()
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<CoreDocument*>(&m_document)->~CoreDocument();
#endif
    }
    bsoncxx::document& bsoncxx::document::operator=(const document& v)
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<CoreDocument*>(&m_document) = *reinterpret_cast<const CoreDocument*>(&v.m_document);
#else
        m_document = v.m_document;
#endif
        return *this;
    }
    bsoncxx::document& bsoncxx::document::operator=(document&& v)
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<CoreDocument*>(&m_document) = std::move(*reinterpret_cast<const CoreDocument*>(&v.m_document));
#else
        m_document = std::move(v.m_document);
#endif
        return *this;
    }
    bsoncxx::document::document(CoreDocument& doc) noexcept
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_document) CoreDocument(doc);
#else
        m_document = std::make_shared<CoreDocument>(doc);
#endif
    }

    void bsoncxx::document::insert(const std::string& key, const bsoncxx& v)
    {
        CORE_DOCUMENT->operator[](key) = v.operator realm::bson::Bson();
    }

    void bsoncxx::document::pop_back()
    {
        CORE_DOCUMENT->pop_back();
    };

    bool bsoncxx::document::empty()
    {
        return CORE_DOCUMENT->empty();
    };

    size_t bsoncxx::document::size() const
    {
        return CONST_CORE_DOCUMENT->size();
    };

    bsoncxx::document::value bsoncxx::document::operator[](const std::string& key)
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return bsoncxx::document::value(CORE_DOCUMENT, key);
#else
        return bsoncxx::document::value(m_document.get(), key);
#endif
    }

    bsoncxx::document::operator CoreDocument() const
    {
        return *CONST_CORE_DOCUMENT;
    }

    bool operator==(const bsoncxx::document& lhs, const bsoncxx::document& rhs)
    {
        auto l = lhs.operator bsoncxx::document::CoreDocument();
        auto r = rhs.operator bsoncxx::document::CoreDocument();
        return l.keys() == r.keys() && l.entries() == r.entries();
    }
    bool operator!=(const bsoncxx::document& lhs, const bsoncxx::document& rhs)
    {
        auto l = lhs.operator bsoncxx::document::CoreDocument();
        auto r = rhs.operator bsoncxx::document::CoreDocument();
        return l.keys() != r.keys() || l.entries() != r.entries();
    }

    /*
    ===========================
    bsoncxx::document::iterator
    ===========================
    */

    bsoncxx::document::iterator bsoncxx::document::begin()
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return bsoncxx::document::iterator(CORE_DOCUMENT, 0);
#else
        return bsoncxx::document::iterator(m_document.get(), 0);
#endif
    }

    bsoncxx::document::iterator bsoncxx::document::end()
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return bsoncxx::document::iterator(CORE_DOCUMENT, CONST_CORE_DOCUMENT->size());
#else
        return bsoncxx::document::iterator(m_document.get(), m_document->size());
#endif
    }

    std::pair<std::string, bsoncxx> bsoncxx::document::iterator::operator*()
    {
        auto entry = m_document->operator[](m_idx);
        return {entry.first, bsoncxx(entry.second)};
    }
    bsoncxx::document::iterator& bsoncxx::document::iterator::operator++()
    {
        m_idx++;
        return *this;
    }
    bsoncxx::document::iterator& bsoncxx::document::iterator::operator--()
    {
        m_idx--;
        return *this;
    }
    bsoncxx::document::iterator& bsoncxx::document::iterator::operator++(int v)
    {
        m_idx += v;
        return *this;
    }
    bsoncxx::document::iterator& bsoncxx::document::iterator::operator--(int v)
    {
        m_idx -= v;
        return *this;
    }
    bool bsoncxx::document::iterator::operator!=(const bsoncxx::document::iterator& rhs) const noexcept
    {
        return *m_document != *rhs.m_document || m_idx != rhs.m_idx;
    }
    bool bsoncxx::document::iterator::operator==(const bsoncxx::document::iterator& rhs) const noexcept
    {
        return *m_document == *rhs.m_document && m_idx == rhs.m_idx;
    }

    bsoncxx::document::value& bsoncxx::document::value::operator=(const bsoncxx& v)
    {
        m_document->operator[](key) = v;
        return *this;
    }
    bsoncxx::document::value::operator bsoncxx() const
    {
        return bsoncxx(m_document->operator[](key));
    }

    /*
    ===========================
    bsoncxx::regular_expression
    ===========================
    */

    bsoncxx::regular_expression::regular_expression(const realm::bson::RegularExpression& v)
    {
        m_pattern = v.pattern();
        m_options = static_cast<bsoncxx::regular_expression::option>(v.options());
    }
    bsoncxx::regular_expression::regular_expression(const std::string& pattern, const std::string& options)
    {
        realm::bson::RegularExpression expression(pattern, options);
        m_pattern = expression.pattern();
        m_options = static_cast<bsoncxx::regular_expression::option>(expression.options());
    }
    bsoncxx::regular_expression::regular_expression(const std::string& pattern, option options)
    {
        m_pattern = pattern;
        m_options = options;
    }
    bsoncxx::regular_expression::regular_expression()
    {
        m_pattern = "";
        m_options = bsoncxx::regular_expression::option::none;
    }

    bsoncxx::regular_expression::operator realm::bson::RegularExpression() const
    {
        return realm::bson::RegularExpression(m_pattern, static_cast<realm::bson::RegularExpression::Option>(m_options));
    }

    static_assert((int)realm::bson::RegularExpression::Option::None == (int)bsoncxx::regular_expression::option::none);
    static_assert((int)realm::bson::RegularExpression::Option::Multiline == (int)bsoncxx::regular_expression::option::multiline);
    static_assert((int)realm::bson::RegularExpression::Option::IgnoreCase == (int)bsoncxx::regular_expression::option::ignore_case);
    static_assert((int)realm::bson::RegularExpression::Option::Extended == (int)bsoncxx::regular_expression::option::extended);
    static_assert((int)realm::bson::RegularExpression::Option::Dotall == (int)bsoncxx::regular_expression::option::dotall);

    bsoncxx::regular_expression::option bsoncxx::regular_expression::get_options() const
    {
        return m_options;
    }

    std::string bsoncxx::regular_expression::get_pattern() const
    {
        return m_pattern;
    }

    bsoncxx::regular_expression::option operator|(const bsoncxx::regular_expression::option& lhs,
                                                  const bsoncxx::regular_expression::option& rhs) noexcept
    {
        return bsoncxx::regular_expression::option(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    bsoncxx::regular_expression::option operator&(const bsoncxx::regular_expression::option& lhs,
                                                  const bsoncxx::regular_expression::option& rhs) noexcept
    {
        return bsoncxx::regular_expression::option(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    bool operator==(const bsoncxx::regular_expression& lhs, const bsoncxx::regular_expression& rhs) noexcept
    {
        return lhs.operator realm::bson::RegularExpression() == rhs.operator realm::bson::RegularExpression();
    }
    bool operator!=(const bsoncxx::regular_expression& lhs, const bsoncxx::regular_expression& rhs) noexcept
    {
        return lhs.operator realm::bson::RegularExpression() != rhs.operator realm::bson::RegularExpression();
    }

} // namespace realm
