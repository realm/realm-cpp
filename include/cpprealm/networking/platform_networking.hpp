////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_PLATFORM_NETWORKING_HPP
#define CPPREALM_PLATFORM_NETWORKING_HPP

#include <cpprealm/internal/bridge/status.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>

#ifndef REALMCXX_VERSION_MAJOR
#include <cpprealm/version_numbers.hpp>
#endif

namespace realm {
    namespace app {
        struct GenericNetworkTransport;
    }
    namespace sync {
        class SyncSocketProvider;
    }

    namespace util {
        class Logger;
    }
}

namespace realm::networking {

    /// The WebSocket base class that is used by the SyncClient to send data over the
    /// WebSocket connection with the server. This is the class that is returned by
    /// SyncSocketProvider::connect() when a connection to an endpoint is requested.
    /// If an error occurs while establishing the connection, the error is presented
    /// to the WebSocketObserver provided when the WebSocket was created.
    struct websocket_interface {
        /// The destructor must close the websocket connection when the WebSocket object
        /// is destroyed
        virtual ~websocket_interface() = default;

        using status = internal::bridge::status;
        using FunctionHandler = std::function<void(internal::bridge::status)>;

        /// Write data asynchronously to the WebSocket connection. The handler function
        /// will be called when the data has been sent successfully. The WebSocketOberver
        /// provided when the WebSocket was created will be called if any errors occur
        /// during the write operation.
        /// @param data A std::string_view containing the data to be sent to the server.
        /// @param handler The handler function to be called when the data has been sent
        ///                successfully or the websocket has been closed (with
        ///                ErrorCodes::OperationAborted). If an error occurs during the
        ///                write operation, the websocket will be closed and the error
        ///                will be provided via the websocket_closed_handler() function.
        virtual void async_write_binary(std::string_view data, FunctionHandler&& handler) = 0;
    };

    /// WebSocket observer interface in the SyncClient that receives the websocket
    /// events during operation.
    struct websocket_observer {
        virtual ~websocket_observer() = default;

        /// Called when the websocket is connected, i.e. after the handshake is done.
        /// The Sync Client is not allowed to send messages on the socket before the
        /// handshake is complete and no message_received callbacks will be called
        /// before the handshake is done.
        ///
        /// @param protocol The negotiated subprotocol value returned by the server
        virtual void websocket_connected_handler(const std::string& protocol) = 0;

        /// Called when an error occurs while establishing the WebSocket connection
        /// to the server or during normal operations. No additional binary messages
        /// will be processed after this function is called.
        virtual void websocket_error_handler() = 0;

        /// Called whenever a full message has arrived. The WebSocket implementation
        /// is responsible for defragmenting fragmented messages internally and
        /// delivering a full message to the Sync Client.
        ///
        /// @param data A util::Span containing the data received from the server.
        ///             The buffer is only valid until the function returns.
        ///
        /// @return bool designates whether the WebSocket object should continue
        ///         processing messages. The normal return value is true . False must
        ///         be returned if the websocket object has been destroyed during
        ///         execution of the function.
        virtual bool websocket_binary_message_received(std::string_view data) = 0;

        /// Called whenever the WebSocket connection has been closed, either as a result
        /// of a WebSocket error or a normal close.
        ///
        /// @param was_clean Was the TCP connection closed after the WebSocket closing
        ///                  handshake was completed.
        /// @param error_code The error code received or synthesized when the websocket was closed.
        /// @param message    The message received in the close frame when the websocket was closed.
        ///
        /// @return bool designates whether the WebSocket object has been destroyed
        ///         during the execution of this function. The normal return value is
        ///         True to indicate the WebSocket object is no longer valid. If False
        ///         is returned, the WebSocket object will be destroyed at some point
        ///         in the future.
        virtual bool websocket_closed_handler(bool was_clean, websocket_err_codes error_code,
                                              std::string_view message) = 0;
    };

    /// Sync Socket Provider interface that provides the event loop and WebSocket
    /// factory used by the SyncClient.
    ///
    /// All callback and event operations in the SyncClient must be completed in
    /// the order in which they were issued (via post() or timer) to the event
    /// loop and cannot be run in parallel. It is up to the custom event loop
    /// implementation to determine if these are run on the same thread or a
    /// thread pool as long as it is guaranteed that the callback handler
    /// functions are processed in order and not run concurrently.
    ///
    /// The implementation of a SyncSocketProvider must support the following
    /// operations that post handler functions (via by the Sync client) onto the
    /// event loop:
    /// * Post a handler function directly onto the event loop
    /// * Post a handler function when the specified timer duration expires
    ///
    /// The event loop is not required to be a single thread as long as the
    /// following requirements are satisfied:
    /// * handler functions are called in the order they were posted to the
    /// event loop queue, and
    /// * a handler function runs to completion before the next handler function
    /// is called.
    ///
    /// The SyncSocketProvider also provides a WebSocket interface for
    /// connecting to the server via a WebSocket connection.
    class sync_socket_provider {
    public:
        /// Function handler typedef
        using FunctionHandler = std::function<void(internal::bridge::status)>;

