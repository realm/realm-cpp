#include <cpprealm/schedulers/uv_scheduler.hpp>

namespace realm {
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
} // namespace realm
