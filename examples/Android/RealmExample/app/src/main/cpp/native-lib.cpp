#include <jni.h>
#include <string>
#include <cpprealm/sdk.hpp>
#include <cpprealm/experimental/sdk.hpp>

namespace realm::experimental {
    struct Counter {
        primary_key<int64_t> _id;
        int64_t count;
        std::string name;
    };
    REALM_SCHEMA(Counter, _id, count, name)
}

// Realm specific vars
realm::notification_token token;
realm::experimental::managed<realm::experimental::Counter> m_counter;
std::unique_ptr<realm::experimental::db> synced_realm;

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

    auto app_config = realm::App::configuration();
    app_config.app_id = "MY_ATLAS_DEVICE_SYNC_ID";
    app_config.path = path;
    auto app = realm::App(app_config);

    realm::user user = app.login(realm::App::credentials::anonymous()).get();

    synced_realm = std::make_unique<realm::experimental::db>(realm::experimental::db(user.flexible_sync_configuration()));

    synced_realm->subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
        if (!subs.find("my_counter")) {
            subs.add<realm::experimental::Counter>("car", [](auto& obj) {
                return obj._id == 0;
            });
        }
    }).get();

    synced_realm->get_sync_session()->wait_for_download_completion().get();
    auto counters = synced_realm->objects<realm::experimental::Counter>();

    if (counters.size() == 0) {
        synced_realm->write([&]() {
            auto counter = realm::experimental::Counter();
            counter._id = 0;
            counter.count = 0;
            counter.name = "My Counter";
            synced_realm->add(std::move(counter));
        });
    }

    m_counter = realm::experimental::managed<realm::experimental::Counter>(counters[0]);

    // Changing the 'count' value manually from the MongoDB collection will also
    // trigger the observation callback.
    token = m_counter.observe([&](auto&& changes) {
        JNIEnv* jnv;
        jvm->AttachCurrentThread(&jnv, NULL);
        std::string str;
        for (auto& change : changes.property_changes) {
            str += change.name + " ";
            if (change.name == "count" && change.new_value) {
                str += std::to_string(std::get<int64_t>(*change.new_value));
            }
        }
        jstring jstr = jnv->NewStringUTF(str.c_str());
        jnv->CallVoidMethod(global_ref, jnv->GetMethodID(jnv->GetObjectClass(global_ref), "counterNotificationRecieved", "(Ljava/lang/String;)V" ), jstr);
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_incrementCounter(JNIEnv * env, jobject act) {
    synced_realm->write([&]() {
        m_counter.count += 1;
    });
}

extern "C" JNIEXPORT void JNICALL
Java_com_mongodb_realmexample_MainActivity_decrementCounter(JNIEnv * env, jobject act) {
    synced_realm->write([]() {
        m_counter.count -= 1;
    });
}