        /// The Timer object used to track a timer that was started on the event
        /// loop.
        ///
        /// This object provides a cancel() mechanism to cancel the timer. The
        /// handler function for this timer must be called with a Status of
        /// ErrorCodes::OperationAborted error code if the timer is canceled.
        ///
        /// Custom event loop implementations will need to create a subclass of
        /// Timer that provides access to the underlying implementation to cancel
        /// the timer.
        struct timer {
            /// Cancels the timer and destroys the timer instance.
            virtual ~timer() = default;
            /// Cancel the timer immediately. Does nothing if the timer has
            /// already expired or been cancelled.
            virtual void cancel() = 0;
        };

        /// Other class typedefs
        using SyncTimer = std::unique_ptr<sync_socket_provider::timer>;

        /// The event loop implementation must ensure the event loop is stopped and
        /// flushed when the object is destroyed. If the event loop is processed by
        /// a thread, the thread must be joined as part of this operation.
        virtual ~sync_socket_provider() = default;

        /// Create a new websocket pointed to the server indicated by endpoint and
        /// connect to the server. Any events that occur during the execution of the
        /// websocket will call directly to the handlers provided by the observer.
        /// The WebSocketObserver guarantees that the WebSocket object will be
        /// closed/destroyed before the observer is terminated/destroyed.
        virtual std::unique_ptr<websocket_interface> connect(std::unique_ptr<websocket_observer> observer,
                                                             ws_endpoint&& endpoint) = 0;

        /// Submit a handler function to be executed by the event loop (thread).
        ///
        /// Register the specified handler function to be queued on the event loop
        /// for immediate asynchronous execution. The specified handler will be
        /// executed by an expression on the form `handler()`. If the the handler
        /// object is movable, it will never be copied. Otherwise, it will be
        /// copied as necessary.
        ///
        /// This function is thread-safe and can be called by any thread. It can
        /// also be called from other post() handler function.
        ///
        /// The handler will never be called as part of the execution of post(). If
        /// post() is called on a thread separate from the event loop, the handler
        /// may be called before post() returns.
        ///
        /// Handler functions added through post() must be executed in the order
        /// they are added. More precisely, if post() is called twice to add two
        /// handlers, A and B, and the execution of post(A) ends before the
        /// beginning of the execution of post(B), then A is guaranteed to execute
        /// before B.
        ///
        /// @param handler The handler function to be queued on the event loop.
        virtual void post(FunctionHandler&& handler) = 0;

        /// Create and register a new timer whose handler function will be posted
        /// to the event loop when the provided delay expires.
        ///
        /// This is a one shot timer and the Timer class returned becomes invalid
        /// once the timer has expired. A new timer will need to be created to wait
        /// again.
        ///
        /// @param delay The duration to wait in ms before the timer expires.
        /// @param handler The handler function to be called on the event loop
        ///                when the timer expires.
        ///
        /// @return A pointer to the Timer object that can be used to cancel the
        /// timer. The timer will also be canceled if the Timer object returned is
        /// destroyed.
        virtual SyncTimer create_timer(std::chrono::milliseconds delay, FunctionHandler&& handler) = 0;
    };

    struct http_client_factory {
        static std::optional<std::map<std::string, std::string>> custom_http_headers;
        static std::optional<internal::bridge::realm::sync_config::proxy_config> proxy_config;

        static std::shared_ptr<http_transport_client> make_default_http_client();
        static void set_http_client_factory(std::shared_ptr<http_transport_client> (*factory)());
    };
}

namespace realm::internal::networking {
    std::shared_ptr<realm::sync::SyncSocketProvider> default_sync_socket_provider_factory(const std::shared_ptr<util::Logger>& logger,
                                                                                          const std::string& user_agent_binding_info,
                                                                                          const std::string& user_agent_application_info,
                                                                                          const std::shared_ptr<::realm::networking::websocket_event_handler>&);

    std::shared_ptr<app::GenericNetworkTransport> create_http_client_shim(const std::shared_ptr<::realm::networking::http_transport_client>&);
}

#endif//CPPREALM_PLATFORM_NETWORKING_HPP
