#include <cpprealm/bson.hpp>

#include <realm/util/bson/bson.hpp>

namespace realm {
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
    bsoncxx::bsoncxx(const std::chrono::time_point<std::chrono::system_clock>& v) noexcept
    {
        m_bson = std::make_shared<realm::bson::Bson>(realm::bson::MongoTimestamp(v.time_since_epoch().count(), 0));
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
    }


    bsoncxx::document::document() noexcept
    {
        m_document = std::make_shared<CoreDocument>();
    }
    bsoncxx::document::document(const std::vector<std::pair<std::string, bsoncxx>>& v) noexcept
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

    bsoncxx::document::operator CoreDocument() const
    {
        return *m_document;
    }

    bsoncxx::regular_expression::operator realm::bson::RegularExpression() const
    {
        return realm::bson::RegularExpression();
    }
}
