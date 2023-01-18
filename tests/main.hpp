#ifndef REALM_MAIN_HPP
#define REALM_MAIN_HPP

#include <catch2/catch_all.hpp>
#include <cpprealm/sdk.hpp>

struct realm_path {
    template <typename T = std::mt19937>
    auto random_generator() -> T {
        auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
        auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
        auto seed = std::array<std::seed_seq::result_type, seed_len>();
        auto dev = std::random_device();
        std::generate_n(begin(seed), seed_len, std::ref(dev));
        auto seed_seq = std::seed_seq(begin(seed), end(seed));
        return T{seed_seq};
    }

    auto gen_random(std::size_t len) -> std::string {
        static constexpr auto chars =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
        thread_local auto rng = random_generator<>();
        auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
        auto result = std::string(len, '\0');
        std::generate_n(begin(result), len, [&]() { return chars[dist(rng)]; });
        return result;
    }

    std::string path = gen_random(32);
    operator std::string() const { //NOLINT(google-explicit-constructor)
        return path;
    }
    ~realm_path() {
        path = std::filesystem::current_path().append(path);
        std::filesystem::remove_all(path + ".realm.management");
        std::filesystem::remove_all(path + ".management");
        std::filesystem::remove(path);
        std::filesystem::remove(path + ".realm");
        std::filesystem::remove(path + ".realm.lock");
        std::filesystem::remove(path + ".lock");
        std::filesystem::remove(path + ".realm.note");
        std::filesystem::remove(path + ".note");
    }
};

int main(int argc, char* argv[]);

#endif //REALM_MAIN_HPP
