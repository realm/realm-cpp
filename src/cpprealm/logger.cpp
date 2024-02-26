#if !defined(WIN32_LEAN_AND_MEAN) && _WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include <cpprealm/logger.hpp>
#include <realm/util/logger.hpp>

namespace realm {
    static_assert((int)util::Logger::Level::off == (int)logger::level::off);
    static_assert((int)util::Logger::Level::fatal == (int)logger::level::fatal);
    static_assert((int)util::Logger::Level::error == (int)logger::level::error);
    static_assert((int)util::Logger::Level::warn == (int)logger::level::warn);
    static_assert((int)util::Logger::Level::info == (int)logger::level::info);
    static_assert((int)util::Logger::Level::detail == (int)logger::level::detail);
    static_assert((int)util::Logger::Level::debug == (int)logger::level::debug);
    static_assert((int)util::Logger::Level::all == (int)logger::level::all);
    
    inline logger::level log_level_for_level(util::Logger::Level log_level) {
        switch (log_level) {
            case util::Logger::Level::off:
                return logger::level::off;
            case util::Logger::Level::fatal:
                return logger::level::fatal;
            case util::Logger::Level::error:
                return logger::level::error;
            case util::Logger::Level::warn:
                return logger::level::warn;
            case util::Logger::Level::info:
                return logger::level::info;
            case util::Logger::Level::detail:
                return logger::level::detail;
            case util::Logger::Level::debug:
                return logger::level::debug;
            case util::Logger::Level::trace:
                return logger::level::trace;
            case util::Logger::Level::all:
                return logger::level::all;
        }
        abort();
    }

    inline util::Logger::Level log_level_for_level(logger::level log_level) {
        switch (log_level) {
            case logger::level::off:
                return util::Logger::Level::off;
            case logger::level::fatal:
                return util::Logger::Level::fatal;
            case logger::level::error:
                return util::Logger::Level::error;
            case logger::level::warn:
                return util::Logger::Level::warn;
            case logger::level::info:
                return util::Logger::Level::info;
            case logger::level::detail:
                return util::Logger::Level::detail;
            case logger::level::debug:
                return util::Logger::Level::debug;
            case logger::level::trace:
                return util::Logger::Level::trace;
            case logger::level::all:
                return util::Logger::Level::all;
        }
        abort();
    }

    struct internal_logger : public util::Logger {
        internal_logger(std::shared_ptr<logger> &&s) {
            m_logger = std::move(s);
        }
        void do_log(util::Logger::Level level, const std::string &msg) {
            m_logger->do_log(log_level_for_level(level), msg);
        }

    private:
        std::shared_ptr<logger> m_logger;
    };

    void set_default_logger(std::shared_ptr<struct logger>&& l) {
//        util::Logger::set_default_logger(std::make_shared<internal_logger>(std::move(l)));
    }
    void set_default_level_threshold(logger::level l) {
//        util::Logger::set_default_level_threshold(log_level_for_level(l));
    }
}