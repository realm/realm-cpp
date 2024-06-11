#include <cpprealm/internal/scheduler/realm_core_scheduler.hpp>
#include <cpprealm/schedulers/default_scheduler.hpp>

#include <realm/object-store/util/scheduler.hpp>

#if defined(REALM_HAVE_UV) && REALM_HAVE_UV
#include <uv.h>
#endif

namespace realm::default_scheduler {
    std::shared_ptr<scheduler> (*s_factory)() = make_platform_default;

    std::shared_ptr<scheduler> make_platform_default() {
#if REALM_PLATFORM_APPLE || REALM_ANDROID && !defined(REALM_AOSP_VENDOR)
        return std::make_shared<internal::realm_core_scheduler>(util::Scheduler::make_platform_default());
#elif defined(REALM_HAVE_UV) && REALM_HAVE_UV
        return make_uv(uv_default_loop());
#else
        return std::make_shared<realm_core_scheduler>(util::Scheduler::make_generic());
#endif
    }

    void set_default_factory(std::shared_ptr<scheduler> (*factory)()) {
        s_factory = std::move(factory);
    }

    /**
     * Create a new instance of the scheduler type returned by the default
     * scheduler factory. By default, the factory function is
     * `Scheduler::make_platform_default()`.
     */
    std::shared_ptr<scheduler> make_default()
    {
        return s_factory();
    }

#if defined(REALM_HAVE_UV) && REALM_HAVE_UV
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
        uv_scheduler(uv_loop_t* loop = uv_default_loop())
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
                static_cast<data *>(m_handle->data)->close_requested = true;
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

    std::shared_ptr<scheduler> make_uv(uv_loop_t* loop) {
        return std::make_shared<uv_scheduler>(loop);
    }
#endif

}