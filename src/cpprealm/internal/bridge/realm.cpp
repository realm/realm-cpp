#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/scheduler.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/object-store/util/scheduler.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<312, sizeof(Realm::Config)>{});

    realm::realm(std::shared_ptr<Realm> v)
    : m_realm(std::move(v)){}

    void realm::begin_transaction() const {
        m_realm->begin_transaction();
    }

    void realm::commit_transaction() const {
        m_realm->commit_transaction();
    }

    struct internal_scheduler : util::Scheduler {
        internal_scheduler(const std::shared_ptr<scheduler>& s)
        : m_scheduler(s)
        {
        }

        ~internal_scheduler() override = default;
        void invoke(util::UniqueFunction<void ()> &&fn) override {
            m_scheduler->invoke([&fn]() {
                fn();
            });
        }

        bool is_on_thread() const noexcept override {
            return m_scheduler->is_on_thread();
        }
        bool is_same_as(const util::Scheduler *other) const noexcept override {
            return this == other;
        }

        bool can_invoke() const noexcept override {
            return m_scheduler->can_invoke();
        }
        std::shared_ptr<scheduler> m_scheduler;
    };

    realm::config::config(const std::string &path, const std::shared_ptr<scheduler>& scheduler, const struct schema &s) {
        RealmConfig config;
        config.path = path;
        config.scheduler = std::make_shared<internal_scheduler>(scheduler);
        config.schema = s;
    }

    schema realm::schema() const {
        return m_realm->schema();
    }
}