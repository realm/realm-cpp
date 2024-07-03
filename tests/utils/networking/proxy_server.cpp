#include "proxy_server.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <array>

namespace realm::tests::utils {

    class proxy_session : public std::enable_shared_from_this<proxy_session> {
    public:
        proxy_session(tcp::socket client_socket, std::shared_ptr<asio::ssl::context> ctx, bool client_uses_ssl, bool server_uses_ssl)
            : m_client_socket(std::move(client_socket)),
              m_server_socket(m_client_socket.get_executor()),
              m_resolver(m_client_socket.get_executor()),
              m_ssl_ctx(ctx),
              m_ssl_server_socket(m_client_socket.get_executor(), *ctx),
              m_client_uses_ssl(client_uses_ssl),
              m_server_uses_ssl(server_uses_ssl) {}

        void start() {
            do_read_client_request();
        }

    private:
        void do_read_client_request() {
            auto self(shared_from_this());
            m_client_socket.async_read_some(boost::asio::buffer(m_buffer),
                                            [this, self](boost::system::error_code ec, std::size_t length) {
                                                if (!ec) {
                                                    std::string request(m_buffer.data(), length);
                                                    if (request.substr(0, 7) == "CONNECT") {
                                                        handle_connect(request);
                                                    } else {
                                                        std::cerr << "Unsupported method: " << request.substr(0, 7) << std::endl;
                                                    }
                                                } else {
                                                    std::cerr << "Error reading client request: " << ec.message() << std::endl;
                                                }
                                            });
        }

