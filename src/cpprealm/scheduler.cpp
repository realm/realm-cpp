#include <cpprealm/scheduler.hpp>

#include <realm/object-store/util/scheduler.hpp>
#ifdef QT_CORE_LIB
#include <QStandardPaths>
#include <QMetaObject>
#include <QTimer>
#include <QThread>
#include <QtWidgets/QApplication>
#endif

namespace realm {
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

        void set_notify_callback(std::function<void()> fn) override
        {
            m_callback = std::move(fn);
        }

        void notify() override
        {
            schedule(m_callback);
        }

        void schedule(std::function<void()> fn) {
            QMetaObject::invokeMethod(this, fn);
        }
    private:
        std::function<void()> m_callback;
        std::thread::id m_id = std::this_thread::get_id();
    };
    static std::shared_ptr<realm::util::Scheduler> make_qt()
    {
        return std::make_shared<QtMainLoopScheduler>();
    }
#endif

    struct platform_default_scheduler final : public scheduler {
        platform_default_scheduler() = default;
        ~platform_default_scheduler() final = default;
        void invoke(util::UniqueFunction<void()> &&fn) override {
            m_scheduler->invoke(std::move(fn));
        }
        [[nodiscard]] bool is_on_thread() const noexcept override {
            return m_scheduler->is_on_thread();
        }
        bool is_same_as(const realm::scheduler *other) const noexcept override {
            return this == other;
        }
        [[nodiscard]] bool can_invoke() const noexcept override {
            return m_scheduler->can_invoke();
        }
        std::shared_ptr<util::Scheduler> m_scheduler = util::Scheduler::make_default();
    };

    std::shared_ptr<scheduler> scheduler::make_default() {
#if QT_CORE_LIB
        util::Scheduler::set_default_factory(make_qt);
#endif
        return std::make_shared<platform_default_scheduler>();
    }
}
