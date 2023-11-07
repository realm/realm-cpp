#include "../../main.hpp"
#include "test_objects.hpp"

namespace realm::experimental {

    TEST_CASE("string", "[str]") {
        realm_path path;
        db_config config;
        config.set_path(path);

        SECTION("managed_str_get_set", "[str]") {
            auto obj = AllTypesObject();
            auto realm = db(std::move(config));
            auto managed_obj = realm.write([&realm, &obj] {
                return realm.add(std::move(obj));
            });
            CHECK_THROWS(managed_obj.str_col = "foo");
            CHECK(managed_obj.str_col == "");
            realm.write([&managed_obj] { managed_obj.str_col = "bar"; });
            CHECK(managed_obj.str_col == "bar");
        }

        SECTION("managed_str_contains", "[str]") {
            auto obj = AllTypesObject();
            auto realm = db(std::move(config));
            auto managed_obj = realm.write([&realm, &obj] {
                obj.str_col = "foo";
                return realm.add(std::move(obj));
            });
            CHECK(managed_obj.str_col.contains("oo"));
        }
    }
}

template <typename ResultType>
struct accessor_base {
    virtual ResultType get() = 0;
    virtual void set(ResultType&& value) = 0;
};


template <typename T>
struct unmanaged /*: accessor<unmanaged<T>, T> */ {
    //    unmanaged()
    ////    : accessor<unmanaged<T>, T>(this)
    //    {}
    //    T _value;
    //    unmanaged(T&& value) :
    ////        : accessor<unmanaged<T>, T>(this),
    //                _value(std::move(value)) {}
    //    T get() {
    //        return _value;
    //    }
    //    void set(T& value) {
    //        _value = value;
    //    }
};

template <typename T>
struct managed {
    //    managed()
    ////    : accessor<managed<T>, T>(this)
    //    {};
    //    T _value;
    //    managed(T&& value) :
    ////    : accessor<managed<T>, T>(this)
    ////    ,
    //            _value(value) {}
    //
    //    T get() {
    //        return _value;
    //    }
    //    void set(T& value) {
    //        _value = value;
    //    }
};

template <typename AccessorType, typename ResultType>
struct accessor : accessor_base<ResultType> {


//    accessor(AccessorType* accessor)
//    : m_accessor(accessor)
//    {}
    template <typename T>
    struct Storage;
    template <> struct Storage<unmanaged<ResultType>> {
        ResultType result;
    };
    template <> struct Storage<managed<ResultType>> {
        ResultType result; // TODO: this would be an obj. just testing the values
    };

    Storage<AccessorType> m_accessor;

    virtual ResultType get() override {
        return m_accessor.result;
    }
    virtual void set(ResultType&& value) override {
        if constexpr (std::is_same_v<decltype(m_accessor), Storage<managed<ResultType>>>) {
            std::cout<<"Using managed accessor"<<std::endl;
        } else if constexpr (std::is_same_v<decltype(m_accessor), Storage<unmanaged<ResultType>>>) {
            std::cout<<"Using unmanaged accessor"<<std::endl;
        }
        m_accessor.result = std::move(value);
    }
};


//
//struct S : SlabBase {
//};
//

//template <bool = false>
//struct Slab {
//};

struct SlabBase {
    struct $_SlabAccessorsBase {
        accessor_base<int64_t>* _id;
        accessor_base<std::string>* name;

        $_SlabAccessorsBase(accessor_base<int64_t>* _id,
                            accessor_base<std::string>* name)
        : _id(_id)
        , name(name)
        {}
    };
    struct $_SlabAccessorsManaged : public $_SlabAccessorsBase {
        accessor<managed<int64_t>, int64_t> _id_managed;
        accessor<managed<std::string>, std::string> name_managed;

        $_SlabAccessorsManaged()
        :  _id_managed({})
              , name_managed({}), $_SlabAccessorsBase(&_id_managed, &name_managed)

        {
        }
    };
    struct $_SlabAccessorsUnmanaged : public $_SlabAccessorsBase {
        accessor<unmanaged<int64_t>, int64_t> _id_managed;
        accessor<unmanaged<std::string>, std::string> name_managed;
        $_SlabAccessorsUnmanaged()
            :  _id_managed({})
              , name_managed({}), $_SlabAccessorsBase(&_id_managed, &name_managed)

        {
        }
    };
//    $_SlabAccessors _slabAccessors;
    virtual $_SlabAccessorsBase* operator ->() = 0;
};

//
struct Slab : public SlabBase {
    static constexpr b = true;
public:
    $_SlabAccessorsUnmanaged unmanaged;
    $_SlabAccessorsBase* operator ->() override {
        return &unmanaged;
    }
};
std::function<SlabBase::$_SlabAccessorsBase*(void)> managed_accessor_factory = () {

};
//
//std::unique_ptr<int>;
//
//template <>
//struct Slab<true> : SlabBase {
//public:
//    bool is_managed = true;
//    $_SlabAccessorsManaged unmanaged;
//    $_SlabAccessorsBase* operator ->() override {
//        return &unmanaged;
//    }
//};

//Slab<true> Slab<false>::operator=(Slab<true> &&slab) {
//    return Slab<true>();
//}

void realm_add(SlabBase& slab) {
    slab = Slab<true>{};
    slab->_id = &accessor<managed<int64_t>, int64_t>{};
    slab->_id->set(84);
}

TEST_CASE("Experiment", "[str]") {
    SECTION("BLAH") {
        auto slab = Slab();
        slab->_id->set(42);
        std::cout<<slab->_id->get()<<std::endl;
        realm_add(slab);
//        slab.is_managed = true;
        std::cout<<slab->_id->get()<<std::endl;
    }
}