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
        m_bson = std::make_shared<realm::bson::Bson>();
    }

    bsoncxx::~bsoncxx() noexcept
    {

    }

    bsoncxx::bsoncxx(const bsoncxx& v) noexcept
    {
        m_bson = v.m_bson;
    }

    bsoncxx::bsoncxx(bsoncxx&& v) noexcept
    {
        m_bson = std::move(v.m_bson);
    }

    bsoncxx& bsoncxx::operator=(const bsoncxx& v) noexcept
    {
        m_bson = v.m_bson;
        return *this;
    }

    bsoncxx& bsoncxx::operator=(bsoncxx&& v) noexcept
    {
        m_bson = std::move(v.m_bson);
        return *this;
    }

    bsoncxx::bsoncxx(realm::bson::Bson& b) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(b);
    }

    bsoncxx::bsoncxx(int32_t v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(v);
    }
    bsoncxx::bsoncxx(int64_t v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(v);
    }
    bsoncxx::bsoncxx(bool v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(v);
    }
    bsoncxx::bsoncxx(double v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(v);
    }
    bsoncxx::bsoncxx(min_key) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(realm::bson::MinKey());
    }
    bsoncxx::bsoncxx(max_key) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(realm::bson::MaxKey());
    }
    bsoncxx::bsoncxx(const timestamp& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(realm::bson::MongoTimestamp(v.m_seconds, v.m_increment));
    }
    bsoncxx::bsoncxx(const date& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(realm::Timestamp(v.m_date));
    }
    bsoncxx::bsoncxx(const decimal128& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(serialize(v).operator Decimal128());
    }
    bsoncxx::bsoncxx(const object_id& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(serialize(v).operator ObjectId());
    }
    bsoncxx::bsoncxx(const uuid& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(serialize(v).operator UUID());
    }
    bsoncxx::bsoncxx(const regular_expression& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(v.operator bson::RegularExpression());
    }
    bsoncxx::bsoncxx(const std::vector<uint8_t>& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(std::vector<char>(v.begin(), v.end()));
    }
    bsoncxx::bsoncxx(const std::string& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(v);
    }
    bsoncxx::bsoncxx(const char* v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(v);
    }
    bsoncxx::bsoncxx(const document& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(v.operator document::CoreDocument());
    }
    bsoncxx::bsoncxx(const std::vector<bsoncxx>& v) noexcept
    {
        std::vector<realm::bson::Bson> core_bson;
        std::transform(v.begin(), v.end(), std::back_inserter(core_bson), [](const bsoncxx& b) {
            return b.operator realm::bson::Bson();
        });
        m_bson = std::make_shared<realm::bson::Bson>(std::move(core_bson));
    }

    bsoncxx::operator realm::bson::Bson() const
    {
        return *m_bson;
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
        return m_bson->operator util::None();
    }
    bsoncxx::operator int32_t() const
    {
        return m_bson->operator int32_t();
    }
    bsoncxx::operator int64_t() const
    {
        return m_bson->operator int64_t();
    }
    bsoncxx::operator bool() const
    {
        return m_bson->operator bool();
    }
    bsoncxx::operator double() const
    {
        return m_bson->operator double();
    }
    bsoncxx::operator min_key() const
    {
        m_bson->operator bson::MinKey();
        return min_key();
    }
    bsoncxx::operator max_key() const
    {
        m_bson->operator bson::MaxKey();
        return max_key();
    }
    bsoncxx::operator timestamp() const
    {
        auto ts = m_bson->operator realm::bson::MongoTimestamp();
        return timestamp(ts.seconds, ts.increment);
    }
    bsoncxx::operator date() const
    {
        auto d = m_bson->operator realm::Timestamp();
        return date(d.get_time_point());
    }
    bsoncxx::operator decimal128() const
    {
        return deserialize(internal::bridge::decimal128(m_bson->operator Decimal128()));
    }
    bsoncxx::operator object_id() const
    {
        return deserialize(internal::bridge::object_id(m_bson->operator ObjectId()));
    }
    bsoncxx::operator uuid() const
    {
        return deserialize(internal::bridge::uuid(m_bson->operator UUID()));
    }
    bsoncxx::operator regular_expression() const
    {
        return regular_expression(m_bson->operator const realm::bson::RegularExpression &());
    }
    bsoncxx::operator std::vector<uint8_t>() const
    {
        auto data = m_bson->operator std::vector<char>&();
        return std::vector<uint8_t>(data.begin(), data.end());
    }
    bsoncxx::operator std::string() const
    {
        return m_bson->operator std::string &();
    }
    bsoncxx::operator document() const
    {
        auto doc = m_bson->operator const realm::bson::IndexedMap<realm::bson::Bson>&();
        return bsoncxx::document(doc);
    }
    bsoncxx::operator std::vector<bsoncxx>() const
    {
        std::vector<bsoncxx> ret;
        for(auto v : m_bson->operator std::vector<realm::bson::Bson>&()) {
            ret.push_back(v);
        }
        return ret;
    }

    bsoncxx::type bsoncxx::get_type() const {
        switch(m_bson->type()){
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
        return m_bson->to_string();
    }

    std::string bsoncxx::to_json() const
    {
        return m_bson->toJson();
    }

    /*
    ===========================
    bsoncxx::document
    ===========================
    */

    bsoncxx::document::document() noexcept
    {
        m_document = std::make_shared<CoreDocument>();
    }
    bsoncxx::document::document(const std::initializer_list<std::pair<std::string, bsoncxx>>& v) noexcept
    {
        CoreDocument core_document;
        for (auto& [k, v] : v) {
            core_document[k] = v.operator realm::bson::Bson();
        }
        m_document = std::make_shared<CoreDocument>(std::move(core_document));
    }
    bsoncxx::document::document(const document& v)
    {
        m_document = v.m_document;
    }
    bsoncxx::document::document(document&& v)
    {
        m_document = std::move(v.m_document);
    }
    bsoncxx::document::~document()
    {

    }
    bsoncxx::document& bsoncxx::document::operator=(const document& v)
    {
        m_document = v.m_document;
        return *this;
    }
    bsoncxx::document& bsoncxx::document::operator=(document&& v)
    {
        m_document = std::move(v.m_document);
        return *this;
    }
    bsoncxx::document::document(CoreDocument& doc) noexcept
    {
        m_document = std::make_shared<CoreDocument>(doc);
    }

    void bsoncxx::document::insert(const std::string& key, const bsoncxx& v)
    {
        m_document->operator[](key) = v.operator realm::bson::Bson();
    }

    void bsoncxx::document::pop_back()
    {
        m_document->pop_back();
    };

    bool bsoncxx::document::empty() const
    {
        return m_document->empty();
    };

    size_t bsoncxx::document::size() const
    {
        return m_document->size();
    };

    bsoncxx::document::value bsoncxx::document::operator[](const std::string& key)
    {
        return bsoncxx::document::value(m_document, key);
    }

    bsoncxx::document::operator CoreDocument() const
    {
        return *m_document;
    }

    /*
    ===========================
    bsoncxx::document::iterator
    ===========================
    */

    bsoncxx::document::iterator bsoncxx::document::begin()
    {
        return bsoncxx::document::iterator(m_document, 0);
    }

    bsoncxx::document::iterator bsoncxx::document::end()
    {
        return bsoncxx::document::iterator(m_document, m_document->size());
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
        m_reg_expr = std::make_shared<realm::bson::RegularExpression>(v);
    }
    bsoncxx::regular_expression::regular_expression(const std::string& pattern, const std::string& options)
    {
        m_reg_expr = std::make_shared<realm::bson::RegularExpression>(pattern, options);
    }
    bsoncxx::regular_expression::regular_expression(const std::string& pattern, option options)
    {
        m_reg_expr = std::make_shared<realm::bson::RegularExpression>(pattern, static_cast<realm::bson::RegularExpression::Option>(options));
    }
    bsoncxx::regular_expression::regular_expression()
    {
        m_reg_expr = std::make_shared<realm::bson::RegularExpression>();
    }

    bsoncxx::regular_expression::operator realm::bson::RegularExpression() const
    {
        return *m_reg_expr;
    }

    static_assert((int)realm::bson::RegularExpression::Option::None == (int)bsoncxx::regular_expression::option::none);
    static_assert((int)realm::bson::RegularExpression::Option::Multiline == (int)bsoncxx::regular_expression::option::multiline);
    static_assert((int)realm::bson::RegularExpression::Option::IgnoreCase == (int)bsoncxx::regular_expression::option::ignore_case);
    static_assert((int)realm::bson::RegularExpression::Option::Extended == (int)bsoncxx::regular_expression::option::extended);
    static_assert((int)realm::bson::RegularExpression::Option::Dotall == (int)bsoncxx::regular_expression::option::dotall);

    bsoncxx::regular_expression::option bsoncxx::regular_expression::get_options() const
    {
        return static_cast<bsoncxx::regular_expression::option>(m_reg_expr->options());
    }

    std::string bsoncxx::regular_expression::get_pattern() const
    {
        return m_reg_expr->pattern();
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
