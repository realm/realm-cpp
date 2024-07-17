#include "proxy_server.hpp"
#include "realm/util/terminate.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <array>

namespace realm::tests::utils {

    class proxy_session : public std::enable_shared_from_this<proxy_session> {
    public:
        proxy_session(tcp::socket client_socket, std::shared_ptr<asio::ssl::context> ctx, 
            bool server_uses_ssl, 
            const std::shared_ptr<std::function<void(proxy_server::event)>>& fn)
            : m_client_socket(std::move(client_socket)),
              m_server_socket(m_client_socket.get_executor()),
              m_resolver(m_client_socket.get_executor()),
              m_ssl_ctx(ctx),
              m_ssl_server_socket(m_client_socket.get_executor(), *ctx),
              m_server_uses_ssl(server_uses_ssl),
              m_event_handler(fn) {}

        void start() {
            do_read_client_request();
        }

    private:
        void do_read_client_request() {
            auto self(shared_from_this());
            m_client_socket.async_read_some(asio::buffer(m_buffer),
                                            [this, self](auto ec, std::size_t length) {
                                                if (!ec) {
                                                    std::string request(m_buffer.data(), length);
                                                    if (request.substr(0, 7) == "CONNECT") {
                                                        handle_connect(request);
                                                    } else {
                                                        REALM_TERMINATE("Unsupported HTTP method in proxy");
                                                    }
                                                } else {
                                                    if (ec == asio::error::eof) {
                                                        std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                        return;
                                                    }
                                                    REALM_TERMINATE("Error reading client request");
                                                }
                                            });
        }

        void handle_connect(const std::string &request) {
            // Extract the host and port from the CONNECT request
            std::string::size_type host_start = request.find(" ") + 1;
            std::string::size_type host_end = request.find(":", host_start);
            std::string host = request.substr(host_start, host_end - host_start);
            std::string port = request.substr(host_end + 1, request.find(" ", host_end) - host_end - 1);

            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::connect);
            }

