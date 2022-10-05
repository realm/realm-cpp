//
// Created by Jason Flax on 9/20/22.
//
#include "admin_utils.hpp"

RealmServer RealmServer::shared = RealmServer();
Admin::Session Admin::shared() {
    if (!_shared) {
        Admin::_shared = RealmServer::shared.login();
    }
    return *_shared;
}
