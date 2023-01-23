#ifndef UTILS_HPP
#define UTILS_HPP

#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <optional>
#include <typeinfo>
#include <variant>
#include <vector>
#include <realm/object-store/sync/sync_session.hpp>
#include <cpprealm/sdk.hpp>

inline std::promise<void> wait_for_sync_uploads(const realm::User& user) {
    auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
    auto session = sync_sessions[0];
    std::promise<void> p;
    session->wait_for_upload_completion([&p] (auto) { p.set_value(); });
    return p;
}

inline std::promise<void> wait_for_sync_downloads(const realm::User& user) {
    auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
    auto session = sync_sessions[0];
    std::promise<void> p;
    session->wait_for_download_completion([&p](auto) { p.set_value(); });
    return p;
}

inline void wait_for_sync(const realm::User& user) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    wait_for_sync_uploads(user).get_future().get();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    wait_for_sync_downloads(user).get_future().get();
}

inline std::ostream& operator << (std::ostream &os, const std::chrono::system_clock::time_point &tp) {
    time_t time = std::chrono::system_clock::to_time_t(tp);
    os << std::ctime(&time);
    return os;
}

template <typename T>
inline std::ostream& operator << (std::ostream &os, const std::vector<T> &v) {
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os));
    return os;
}

inline std::ostream& operator << (std::ostream &os, std::exception_ptr &error) {
    try {
        std::rethrow_exception(error);
    } catch (const std::exception &error) {
        os << "An error occurred: " << error.what();
    }
    return os;
}

template <typename T, typename ...Ts>
inline std::ostream& operator << (std::ostream &os, const std::variant<T, Ts...> &var) {
    std::visit([&] (auto &&v) { os << v; }, var);
    return os;
}

template <typename T>
inline std::ostream& operator << (std::ostream &os, const std::optional<T> &opt) {
    opt ? os << opt.value() : os << "{null}";
    return os;
}

#endif
