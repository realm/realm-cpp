#include <jni.h>
#include <string>
#include <cpprealm/sdk.hpp>

class Car : public realm::object
{
public:
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

// Realm specific vars
realm::notification_token token;
Car m_car;
std::unique_ptr<realm::db<Car>> db;

// JNI specific vars
jobject global_ref;
JavaVM* jvm;

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_setupRealm(JNIEnv * env, jobject act, jstring jraw_path) {
    // Get a global ref to the activity so that we can post notification callbacks to it.
    global_ref = env->NewGlobalRef(act);
    env->GetJavaVM(&jvm);

    const char *raw_path = env->GetStringUTFChars(jraw_path, NULL);
    env->ReleaseStringUTFChars(jraw_path, raw_path);
    auto path = std::string(raw_path);

    auto app = realm::App("qt-car-demo-tdbmy", std::nullopt, path);
    realm::User user = app.login(realm::App::Credentials::anonymous()).get_future().get();
    db = std::make_unique<realm::db<Car>>(realm::open<Car>(user.flexible_sync_configuration()));
    db->subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
        if (!subs.find("car")) {
            subs.add<Car>("car", [](auto& c) {
                return c._id == 0;
            });
        }
    }).get_future().get();

    db->refresh(); // Bring Realm up to date after initial data sync.
    auto cars = db->objects<Car>();

    // Car with _id of 0 must already exist in Atlas.
    m_car = *cars[0];

    token = m_car.observe<Car>([&](auto changes) {
        JNIEnv* jnv;
        jvm->AttachCurrentThread(&jnv, NULL);
        std::string str;
        for (auto& change : changes.property_changes) {
            str += change.name + " ";
        }
        str += "changed.";
        jstring jstr = jnv->NewStringUTF(str.c_str());
        jnv->CallVoidMethod(global_ref, jnv->GetMethodID(jnv->GetObjectClass(global_ref), "carNotificationRecieved", "(Ljava/lang/String;)V" ), jstr);
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_moveForward(JNIEnv * env, jobject act) {
    db->write([]() {
        m_car.speed += 1;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_moveBack(JNIEnv * env, jobject act) {
    db->write([]() {
        m_car.speed -= 1;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_moveLeft(JNIEnv * env, jobject act) {
    db->write([]() {
        m_car.wheelsAngle -= 20;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_moveRight(JNIEnv * env, jobject act) {
    db->write([]() {
        m_car.wheelsAngle += 20;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_changeColor(JNIEnv * env, jobject act) {
    db->write([&]() {
        if (m_car.color < 5) {
            m_car.color += 1;
        } else {
            m_car.color = 0;
        }
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_resetCar(JNIEnv * env, jobject act) {
    db->write([&]() {
        m_car.color = 0;
        m_car.wheelsAngle = 0;
        m_car.speed = 0;
    });
}