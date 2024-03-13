#include <cpprealm/scheduler.hpp>

#include <realm/object-store/util/scheduler.hpp>
#ifdef QT_CORE_LIB
#include <QStandardPaths>
#include <QMetaObject>
#include <QTimer>
#include <QThread>
#include <QtWidgets/QApplication>
#endif

#if __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace realm {

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

#if QT_CORE_LIB
    struct QtMainLoopScheduler : public QObject, public util::Scheduler {

        bool is_on_thread() const noexcept override
        {
            return m_id == std::this_thread::get_id();
        }
        bool is_same_as(const Scheduler* other) const noexcept override
        {
            auto o = dynamic_cast<const QtMainLoopScheduler*>(other);
            return (o && (o->m_id == m_id));
        }
        bool can_deliver_notifications() const noexcept override
        {
            return QThread::currentThread()->eventDispatcher();
        }

        void set_notify_callback(Function<void()> fn) override
        {
            m_callback = std::move(fn);
        }

        void notify() override
        {
            schedule(m_callback);
        }

        void schedule(Function<void()> fn) {
            QMetaObject::invokeMethod(this, fn);
        }
    private:
        Function<void()> m_callback;
        std::thread::id m_id = std::this_thread::get_id();
    };
    static std::shared_ptr<realm::util::Scheduler> make_qt()
    {
        return std::make_shared<QtMainLoopScheduler>();
    }
#endif

    struct platform_default_scheduler final : public scheduler {
        explicit platform_default_scheduler(const std::shared_ptr<util::Scheduler>& s) : m_scheduler(s) { };
        platform_default_scheduler() = delete;
        ~platform_default_scheduler() final = default;
        void invoke(std::function<void()> &&fn) override {
            m_scheduler->invoke([f = std::move(fn)]() {
                f();
            });
        }
        [[nodiscard]] bool is_on_thread() const noexcept override {
            return m_scheduler->is_on_thread();
        }
        bool is_same_as(const realm::scheduler *other) const noexcept override {
            if (auto o = dynamic_cast<const platform_default_scheduler *>(other)) {
                return m_scheduler->is_same_as(o->m_scheduler.get());
            }
            return false;
        }
        [[nodiscard]] bool can_invoke() const noexcept override {
            return m_scheduler->can_invoke();
        }
        std::shared_ptr<util::Scheduler> m_scheduler;
    };

    std::shared_ptr<scheduler> scheduler::make_platform_default() {
#if QT_CORE_LIB
        util::Scheduler::set_default_factory(make_qt);
#endif
        return std::make_shared<platform_default_scheduler>(util::Scheduler::make_default());
    }

    std::shared_ptr<scheduler> scheduler::make_dummy_scheduler() {
        return std::make_shared<platform_default_scheduler>(util::Scheduler::make_dummy());
    }

#if QT_CORE_LIB
    std::shared_ptr<scheduler> scheduler::make_Qt_scheduler() {
        return std::make_shared<platform_default_scheduler>(make_qt());
    }
#endif

#if __ANDROID__
    std::shared_ptr<scheduler> scheduler::make_Android_ALooper_scheduler() {
        return std::make_shared<platform_default_scheduler>(util::Scheduler::make_alooper());
    }
#endif // REALM_ANDROID

}
