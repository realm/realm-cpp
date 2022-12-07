#include <jni.h>
#include <string>
#include <cpprealm/sdk.hpp>

struct Car : public realm::object
{
    realm::persisted<int> _id;
    realm::persisted<double> wheelsAngle; // used when applying rotation
    realm::persisted<double> speed; // delta movement along the body axis
    realm::persisted<int> color;

    static constexpr auto schema = realm::schema("Car",
                                                 realm::property<&Car::_id, true>("_id"),
                                                 realm::property<&Car::wheelsAngle>("wheelsAngle"),
                                                 realm::property<&Car::speed>("speed"),
                                                 realm::property<&Car::color>("color")
    );
};

//class Singleton
//{
//public:
//    Singleton(Singleton const&) = delete;
//    Singleton& operator=(Singleton const&) = delete;
//
//    static std::shared_ptr<Singleton> instance()
//    {
//        static std::shared_ptr<Singleton> s{new Singleton};
//        return s;
//    }
//
//    void start_realm(const std::string& path)
//    {
//        m_realm_app = std::make_unique<realm::App>("qt-car-demo-tdbmy", std::nullopt, path);
//        std::promise<void> p;
//        realm::User user;
//        m_realm_app->login(realm::App::Credentials::anonymous(), [&](auto u, auto err) {
//            if (err) {
//                // handle it
//            } else {
//                user = std::move(u);
//                p.set_value();
//            }
//        });
//        p.get_future().get();
//
//        auto db = realm::open<Car>(user.flexible_sync_configuration());
//        db.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
//            if (!subs.find("foo")) {
//                subs.add<Car>("foo"); // Subscription to get all cars
//            }
//        }).get_future().get();
//
//        auto cars = db.objects<Car>();
//
//        m_car = std::make_unique<Car>(*cars[0]);
//        token = m_car->observe<Car>([](auto) {
//            auto x = 0;
//        });
//    }
//
//private:
//    realm::notification_token token;
//    std::unique_ptr<Car> m_car;
//    std::unique_ptr<realm::App> m_realm_app;
//    Singleton() {}
//};


extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_observe(JNIEnv * env, jobject act) {


}

realm::notification_token token;
Car m_car;
std::unique_ptr<realm::App> m_realm_app;

jclass main_activity;
JNIEnv * m_env;

jobject global_ref;
JavaVM* jvm;

extern "C" JNIEXPORT jstring JNICALL
Java_com_mongodb_realmexample_MainActivity_stringFromJNI(JNIEnv * env, jobject act, jstring jraw_path) {
    const char *raw_path = env->GetStringUTFChars(jraw_path, NULL);
    env->ReleaseStringUTFChars(jraw_path, raw_path);
    auto path = std::string(raw_path);

    m_realm_app = std::make_unique<realm::App>("qt-car-demo-tdbmy", std::nullopt, path);
    std::promise<void> p;
    realm::User user;
    m_realm_app->login(realm::App::Credentials::anonymous(), [&](auto u, auto err) {
        if (err) {
            // handle it
        } else {
            user = std::move(u);
            p.set_value();
        }
    });
    p.get_future().get();

    auto db = realm::open<Car>(user.flexible_sync_configuration());
    db.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
        if (!subs.find("foo")) {
            subs.add<Car>("foo"); // Subscription to get all cars
        }
    }).get_future().get();

    auto cars = db.objects<Car>();

    auto main_activity = env->FindClass("com/mongodb/realmexample/MainActivity");
    jmethodID methodid = env->GetMethodID(main_activity, "registerErrorCallback",  "(Ljava/lang/String;)V");
//        jstring jstr = env->NewStringUTF("Some callback");
//        env->CallVoidMethod(act, methodid, jstr);

    global_ref = env->NewGlobalRef(  act );
    env->GetJavaVM( &jvm );

    m_car = *cars[0];
    token = m_car.observe<Car>([&](auto) {

        JNIEnv* jnv;
        jvm->AttachCurrentThread( &jnv, NULL );
        jstring jstr = jnv->NewStringUTF("Some callback");
        jnv->CallVoidMethod( global_ref, jnv->GetMethodID( jnv->GetObjectClass( global_ref ), "registerErrorCallback", "(Ljava/lang/String;)V" ), jstr);

        //env->CallVoidMethod(global_ref, methodid, jstr);
//        auto x = 0;
    });

    std::string hello = "Number of cars: ";
    return env->NewStringUTF(hello.c_str());
}