            if (host == "127.0.0.1" || host == "localhost") {
                tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), std::stoi(port));
                connect_to_server(endpoint, host);
            } else if (host.find(server_endpoint) != std::string::npos) {
                auto self(shared_from_this());
                m_resolver.async_resolve(host, port,
                                         [this, self, host](auto ec, tcp::resolver::results_type results) {
                                             if (!ec) {
                                                 connect_to_server(*results.begin(), host);
                                             } else {
                                                 if (ec == asio::error::eof) {
                                                     std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                 }
                                             }
                                         });
            } else {
                // BAASAAS endpoint
                tcp::endpoint endpoint(asio::ip::make_address(host), std::stoi(port));
                connect_to_server(endpoint, host);
            }
        }

        void connect_to_server(const tcp::endpoint &endpoint, const std::string &host) {
            if (m_server_uses_ssl) {
                // Handle SSL connection
                auto self(shared_from_this());
                m_ssl_server_socket.lowest_layer().async_connect(endpoint,
                                                                 [this, self, host](auto ec) {
                                                                     if (!ec) {
                                                                         do_ssl_handshake_to_server(host);
                                                                     } else {
                                                                         if (ec == asio::error::eof) {
                                                                             std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                                         }
                                                                     }
                                                                 });
            } else {
                // Handle non-SSL connection
                auto self(shared_from_this());
                m_server_socket.async_connect(endpoint,
                                              [this, self](auto ec) {
                                                  if (!ec) {
                                                      do_write_connect_response();
                                                  } else {
                                                      if (ec == asio::error::eof) {
                                                          std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                      }
                                                  }
                                              });
            }
        }

        void do_ssl_handshake_to_server(const std::string &hostname) {
            auto self(shared_from_this());

            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::ssl_handshake);
            }

            m_ssl_server_socket.set_verify_callback(asio::ssl::host_name_verification(hostname));
            m_ssl_server_socket.set_verify_mode(asio::ssl::verify_peer);

            // Set SNI Hostname (many hosts need this to handshake successfully)
            if (!SSL_set_tlsext_host_name(m_ssl_server_socket.native_handle(), hostname.c_str())) {
                REALM_TERMINATE("Proxy: Could not set host name");
            }

            m_ssl_server_socket.async_handshake(asio::ssl::stream_base::client,
                                                [this, self](auto ec) {
                                                    if (!ec) {
                                                        do_write_connect_response();
                                                    } else {
                                                        REALM_TERMINATE("Proxy: Error performing SSL handshake.");
                                                    }
                                                });
        }

        void do_write_connect_response() {
            auto self(shared_from_this());
            asio::async_write(m_client_socket, asio::buffer(std::string_view("HTTP/1.1 200 Connection Established\r\n\r\n")),
                              [this, self](auto ec, std::size_t) {
                                  if (!ec) {
                                      do_read_client();
                                  } else {
                                      REALM_TERMINATE("Proxy: Error writing CONNECT response.");
                                  }
                              });
        }

        void do_ssl_write_to_server(std::size_t length) {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::ssl);
            }
            asio::async_write(m_ssl_server_socket, asio::buffer(std::string_view(m_client_buffer.data()), length),
                             [this, self](auto ec, std::size_t) {
                                 if (!ec) {
                                     do_ssl_read_server();
                                 } else {
                                     if (ec == asio::error::eof) {
                                         std::cout << "Connection closed by peer (EOF)." << std::endl;
                                     }
                                 }
                             });
        }

        void do_ssl_read_server() {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::ssl);
            }
            m_ssl_server_socket.async_read_some(asio::buffer(m_server_buffer),
                                                [this, self](auto ec, std::size_t length) {
                                                    if (!ec) {
                                                        do_write_to_client(length);
                                                    } else {
                                                        if (ec == asio::error::eof) {
                                                            std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                        }
                                                    }
                                                });
        }

        void do_read_client() {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::client);
            }
            m_client_socket.async_read_some(asio::buffer(m_client_buffer),
                                            [this, self](auto ec, std::size_t length) {
                                                if (!ec) {
                                                    auto req = std::string_view(m_client_buffer.data(), length);
                                                    std::cout << "Client to Server: " << req << std::endl;
                                                    if (m_server_uses_ssl) {
                                                        do_ssl_write_to_server(length);
                                                    } else {
                                                        do_write_to_server(length);
                                                    }
                                                } else {
                                                    if (ec == asio::error::eof) {
                                                        std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                    }
                                                }
                                            });
        }

        void do_read_from_websocket_client() {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::websocket);
            }
            m_client_socket.async_read_some(asio::buffer(m_client_buffer),
                                            [this, self](auto ec, std::size_t length) {
                                                if (!ec) {
                                                    do_write_to_websocket_server(length);
                                                    do_read_from_websocket_client();
                                                } else {
                                                    if (ec == asio::error::eof) {
                                                        std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                    }
                                                }
                                            });

        }

        void do_write_to_websocket_server(std::size_t length) {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::websocket);
            }
            if (m_server_uses_ssl) {
                asio::async_write(m_ssl_server_socket, asio::buffer(m_client_buffer.data(), length),
                                  [this, self](auto ec, std::size_t) {
                    if (!ec) {
                        do_read_from_websocket_client();
                    } else {
                        REALM_TERMINATE("Proxy: Error writing to websocket server.");
                    }
                });
            } else {
                asio::async_write(m_server_socket, asio::buffer(m_client_buffer.data(), length),
                                         [this, self](auto ec, std::size_t) {
                    if (!ec) {
                        do_read_from_websocket_client();
                    } else {
                        if (ec == asio::error::eof) {
                            std::cout << "Connection closed by peer (EOF)." << std::endl;
                        }
                    }
                });
            }
        }

        void do_read_from_websocket_server() {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::websocket);
            }
            if (m_server_uses_ssl) {
                m_ssl_server_socket.async_read_some(asio::buffer(m_server_buffer),
                                                [this, self](auto ec, std::size_t length) {
                                                    if (!ec) {
                                                        do_write_to_websocket_client(length);
                                                        do_read_from_websocket_server();
                                                    } else {
                                                        if (ec == asio::error::eof) {
                                                            std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                        }
                                                    }
                                                });
            } else {
                m_server_socket.async_read_some(asio::buffer(m_server_buffer),
                                                [this, self](auto ec, std::size_t length) {
                                                    if (!ec) {
                                                        do_write_to_websocket_client(length);
                                                        do_read_from_websocket_server();
                                                    } else {
                                                        if (ec == asio::error::eof) {
                                                            std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                        }
                                                    }
                                                });
            }
        }

        void do_write_to_websocket_client(std::size_t length) {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::websocket);
            }
            asio::async_write(m_client_socket, asio::buffer(m_server_buffer.data(), length),
                                     [this, self](auto ec, std::size_t) {
                                         if (!ec) {
                                             do_read_from_websocket_server();
                                         } else {
                                             if (ec == asio::error::eof) {
                                                 std::cout << "Connection closed by peer (EOF)." << std::endl;
                                             }
                                         }
                                     });
        }

        void upgrade_client_to_websocket(std::size_t length) {
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::websocket_upgrade);
            }
            do_read_from_websocket_client();
            do_read_from_websocket_server();
        }

        void do_write_to_server(std::size_t length) {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::nonssl);
            }
            asio::async_write(m_server_socket, asio::buffer(std::string_view(m_client_buffer.data()), length),
                                     [this, self](auto ec, std::size_t) {
                                         if (!ec) {
                                             do_read_server();
                                         } else {
                                             if (ec == asio::error::eof) {
                                                 std::cout << "Connection closed by peer (EOF)." << std::endl;
                                             }
                                         }
                                     });
        }

        void do_read_server() {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::nonssl);
            }
            m_server_socket.async_read_some(asio::buffer(m_server_buffer),
                                            [this, self](auto ec, std::size_t length) {
                                                if (!ec) {
                                                    do_write_to_client(length);
                                                } else {
                                                    if (ec == asio::error::eof) {
                                                        std::cout << "Connection closed by peer (EOF)." << std::endl;
                                                    }
                                                }
                                            });
        }

        void do_write_to_client(std::size_t length) {
            auto self(shared_from_this());
            if (m_event_handler) {
                m_event_handler->operator()(proxy_server::event::client);
            }
            auto res = std::string(m_server_buffer.data(), length);
            bool upgrade_to_websocket = res.find("HTTP/1.1 101 Switching Protocols") != std::string::npos;

            asio::async_write(m_client_socket, asio::buffer(m_server_buffer.data(), length),
                             [this, self, upgrade_to_websocket](auto ec, std::size_t bytes_written) {
                                 if (!ec) {
                                     if (upgrade_to_websocket) {
                                         upgrade_client_to_websocket(bytes_written);
                                     } else {
                                         if (m_server_uses_ssl) {
                                             do_ssl_read_server();
                                         } else {
                                             do_read_server();
                                         }
                                     }
                                 } else {
                                     if (ec == asio::error::eof) {
                                         std::cout << "Connection closed by peer (EOF)." << std::endl;
                                     }
                                 }
                             });
        }

        bool m_server_uses_ssl = false;

        tcp::socket m_client_socket;
        tcp::socket m_server_socket;
        tcp::resolver m_resolver;

        std::shared_ptr<asio::ssl::context> m_ssl_ctx;
        asio::ssl::stream<tcp::socket> m_ssl_server_socket;

        std::array<char, 8192> m_buffer;
        std::array<char, 8192> m_client_buffer;
        std::array<char, 8192> m_server_buffer;

        const std::string server_endpoint = "services.cloud.mongodb.com";
        std::shared_ptr<std::function<void(proxy_server::event)>> m_event_handler;
    };

    proxy_server::proxy_server(const config &cfg) : m_config(cfg), m_strand(m_io_context) {
        m_acceptor = std::make_unique<tcp::acceptor>(m_io_context, tcp::endpoint(tcp::v4(), m_config.port));

        m_ssl_ctx = std::make_shared<asio::ssl::context>(asio::ssl::context::tlsv13_client);
        m_ssl_ctx->set_default_verify_paths();

        do_accept();

        m_io_thread = std::thread([this] {
            m_io_context.run();
        });
    }

    proxy_server::~proxy_server() {
        m_io_context.stop();
        if (m_io_thread.joinable()) {
            m_io_thread.join();
        }
    }

    void proxy_server::do_accept() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_acceptor->async_accept(
                asio::bind_executor(m_strand,
                                    [this](std::error_code ec, asio::ip::tcp::socket socket) {
                                        if (!ec) {
                                            std::make_shared<proxy_session>(std::move(socket), m_ssl_ctx, m_config.server_uses_ssl, std::move(m_event_handler))->start();
                                        }
                                        do_accept();
                                    }));
    }
} // namespace realm::tests::utils
