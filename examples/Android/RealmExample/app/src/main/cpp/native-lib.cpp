#include <jni.h>
#include <string>
#include <cpprealm/sdk.hpp>

namespace realm {
    struct Car {
    public:
        primary_key<int64_t> _id;
        double wheelsAngle; // used when applying rotation
        double speed; // delta movement along the body axis
        int64_t color;
    };
    REALM_SCHEMA(Car, _id, wheelsAngle, speed, color)
}

// Realm specific vars
realm::notification_token token;
realm::managed<realm::Car> m_car;
std::unique_ptr<realm::db> synced_realm;

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
    realm::user user = app.login(realm::App::credentials::anonymous()).get();

    synced_realm = std::make_unique<realm::db>(realm::db(user.flexible_sync_configuration()));

    synced_realm->subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
        if (!subs.find("car")) {
            subs.add<realm::Car>("car", [](auto& obj) {
                return obj._id == 0;
            });
        }
    }).get();

    synced_realm->get_sync_session()->wait_for_download_completion().get();
    auto cars = synced_realm->objects<realm::Car>();

    // Car with _id of 0 must already exist in Atlas.
    m_car = realm::managed<realm::Car>(cars[0]);
    synced_realm->write([&]() {
        m_car.speed += 1.0;
    });

    token = m_car.observe([&](auto&& changes) {
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
    double c = *(m_car.speed);
    synced_realm->write([&]() {
        m_car.speed += 1.0;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_moveBack(JNIEnv * env, jobject act) {
    synced_realm->write([]() {
        m_car.speed -= 1.0;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_moveLeft(JNIEnv * env, jobject act) {
    synced_realm->write([]() {
        m_car.wheelsAngle -= 20.0;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_moveRight(JNIEnv * env, jobject act) {
    synced_realm->write([]() {
        m_car.wheelsAngle += 20.0;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_changeColor(JNIEnv * env, jobject act) {
    synced_realm->write([&]() {
        if (m_car.color < 5) {
            m_car.color += 1;
        } else {
            m_car.color = 0;
        }
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_resetCar(JNIEnv * env, jobject act) {
    synced_realm->write([&]() {
        m_car.color = 0;
        m_car.wheelsAngle = 0;
        m_car.speed = 0;
    });
}