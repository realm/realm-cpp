#include <cpprealm/networking/platform_networking.hpp>

#include <realm/object-store/sync/generic_network_transport.hpp>
#include <realm/sync/network/default_socket.hpp>
#include <realm/util/platform_info.hpp>

namespace realm::networking {
    std::shared_ptr<realm::sync::SyncSocketProvider> default_sync_socket_provider_factory(const std::shared_ptr<util::Logger>& logger,
                                                                                          const std::string& user_agent_binding_info,
                                                                                          const std::string& user_agent_application_info,
                                                                                          const std::shared_ptr<websocket_event_handler>& configuration_handler) {
        class default_sync_socket_provider_shim final : public ::realm::sync::SyncSocketProvider {
        public:
            explicit default_sync_socket_provider_shim(const std::shared_ptr<util::Logger>& logger,
                                                       const std::string& user_agent_binding_info,
                                                       const std::string& user_agent_application_info,
                                                       const std::shared_ptr<websocket_event_handler>& configuration_handler) {
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
            std::shared_ptr<websocket_event_handler> m_websocket_event_handler;
            std::unique_ptr<::realm::sync::websocket::DefaultSocketProvider> m_default_provider;
        };

        return std::make_shared<default_sync_socket_provider_shim>(logger, user_agent_binding_info,
                                                                   user_agent_application_info, configuration_handler);
    }

    std::shared_ptr<app::GenericNetworkTransport> default_http_client_factory(const std::optional<std::map<std::string, std::string>>& custom_http_headers,
                                                                              const std::optional<internal::bridge::realm::sync_config::proxy_config>& proxy_config) {
        struct core_http_transport_shim : app::GenericNetworkTransport {
            ~core_http_transport_shim() = default;
            core_http_transport_shim(const std::optional<std::map<std::string, std::string>>& custom_http_headers = std::nullopt,
                                     const std::optional<internal::bridge::realm::sync_config::proxy_config>& proxy_config = std::nullopt) {
                m_transport = internal::DefaultTransport(custom_http_headers, proxy_config);
            }

            void send_request_to_server(const app::Request& request,
                                        util::UniqueFunction<void(const app::Response&)>&& completion) {
                auto completion_ptr = completion.release();
                m_transport.send_request_to_server(to_request(request),
                                                   [f = std::move(completion_ptr)]
                                                   (const networking::response& response) {
                                                       auto uf = util::UniqueFunction<void(const app::Response&)>(std::move(f));
                                                       uf(to_core_response(response));
                                                   });
            }

            private:
                internal::DefaultTransport m_transport;
            };

            return std::make_shared<core_http_transport_shim>(custom_http_headers, proxy_config);
    };
} //namespace realm::networking