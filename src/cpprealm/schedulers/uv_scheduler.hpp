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

#ifndef CPPREALM_UV_SCHEDULER_HPP
#define CPPREALM_UV_SCHEDULER_HPP

#include <cpprealm/scheduler.hpp>

#if __has_include(<uv.h>) && defined(CPPREALM_USE_UV)
#include <uv.h>

namespace realm {

    class invocation_queue {
    public:
        void push(std::function<void()>&&);
        void invoke_all();

    private:
        std::mutex m_mutex;
        std::vector<std::function<void()>> m_functions;
    };

    void invocation_queue::push(std::function<void()>&& fn)
    {
        std::lock_guard lock(m_mutex);
        m_functions.push_back(std::move(fn));
    }

    void invocation_queue::invoke_all()
    {
        std::vector<std::function<void()>> functions;
        {
            std::lock_guard lock(m_mutex);
            functions.swap(m_functions);
        }
        for (auto&& fn : functions) {
            fn();
        }
    }

    class uv_scheduler final : public realm::scheduler {
    public:
        uv_scheduler(uv_loop_t* loop)
            : m_handle(std::make_unique<uv_async_t>()) {
            int err = uv_async_init(loop, m_handle.get(), [](uv_async_t *handle) {
                if (!handle->data) {
                    return;
                }
                auto &data = *static_cast<struct data *>(handle->data);
                if (data.close_requested) {
                    uv_close(reinterpret_cast<uv_handle_t *>(handle), [](uv_handle_t *handle) {
                        delete reinterpret_cast<struct data *>(handle->data);
                        delete reinterpret_cast<uv_async_t *>(handle);
                    });
                } else {
                    data.queue.invoke_all();
                }
            });
            if (err < 0) {
                throw std::runtime_error("uv_async_init failed");
            }
            m_handle->data = new data;
        }

        ~uv_scheduler() {
            if (m_handle && m_handle->data) {
                static_cast<struct data *>(m_handle->data)->close_requested = true;
                uv_async_send(m_handle.get());
                // Don't delete anything here as we need to delete it from within the event loop instead
                m_handle.release();
            }
        }

        bool is_on_thread() const noexcept override {
            return m_id == std::this_thread::get_id();
        }
        bool is_same_as(const scheduler *other) const noexcept override {
            auto o = dynamic_cast<const uv_scheduler *>(other);
            return (o && (o->m_id == m_id));
        }
        bool can_invoke() const noexcept override {
            return true;
        }

        void invoke(std::function<void()> &&fn) override {
            auto &data = *static_cast<struct data *>(m_handle->data);
            data.queue.push(std::move(fn));
            uv_async_send(m_handle.get());
        }

    private:
        struct data {
            invocation_queue queue;
            std::atomic<bool> close_requested = {false};
        };
        std::unique_ptr<uv_async_t> m_handle;
        std::thread::id m_id = std::this_thread::get_id();
    };

    std::shared_ptr<scheduler> make_uv_scheduler(uv_loop_t* run_loop) {
        return std::make_shared<uv_scheduler>(run_loop);
    }
}; // namespace realm
#endif // __has_include(<uv.h>) && defined(CPPREALM_USE_UV)

#endif //CPPREALM_UV_SCHEDULER_HPP
