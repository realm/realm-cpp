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

extern "C" JNIEXPORT jstring JNICALL
Java_com_mongodb_realmexample_MainActivity_stringFromJNI(JNIEnv * env, jobject act, jstring jpath) {
    const char *nativeInput = env->GetStringUTFChars(jpath, NULL);
    env->ReleaseStringUTFChars(jpath, nativeInput);

    auto app = realm::App("qt-car-demo-tdbmy", std::nullopt, nativeInput);
    std::promise<void> p;
    realm::User user;
    app.login(realm::App::Credentials::anonymous(), [&](auto u, auto err) {
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

    db.write([&]() {
        db.add(Car {._id = 3, .wheelsAngle=0.0, .speed=0.0, .color=0 });
    });
    auto size = cars.size();
    std::string hello = "Number of cars: " + std::to_string(size);
    return env->NewStringUTF(hello.c_str());
}