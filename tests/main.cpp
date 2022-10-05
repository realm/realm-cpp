#define CATCH_CONFIG_RUNNER

//#include <catch2/catch.hpp>
#include <catch2/catch_all.hpp>
#include "admin_utils.hpp"

 void write_to_devnull() {
    /* open /dev/null for writing */
    int fd = open("/dev/null", O_WRONLY);

    dup2(fd, 1);    /* make stdout a copy of fd (> /dev/null) */
    dup2(fd, 2);    /* ...and same with stderr */
    close(fd);      /* close fd */
}

int main(int argc, char* argv[]) {
    auto args = std::set<std::string>(argv, argv + argc);
    auto contains_tag = false;
    auto requires_admin_server = false;
    for (auto& arg : args) {
        if (arg.front() == '[' && arg.back() == ']') {
            contains_tag = true;
            if (arg == "[flx]" || arg == "[app]") {
                requires_admin_server = true;
            }
        }
    }
    std::vector<pid_t> pids;
    if (!contains_tag || requires_admin_server) {
        RealmServer::setup();

        if (auto mongo_pid = fork()) {
            if (auto server_pid = fork()) {
                if (auto user_pid = fork()) {
                    // parent
                    pids.push_back(mongo_pid);
                    pids.push_back(server_pid);
                    pids.push_back(user_pid);
                } else {
                    // third child
                    write_to_devnull();
                    RealmServer::launch_add_user_process();
                    return 0;
                }
            } else {
                // second child
                write_to_devnull();
                RealmServer::launch_server_process();
                return 0;
            }
        } else {
            // first child
            write_to_devnull();
            RealmServer::launch_mongo_process();
            return 0;
        }
        RealmServer::wait_for_server_to_start();
        RealmServer::launch_add_user_process();
    }

    int result = Catch::Session().run(argc, argv);
    return result;
}
