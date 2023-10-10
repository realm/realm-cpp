#ifndef CPPREALM_ALPHA_SUPPORT_HPP
#define CPPREALM_ALPHA_SUPPORT_HPP

#if defined(__clang__)
#define CPP_REALM_ENABLE_ALPHA_SDK
#elif defined(__GNUC__) && __GNUC__ > 10
#define CPP_REALM_ENABLE_ALPHA_SDK
#endif

#endif//CPPREALM_ALPHA_SUPPORT_HPP