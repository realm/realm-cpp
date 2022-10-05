//
// Created by Jason Flax on 9/20/22.
//
#include "admin_utils.hpp"
#if TARGET_OS_SIMULATOR == 0 && TARGET_OS_IPHONE == 0 && TARGET_OS_WATCH == 0 && TARGET_OS_TV == 0
void write_to_devnull() {
    /* open /dev/null for writing */
    int fd = open("/dev/null", O_WRONLY);

    dup2(fd, 1);    /* make stdout a copy of fd (> /dev/null) */
    dup2(fd, 2);    /* ...and same with stderr */
    close(fd);      /* close fd */
}

RealmServer RealmServer::shared = RealmServer();
Admin::Session Admin::shared() {
    if (!_shared) {
        RealmServer::setup();

        if (fork()) {
            if (fork()) {
                if (fork()) {
                    // parent
                } else {
                    // third child
                    write_to_devnull();
                    RealmServer::launch_add_user_process();
                    abort();
                }
            } else {
                // second child
                write_to_devnull();
                RealmServer::launch_server_process();
                abort();
            }
        } else {
            // first child
            write_to_devnull();
            RealmServer::launch_mongo_process();
            abort();
        }
        RealmServer::wait_for_server_to_start();
        RealmServer::launch_add_user_process();
        Admin::_shared = RealmServer::shared.login();
    }
    return *_shared;
}
#endif
