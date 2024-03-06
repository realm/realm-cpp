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

#ifndef CPPREALM_LOGGER_HPP
#define CPPREALM_LOGGER_HPP

#include <memory>
#include <string>

namespace realm {

    struct logger {
        /// Specifies criticality when passed to log().
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
        virtual inline ~logger() noexcept = default;
        void set_level_threshold(level l) {
            m_level_threshold = l;
        }

        level get_level_threshold() const {
            return m_level_threshold;
        }
    protected:
        level m_level_threshold;
    };

    void set_default_logger(std::shared_ptr<struct logger> &&);
}

#endif//CPPREALM_LOGGER_HPP