        void handle_connect(const std::string &request) {
            // Extract the host and port from the CONNECT request
            std::string::size_type host_start = request.find(" ") + 1;
            std::string::size_type host_end = request.find(":", host_start);
            std::string host = request.substr(host_start, host_end - host_start);
            std::string port = request.substr(host_end + 1, request.find(" ", host_end) - host_end - 1);

            if (host == "127.0.0.1" || host == "localhost") {
                tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), std::stoi(port));
                connect_to_server(endpoint, host);
            } else if (host.find(server_endpoint) != std::string::npos) {
                auto self(shared_from_this());
                m_resolver.async_resolve(host, port,
                                         [this, self, host](boost::system::error_code ec, tcp::resolver::results_type results) {
                                             if (!ec) {
                                                 connect_to_server(*results.begin(), host);
                                             } else {
                                                 std::cerr << "Error resolving host: " << ec.message() << std::endl;
                                             }
                                         });
            } else {
                // BAASAAS endpoint
                tcp::endpoint endpoint(boost::asio::ip::make_address(host), std::stoi(port));
                connect_to_server(endpoint, host);
            }
        }

        void connect_to_server(const tcp::endpoint &endpoint, const std::string &host) {
            if (m_server_uses_ssl) {
                // Handle SSL connection
                auto self(shared_from_this());
                m_ssl_server_socket.lowest_layer().async_connect(endpoint,
                                                                 [this, self, host](boost::system::error_code ec) {
                                                                     if (!ec) {
                                                                         do_ssl_handshake_to_server(host);
                                                                     } else {
                                                                         std::cerr << "Error connecting to server: " << ec.message() << std::endl;
                                                                     }
                                                                 });
            } else {
                // Handle non-SSL connection
                auto self(shared_from_this());
                m_server_socket.async_connect(endpoint,
                                              [this, self](boost::system::error_code ec) {
                                                  if (!ec) {
                                                      do_write_connect_response();
                                                  } else {
                                                      std::cerr << "Error connecting to server: " << ec.message() << std::endl;
                                                  }
                                              });
            }
        }

        void do_ssl_handshake_to_server(const std::string &hostname) {
            auto self(shared_from_this());

            m_ssl_server_socket.set_verify_callback(asio::ssl::host_name_verification(hostname));
            m_ssl_server_socket.set_verify_mode(boost::asio::ssl::verify_peer);

            // Set SNI Hostname (many hosts need this to handshake successfully)
            if (!SSL_set_tlsext_host_name(m_ssl_server_socket.native_handle(), hostname.c_str())) {
                throw std::runtime_error("Could not set hostname");
            }

            m_ssl_server_socket.async_handshake(asio::ssl::stream_base::client,
                                                [this, self](boost::system::error_code ec) {
                                                    if (!ec) {
                                                        do_write_connect_response();
                                                    } else {
                                                        std::cerr << "Error performing SSL handshake: " << ec.message() << std::endl;
                                                    }
                                                });
        }

        void do_write_connect_response() {
            auto self(shared_from_this());
            asio::async_write(m_client_socket, asio::buffer(std::string("HTTP/1.1 200 Connection Established\r\n\r\n")),
                              [this, self](boost::system::error_code ec, std::size_t) {
                                  if (!ec) {
                                      do_read_client();
                                  } else {
                                      std::cerr << "Error writing connect response: " << ec.message() << std::endl;
                                  }
                              });
        }

        void do_ssl_write_to_server(std::size_t length) {
            auto self(shared_from_this());
            boost::asio::async_write(m_ssl_server_socket, boost::asio::buffer(std::string(m_client_buffer.data()), length),
                                     [this, self](boost::system::error_code ec, std::size_t) {
                                         if (!ec) {
                                             do_ssl_read_server();
                                         } else {
                                             std::cerr << "Error writing to server: " << ec.message() << std::endl;
                                             m_client_socket.close();
                                             m_server_socket.close();
                                         }
                                     });
        }

        void do_ssl_read_server() {
            auto self(shared_from_this());
            m_ssl_server_socket.async_read_some(boost::asio::buffer(m_server_buffer),
                                                [this, self](boost::system::error_code ec, std::size_t length) {
                                                    if (!ec) {
                                                        do_write_to_client(length);
                                                    } else {
                                                        std::cerr << "Error reading from server (SSL): " << ec.message() << std::endl;
                                                        m_client_socket.close();
                                                        m_server_socket.close();
                                                    }
                                                });
        }

        void do_read_client() {
            auto self(shared_from_this());
            m_client_socket.async_read_some(boost::asio::buffer(m_client_buffer),
                                            [this, self](boost::system::error_code ec, std::size_t length) {
                                                if (!ec) {
                                                    auto req = std::string(m_client_buffer.data(), length);
                                                    std::cout << "Client to Server: " << req << std::endl;
                                                    if (m_server_uses_ssl) {
                                                        do_ssl_write_to_server(length);
                                                    } else {
                                                        do_write_to_server(length);
                                                    }
                                                } else {
                                                    std::cerr << "Error reading from client: " << ec.message() << std::endl;
                                                    m_client_socket.close();
                                                    m_server_socket.close();
                                                }
                                            });
        }

        void handle_http_req_res() {
            auto length = m_client_socket.read_some(boost::asio::buffer(m_client_buffer));
            boost::asio::write(m_server_socket, boost::asio::buffer(std::string(m_client_buffer.data()), length));
            length = m_server_socket.read_some(boost::asio::buffer(m_server_buffer));

            auto res = std::string(m_server_buffer.data(), length);
            bool upgrade_to_websocket = res.find("HTTP/1.1 101 Switching Protocols") != std::string::npos;
            auto bytes_written = boost::asio::write(m_client_socket, boost::asio::buffer(std::string(m_server_buffer.data()), length));
            if (upgrade_to_websocket) {
                upgrade_client_to_websocket(bytes_written);
            }
        }

        void do_read_from_websocket_client() {
            auto self(shared_from_this());
            m_client_socket.async_read_some(boost::asio::buffer(m_client_buffer),
                                            [this, self](boost::system::error_code ec, std::size_t length) {
                                                if (!ec) {
                                                    do_write_to_websocket_server(length);
                                                    do_read_from_websocket_client();
                                                } else {
                                                    // Handle error
                                                }
                                            });
        }

        void do_write_to_websocket_server(std::size_t length) {
            auto self(shared_from_this());
            boost::asio::async_write(m_server_socket, boost::asio::buffer(m_client_buffer.data(), length),
                                     [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                         if (!ec) {
                                             do_read_from_websocket_client();
                                         } else {
                                             // Handle error
                                         }
                                     });
        }

        void do_read_from_websocket_server() {
            auto self(shared_from_this());
            m_server_socket.async_read_some(boost::asio::buffer(m_server_buffer),
                                            [this, self](boost::system::error_code ec, std::size_t length) {
                                                if (!ec) {
                                                    do_write_to_websocket_client(length);
                                                    do_read_from_websocket_server();
                                                } else {
                                                    // Handle error
                                                }
                                            });
        }

        void do_write_to_websocket_client(std::size_t length) {
            auto self(shared_from_this());
            boost::asio::async_write(m_client_socket, boost::asio::buffer(m_server_buffer.data(), length),
                                     [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                         if (!ec) {
                                             do_read_from_websocket_server();
                                         } else {
                                             // Handle error
                                         }
                                     });
        }


        void upgrade_client_to_websocket(std::size_t length) {
            auto self(shared_from_this());
            do_read_from_websocket_client();
            do_read_from_websocket_server();
        }

        void do_write_to_server(std::size_t length) {
            auto self(shared_from_this());
            boost::asio::async_write(m_server_socket, boost::asio::buffer(std::string(m_client_buffer.data()), length),
                                     [this, self](boost::system::error_code ec, std::size_t) {
                                         if (!ec) {
                                             do_read_server();
                                         } else {
                                             std::cerr << "Error writing to server: " << ec.message() << std::endl;
                                             m_client_socket.close();
                                             m_server_socket.close();
                                         }
                                     });
        }

        void do_read_server() {
            auto self(shared_from_this());
            m_server_socket.async_read_some(boost::asio::buffer(m_server_buffer),
                                            [this, self](boost::system::error_code ec, std::size_t length) {
                                                if (!ec) {
                                                    do_write_to_client(length);
                                                } else {
                                                    std::cerr << "Error reading from server: " << ec.message() << std::endl;
                                                    m_client_socket.close();
                                                    m_server_socket.close();
                                                }
                                            });
        }

        void do_write_to_client(std::size_t length) {
            auto self(shared_from_this());
            auto res = std::string(m_server_buffer.data(), length);
            std::cout << "Server to Client: " << res << std::endl;
            bool upgrade_to_websocket = res.find("HTTP/1.1 101 Switching Protocols") != std::string::npos;

            boost::asio::async_write(m_client_socket, boost::asio::buffer(std::string(m_server_buffer.data()), length),
                                     [this, self, upgrade_to_websocket](boost::system::error_code ec, std::size_t bytes_written) {
                                         if (!ec) {
                                             std::cout << "BYTES WRITTEN: " << bytes_written << std::endl;
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
                                             std::cerr << "Error writing to client: " << ec.message() << std::endl;
                                             m_client_socket.close();
                                             m_server_socket.close();
                                         }
                                     });
        }

        bool m_client_uses_ssl = false;
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
    };

    proxy_server::proxy_server(const config &cfg) : m_config(cfg), m_strand(m_io_context) {
        m_acceptor = std::make_unique<tcp::acceptor>(m_io_context, tcp::endpoint(tcp::v4(), m_config.port));
        asio::ssl::context ctx(asio::ssl::context::tlsv13_client);

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
                                            std::make_shared<proxy_session>(std::move(socket), m_ssl_ctx, m_config.client_uses_ssl, m_config.server_uses_ssl)->start();
                                        }
                                        do_accept();
                                    }));
    }
} // namespace realm::tests::utils