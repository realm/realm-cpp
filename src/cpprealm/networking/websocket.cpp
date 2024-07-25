#include <cpprealm/networking/websocket.hpp>
#include <cpprealm/internal/networking/utils.hpp>

#include <realm/object-store/sync/generic_network_transport.hpp>
#include <realm/sync/network/default_socket.hpp>
#include <realm/util/platform_info.hpp>

namespace realm::networking {

    struct default_websocket_observer_core : public ::realm::sync::WebSocketObserver {
        default_websocket_observer_core(std::unique_ptr<websocket_observer>&& o) : m_observer(std::move(o)) { }
        ~default_websocket_observer_core() = default;

        void websocket_connected_handler(const std::string& protocol) override {
            m_observer->websocket_connected_handler(protocol);
        }

        void websocket_error_handler() override {
            m_observer->websocket_error_handler();
        }

        bool websocket_binary_message_received(util::Span<const char> data) override {
            return m_observer->websocket_binary_message_received(std::string_view(data.data(), data.size()));
        }

        bool websocket_closed_handler(bool was_clean, ::realm::sync::websocket::WebSocketError error_code,
                                      std::string_view message) override {
            return m_observer->websocket_closed_handler(was_clean, static_cast<websocket_err_codes>(error_code), message);
        }

    private:
        std::unique_ptr<websocket_observer> m_observer;
    };

    struct default_timer : public default_socket_provider::timer {
        default_timer(std::unique_ptr<::realm::sync::SyncSocketProvider::Timer>&& t) : m_timer(std::move(t)) {}
        ~default_timer() = default;
        void cancel() {
            m_timer->cancel();
        };

    private:
        std::unique_ptr<::realm::sync::SyncSocketProvider::Timer> m_timer;
    };

    /// Built in websocket client.
    struct default_socket : public websocket_interface {
        default_socket(std::unique_ptr<::realm::sync::WebSocketInterface>&&);
        ~default_socket() = default;

        void async_write_binary(std::string_view data, websocket_interface::FunctionHandler&& handler) override;
    private:
        std::unique_ptr<::realm::sync::WebSocketInterface> m_ws_interface;
    };

    default_socket_provider::default_socket_provider() {
        initialize();
    }

    default_socket_provider::default_socket_provider(const configuration& c) : m_configuration(c) {
        initialize();
    }

    void default_socket_provider::initialize() {
        auto user_agent_binding_info = std::string("RealmCpp/") + std::string(REALMCXX_VERSION_STRING);
        auto user_agent = util::format("RealmSync/%1 (%2) %3", REALM_VERSION_STRING, util::get_platform_info(),
                                       user_agent_binding_info);
        m_provider = std::make_shared<::realm::sync::websocket::DefaultSocketProvider>(util::Logger::get_default_logger(), user_agent);
    }

    std::unique_ptr<websocket_interface> default_socket_provider::connect(std::unique_ptr<websocket_observer> o, websocket_endpoint&& ep) {

        auto core_ep = internal::networking::to_core_websocket_endpoint(ep, m_configuration);
        auto ws_interface = m_provider->connect(std::make_unique<default_websocket_observer_core>(std::move(o)), std::move(core_ep));
        return std::make_unique<default_socket>(std::move(ws_interface));
    }

    void default_socket_provider::post(default_socket_provider::FunctionHandler&& fn) {
        m_provider->post([fn = std::move(fn)](::realm::Status s) {
            fn(s);
        });
    }

    default_socket_provider::sync_timer default_socket_provider::create_timer(std::chrono::milliseconds delay, FunctionHandler&& fn) {
        return std::make_unique<default_timer>(m_provider->create_timer(delay, [fn = std::move(fn)](::realm::Status s) {
            fn(s);
        }));
    }

    default_socket::default_socket(std::unique_ptr<::realm::sync::WebSocketInterface>&& ws) {
        m_ws_interface = std::move(ws);
    }

    void default_socket::async_write_binary(std::string_view s, websocket_interface::FunctionHandler&& fn) {
        m_ws_interface->async_write_binary(s, [fn = std::move(fn)](::realm::Status s) {
            fn(s);
        });
    }
}
