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

#ifndef CPPREALM_LNKLST_HPP
#define CPPREALM_LNKLST_HPP

#include <cstdlib>
#include <memory>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class LnkLst;
}
namespace realm::internal::bridge {
    struct obj;
    struct obj_key;

    struct lnklst {
        lnklst() ;
        lnklst(const lnklst& other) ;
        lnklst& operator=(const lnklst& other) ;
        lnklst(lnklst&& other);
        lnklst& operator=(lnklst&& other);
        ~lnklst();
        lnklst(const LnkLst&); // NOLINT
        operator LnkLst() const; //NOLINT
        obj create_and_insert_linked_object(size_t idx);
        void add(const obj_key&);
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::LnkLst m_lnk_lst[1];
#else
        std::shared_ptr<LnkLst> m_lnk_lst;
#endif
    };

}

#endif //CPPREALM_LNKLST_HPP
