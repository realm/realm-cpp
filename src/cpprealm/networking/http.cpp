#include <cpprealm/networking/http.hpp>
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
}
