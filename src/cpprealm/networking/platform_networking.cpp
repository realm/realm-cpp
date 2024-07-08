#include <cpprealm/networking/platform_networking.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

#include <realm/object-store/sync/generic_network_transport.hpp>
#include <realm/sync/network/default_socket.hpp>
#include <realm/util/platform_info.hpp>

namespace realm::networking {

    std::shared_ptr<http_transport_client> make_default_http_client() {
        return std::make_shared<networking::default_http_transport>();
    }
    std::function<std::shared_ptr<http_transport_client>()> s_http_client_factory = make_default_http_client;

    void set_http_client_factory(std::function<std::shared_ptr<http_transport_client>()>&& factory_fn) {
        s_http_client_factory = std::move(factory_fn);
    }

    std::shared_ptr<http_transport_client> make_http_client() {
        return s_http_client_factory();
    }

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
        std::shared_ptr<websocket_observer> m_observer;
    };

    struct default_timer : public default_socket_provider::timer {
        default_timer(const std::shared_ptr<::realm::sync::SyncSocketProvider::Timer>& t) : m_timer(t) {}
        ~default_timer() = default;
        void cancel() {
            m_timer->cancel();
        };

        std::shared_ptr<::realm::sync::SyncSocketProvider::Timer> m_timer;
    };

    default_socket_provider::default_socket_provider() {
        auto user_agent_binding_info = std::string("RealmCpp/") + std::string(REALMCXX_VERSION_STRING);
        auto user_agent_application_info = "";//app_id; TODO: Should we pass the app id?

        auto user_agent = util::format("RealmSync/%1 (%2) %3 %4", REALM_VERSION_STRING, util::get_platform_info(),
                                       user_agent_binding_info, user_agent_application_info);
        m_provider = std::make_unique<::realm::sync::websocket::DefaultSocketProvider>(util::Logger::get_default_logger(), user_agent);
    }

    std::unique_ptr<websocket_interface> default_socket_provider::connect(std::unique_ptr<websocket_observer> o, websocket_endpoint&& ep) {
        auto ws_interface = m_provider->connect(std::make_unique<default_websocket_observer_core>(std::move(o)),
                                                                                                  internal::networking::to_core_websocket_endpoint(ep));
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

namespace realm::internal::networking {

    std::unique_ptr<::realm::sync::WebSocketInterface> create_websocket_interface_shim(std::unique_ptr<::realm::networking::websocket_interface>&& m_interface) {
        struct core_websocket_interface_shim : public ::realm::sync::WebSocketInterface {
            ~core_websocket_interface_shim() = default;
            explicit core_websocket_interface_shim(std::unique_ptr<::realm::networking::websocket_interface>&& ws) : m_interface(std::move(ws)) {}

            void async_write_binary(util::Span<const char> data, sync::SyncSocketProvider::FunctionHandler&& handler) override {
                auto handler_ptr = handler.release();
                auto b = std::string_view(data.data(), data.size());
                m_interface->async_write_binary(b, [ptr = std::move(handler_ptr)](::realm::networking::websocket_interface::status s) {
                    auto uf = util::UniqueFunction<void(::realm::Status)>(std::move(ptr));
                    return uf(s.operator ::realm::Status());
                });
            };

            std::unique_ptr<::realm::networking::websocket_interface> m_interface;
        };

        return std::make_unique<core_websocket_interface_shim>(std::move(m_interface));
    }

    std::shared_ptr<app::GenericNetworkTransport> create_http_client_shim(const std::shared_ptr<::realm::networking::http_transport_client>& http_client) {
        struct core_http_transport_shim : app::GenericNetworkTransport {
            ~core_http_transport_shim() = default;
            core_http_transport_shim(const std::shared_ptr<::realm::networking::http_transport_client>& http_client) {
                m_http_client = http_client;
            }

            void send_request_to_server(const app::Request& request,
                                        util::UniqueFunction<void(const app::Response&)>&& completion) {
                auto completion_ptr = completion.release();
                m_http_client->send_request_to_server(to_request(request),
                                                   [f = std::move(completion_ptr)]
                                                   (const ::realm::networking::response& response) {
                                                       auto uf = util::UniqueFunction<void(const app::Response&)>(std::move(f));
                                                       uf(to_core_response(response));
                                                   });
            }

        private:
            std::shared_ptr<::realm::networking::http_transport_client> m_http_client;
        };

        return std::make_shared<core_http_transport_shim>(http_client);
    };

    std::unique_ptr<::realm::networking::websocket_observer> create_websocket_observer_from_core_shim(std::unique_ptr<::realm::sync::WebSocketObserver>&& m_observer) {
        struct core_websocket_observer_shim : public ::realm::networking::websocket_observer {
            explicit core_websocket_observer_shim(std::unique_ptr<::realm::sync::WebSocketObserver>&& ws) : m_observer(std::move(ws)) {}
            ~core_websocket_observer_shim() = default;

            void websocket_connected_handler(const std::string& protocol) override {
                m_observer->websocket_connected_handler(protocol);
            }

            void websocket_error_handler() override {
                m_observer->websocket_error_handler();
            }

            bool websocket_binary_message_received(std::string_view data) override {
                return m_observer->websocket_binary_message_received(data);
            }

            bool websocket_closed_handler(bool was_clean, ::realm::networking::websocket_err_codes error_code,
                                          std::string_view message) override {
                return m_observer->websocket_closed_handler(was_clean, static_cast<::realm::sync::websocket::WebSocketError>(error_code), message);
            }

            std::unique_ptr<::realm::sync::WebSocketObserver> m_observer;
        };

        return std::make_unique<core_websocket_observer_shim>(std::move(m_observer));
    }

    std::unique_ptr<::realm::sync::SyncSocketProvider> create_sync_socket_provider_shim(const std::shared_ptr<::realm::networking::sync_socket_provider>& provider,
                                                                                        std::optional<internal::bridge::realm::sync_config::proxy_config> pc) {
        struct sync_timer_shim final : public ::realm::sync::SyncSocketProvider::Timer {
            sync_timer_shim(std::unique_ptr<::realm::networking::sync_socket_provider::timer>&& timer) : m_timer(std::move(timer)) {}
            ~sync_timer_shim() = default;

            void cancel() override {
                m_timer->cancel();
            }

        private:
            std::unique_ptr<::realm::networking::sync_socket_provider::timer> m_timer;
        };

        struct sync_socket_provider_shim final : public ::realm::sync::SyncSocketProvider {
            explicit sync_socket_provider_shim(const std::shared_ptr<::realm::networking::sync_socket_provider>& provider,
                                               std::optional<internal::bridge::realm::sync_config::proxy_config> proxy_config) {
                m_provider = provider;
                m_proxy_config = proxy_config;
            }

            sync_socket_provider_shim() = delete;
            ~sync_socket_provider_shim() = default;

            std::unique_ptr<::realm::sync::WebSocketInterface> connect(std::unique_ptr<::realm::sync::WebSocketObserver> observer, ::realm::sync::WebSocketEndpoint&& ep) override {
                auto cpprealm_ws_interface = m_provider->connect(create_websocket_observer_from_core_shim(std::move(observer)),
                                                                 to_websocket_endpoint(std::move(ep), m_proxy_config));
                return  create_websocket_interface_shim(std::move(cpprealm_ws_interface));
            }

            void post(FunctionHandler&& handler) override {
                auto handler_ptr = handler.release();
                m_provider->post([ptr = std::move(handler_ptr)](::realm::networking::websocket_interface::status s) {
                    auto uf = util::UniqueFunction<void(::realm::Status)>(std::move(ptr));
                    return uf(s.operator ::realm::Status());
                });
            }

            ::realm::sync::SyncSocketProvider::SyncTimer create_timer(std::chrono::milliseconds delay, ::realm::sync::SyncSocketProvider::FunctionHandler&& handler) override {
                auto handler_ptr = handler.release();
                auto fn = [ptr = std::move(handler_ptr)](::realm::networking::websocket_interface::status s) {
                    auto uf = util::UniqueFunction<void(::realm::Status)>(std::move(ptr));
                    return uf(s.operator ::realm::Status());
                };
                return  std::make_unique<sync_timer_shim>(m_provider->create_timer(delay, std::move(fn)));
            }
        private:
            std::shared_ptr<::realm::networking::sync_socket_provider> m_provider;
            std::optional<internal::bridge::realm::sync_config::proxy_config> m_proxy_config;
        };

        return std::make_unique<sync_socket_provider_shim>(std::move(provider), pc);
    }
} //namespace internal::networking
