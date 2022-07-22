#ifndef REALM_SYNC_TEST_UTILS_HPP
#define REALM_SYNC_TEST_UTILS_HPP

#include <cpprealm/sdk.hpp>

#include <realm/object-store/sync/sync_session.hpp>

namespace test {
    realm::task<void> wait_for_sync_uploads(const realm::User& user) {
        auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
        CHECK_EQUALS(sync_sessions.size(), 1);
        auto session = sync_sessions[0];
        co_await realm::make_awaitable<std::error_code>([&] (auto cb) {
            session->wait_for_upload_completion(cb);
        });
        co_return;
    }

    realm::task<void> wait_for_sync_downloads(const realm::User& user) {
        auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
        CHECK_EQUALS(sync_sessions.size(), 1);
        auto session = sync_sessions[0];
        co_await realm::make_awaitable<std::error_code>([&] (auto cb) {
            session->wait_for_download_completion(cb);
        });
        co_return;
    }
}

#endif //REALM_SYNC_TEST_UTILS_HPP
