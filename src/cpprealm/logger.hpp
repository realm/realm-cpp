#ifndef REALM_LOGGER_HPP
#define REALM_LOGGER_HPP

#include <string>

namespace realm {
    struct logger {
        /// Specifies criticality when passed to log(). Functions as a criticality
        /// threshold when returned from LevelThreshold::get().
        ///
        ///     error   Be silent unless when there is an error.
        ///     warn    Be silent unless when there is an error or a warning.
        ///     info    Reveal information about what is going on, but in a
        ///             minimalistic fashion to avoid general overhead from logging
        ///             and to keep volume down.
        ///     detail  Same as 'info', but prioritize completeness over minimalism.
        ///     debug   Reveal information that can aid debugging, no longer paying
        ///             attention to efficiency.
        ///     trace   A version of 'debug' that allows for very high volume
        ///             output.
        enum class level { all = 0,
                           trace = 1,
                           debug = 2,
                           detail = 3,
                           info = 4,
                           warn = 5,
                           error = 6,
                           fatal = 7,
                           off = 8 };
        virtual void do_log(level, const std::string &) = 0;
    };
    void set_default_logger(std::shared_ptr<struct logger>&&);
    void set_default_level_threshold(const logger::level&);
}

#endif//REALM_LOGGER_HPP
