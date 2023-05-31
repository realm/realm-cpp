#ifndef REALM_SYNC_TEST_UTILS_HPP
#define REALM_SYNC_TEST_UTILS_HPP

#include <cpprealm/sdk.hpp>

#include <realm/object-store/sync/sync_session.hpp>
#include <realm/object-store/sync/sync_manager.hpp>
#include <realm/object-store/sync/sync_user.hpp>

namespace test {
    inline std::future<void> wait_for_sync_uploads(const realm::user& user) {
        auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
        auto session = sync_sessions[0];

        std::promise<void> p;
        std::future<void> f = p.get_future();
        session->wait_for_upload_completion([p = std::move(p)](realm::Status &&ec) mutable {
            if (ec != realm::Status::OK()) p.set_exception(std::make_exception_ptr(ec));
            else p.set_value();
        });

        return f;
    }

    inline std::future<void> wait_for_sync_downloads(const realm::user &user) {
        auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
        auto session = sync_sessions[0];
        std::promise<void> p;
        std::future<void> f = p.get_future();
        session->wait_for_download_completion([p = std::move(p)](realm::Status&& ec) mutable {
            if (ec != realm::Status::OK()) p.set_exception(std::make_exception_ptr(ec));
            else p.set_value();
        });

        return f;
    }
}

#endif //REALM_SYNC_TEST_UTILS_HPP
