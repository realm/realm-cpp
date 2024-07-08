#ifndef CPPREALM_PROXY_SERVER_HPP
#define CPPREALM_PROXY_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace realm::tests::utils {
    class proxy_server {
    public:
        struct config {
            short port = 1234;
            bool server_uses_ssl = false;
        };

        proxy_server(const config &cfg);
        ~proxy_server();

        enum event {
            connect,
            ssl_handshake,
            client,
            nonssl,
            ssl,
            websocket_upgrade,
            websocket,
        };

        // Sets an event handler callback
        void set_callback(std::function<void(event)> fn) {
            m_event_handler = std::move(fn);
        }
    private:
        void do_accept();
        boost::asio::io_context m_io_context;
        std::unique_ptr<tcp::acceptor> m_acceptor;
        std::shared_ptr<asio::ssl::context> m_ssl_ctx;
        config m_config;
        std::mutex m_mutex;
        std::thread m_io_thread;
        asio::io_context::strand m_strand;
        std::function<void(event)> m_event_handler;
    };
}

#endif //CPPREALM_PROXY_SERVER_HPP
