#ifndef REALM_MAIN_HPP
#define REALM_MAIN_HPP

#include <catch2/catch_all.hpp>
#include <cpprealm/sdk.hpp>

struct realm_path {
    static std::string gen_random(const int len) {
        static const char alphanum[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
        std::string tmp_s;
        tmp_s.reserve(len);

        for (int i = 0; i < len; ++i) {
            tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
        }

        return tmp_s;
    }

    std::string path = gen_random(32);
    operator std::string() const {
        return path;
    }
    ~realm_path() {
        std::filesystem::remove_all(path + ".realm.management");
        std::filesystem::remove(path + ".realm");
        std::filesystem::remove(path + ".realm.lock");
        std::filesystem::remove(path + ".realm.note");
    }
};

int main(int argc, char* argv[]);

#endif //REALM_MAIN_HPP
