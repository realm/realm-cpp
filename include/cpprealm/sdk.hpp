////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the >License>);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an >AS IS> BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_SDK_HPP
#define CPPREALM_SDK_HPP

#if __has_include(<cpprealm/util/config.h>)
#include <cpprealm/util/config.h>
#endif

#include <utility>

#include <cpprealm/schema.hpp>
#include <cpprealm/notifications.hpp>
#include <cpprealm/thread_safe_reference.hpp>
#include <cpprealm/rbool.hpp>

#include <cpprealm/db.hpp>
#include <cpprealm/link.hpp>
#include <cpprealm/macros.hpp>
#include <cpprealm/managed_binary.hpp>
#include <cpprealm/managed_decimal.hpp>
#include <cpprealm/managed_dictionary.hpp>
#include <cpprealm/managed_list.hpp>
#include <cpprealm/managed_mixed.hpp>
#include <cpprealm/managed_numeric.hpp>
#include <cpprealm/managed_objectid.hpp>
#include <cpprealm/managed_primary_key.hpp>
#include <cpprealm/managed_set.hpp>
#include <cpprealm/managed_string.hpp>
#include <cpprealm/managed_timestamp.hpp>
#include <cpprealm/managed_uuid.hpp>
#include <cpprealm/observation.hpp>
#include <cpprealm/results.hpp>

#endif /* CPPREALM_SDK_HPP */
