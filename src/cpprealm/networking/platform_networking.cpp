#include <cpprealm/networking/platform_networking.hpp>

#include <realm/object-store/sync/generic_network_transport.hpp>
#include <realm/sync/network/default_socket.hpp>
#include <realm/util/platform_info.hpp>

namespace realm::networking {
    std::shared_ptr<http_transport_client> (*s_http_client_factory)() = http_client_factory::make_default_http_client;
    std::optional<std::map<std::string, std::string>> http_client_factory::custom_http_headers = std::nullopt;
    std::optional<internal::bridge::realm::sync_config::proxy_config> http_client_factory::proxy_config = std::nullopt;

    std::shared_ptr<http_transport_client> http_client_factory::make_default_http_client() {
        return std::make_shared<internal::networking::DefaultTransport>(custom_http_headers, proxy_config);
    }

    void http_client_factory::set_http_client_factory(std::shared_ptr<http_transport_client> (*factory)()) {
        s_http_client_factory = std::move(factory);
    }
}

namespace realm::internal::networking {
    std::shared_ptr<realm::sync::SyncSocketProvider> default_sync_socket_provider_factory(const std::shared_ptr<util::Logger>& logger,
                                                                                          const std::string& user_agent_binding_info,
                                                                                          const std::string& user_agent_application_info,
                                                                                          const std::shared_ptr<::realm::networking::websocket_event_handler>& configuration_handler) {
        class default_sync_socket_provider_shim final : public ::realm::sync::SyncSocketProvider {
        public:
            explicit default_sync_socket_provider_shim(const std::shared_ptr<util::Logger>& logger,
                                                       const std::string& user_agent_binding_info,
                                                       const std::string& user_agent_application_info,
                                                       const std::shared_ptr<::realm::networking::websocket_event_handler>& configuration_handler) {
                auto user_agent = util::format("RealmSync/%1 (%2) %3 %4", REALM_VERSION_STRING, util::get_platform_info(), user_agent_binding_info, user_agent_application_info);
                m_default_provider = std::make_unique<::realm::sync::websocket::DefaultSocketProvider>(logger, user_agent);
                m_websocket_event_handler = configuration_handler;
            }

            ~default_sync_socket_provider_shim() = default;

            std::unique_ptr<::realm::sync::WebSocketInterface> connect(std::unique_ptr<::realm::sync::WebSocketObserver> observer,
                                                                       ::realm::sync::WebSocketEndpoint&& endpoint) override {
                if (m_websocket_event_handler) {
                    auto ep = m_websocket_event_handler->on_connect(to_websocket_endpoint(endpoint));
                    return m_default_provider->connect(std::move(observer), to_core_websocket_endpoint(std::move(ep)));
                }

                return m_default_provider->connect(std::move(observer), std::move(endpoint));
            }

            void post(FunctionHandler&& handler) override {
                m_default_provider->post(std::move(handler));
            }

            ::realm::sync::SyncSocketProvider::SyncTimer create_timer(std::chrono::milliseconds delay,
                                                                      ::realm::sync::SyncSocketProvider::FunctionHandler&& handler) override {
                return m_default_provider->create_timer(delay, std::move(handler));
            }

            void stop(bool b = false) override {
                m_default_provider->stop(b);
            }
        private:
            std::shared_ptr<::realm::networking::websocket_event_handler> m_websocket_event_handler;
            std::unique_ptr<::realm::sync::websocket::DefaultSocketProvider> m_default_provider;
        };

        return std::make_shared<default_sync_socket_provider_shim>(logger, user_agent_binding_info,
                                                                   user_agent_application_info, configuration_handler);
    }

    std::unique_ptr<::realm::sync::WebSocketInterface> create_websocket_interface_shim(std::unique_ptr<::realm::networking::websocket_interface>&& m_interface) {
        struct core_websocket_interface_shim : public ::realm::sync::WebSocketInterface {
            ~core_websocket_interface_shim() = default;
            explicit core_websocket_interface_shim(std::unique_ptr<::realm::networking::websocket_interface>&& ws) : m_interface(std::move(ws)) {}

            void async_write_binary(util::Span<const char> data, sync::SyncSocketProvider::FunctionHandler&& handler) override {
                auto handler_ptr = handler.release();
                m_interface->async_write_binary(data.data(), [ptr = std::move(handler_ptr)](::realm::networking::websocket_interface::status s) {
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

            std::unique_ptr<::realm::sync::WebSocketObserver>&& m_observer;
        };

        return std::make_unique<core_websocket_observer_shim>(std::move(m_observer));
    }

    std::unique_ptr<::realm::sync::SyncSocketProvider> create_sync_socket_provider_shim(std::unique_ptr<::realm::networking::sync_socket_provider>&& provider,
                                                                                        const std::shared_ptr<::realm::networking::websocket_event_handler>& handler) {

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
            explicit sync_socket_provider_shim(std::unique_ptr<::realm::networking::sync_socket_provider>&& provider,
                                               const std::shared_ptr<::realm::networking::websocket_event_handler>& handler) {
                m_provider = std::move(provider);
                m_websocket_event_handler = handler;
            }

            sync_socket_provider_shim() = delete;
            ~sync_socket_provider_shim() = default;

            std::unique_ptr<::realm::sync::WebSocketInterface> connect(std::unique_ptr<::realm::sync::WebSocketObserver> observer, ::realm::sync::WebSocketEndpoint&& ep) override {
                if (m_websocket_event_handler) {
                    auto ws_ep = m_websocket_event_handler->on_connect(to_websocket_endpoint(std::move(ep)));
                    return create_websocket_interface_shim(m_provider->connect(create_websocket_observer_from_core_shim(std::move(observer)), std::move(ws_ep)));
                }

                return create_websocket_interface_shim(m_provider->connect(create_websocket_observer_from_core_shim(std::move(observer)), to_websocket_endpoint(std::move(ep))));
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
            std::shared_ptr<::realm::networking::websocket_event_handler> m_websocket_event_handler;
            std::unique_ptr<::realm::networking::sync_socket_provider> m_provider;
        };

        return std::make_unique<sync_socket_provider_shim>(std::move(provider), handler);
    }
} //namespace internal::networking