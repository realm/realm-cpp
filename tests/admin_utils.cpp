//
// Created by Jason Flax on 9/20/22.
//
#include "admin_utils.hpp"

Admin::Session Admin::Session::shared = RealmServer::shared.login();