// swift-tools-version:5.7

import PackageDescription

let sdkVersion = Version("0.6.1")
let coreVersion = Version("13.25.0")

var cxxSettings: [CXXSetting] = [
    .define("REALM_ENABLE_SYNC", to: "1"),

    .define("REALM_DEBUG", .when(configuration: .debug)),
    .define("REALM_NO_CONFIG"),
    .define("REALM_INSTALL_LIBEXECDIR", to: ""),
    .define("REALM_ENABLE_ASSERTIONS", to: "1"),
    .define("REALM_ENABLE_ENCRYPTION", to: "1"),

    .define("REALMCXX_VERSION_MAJOR", to: String(sdkVersion.major)),
    .define("REALMCXX_VERSION_MINOR", to: String(sdkVersion.minor)),
    .define("REALMCXX_VERSION_PATCH", to: String(sdkVersion.patch)),
    .define("REALMCXX_VERSION_STRING", to: "\"\(sdkVersion)\""),

    // Realm Core
    .define("REALM_VERSION_MAJOR", to: String(coreVersion.major)),
    .define("REALM_VERSION_MINOR", to: String(coreVersion.minor)),
    .define("REALM_VERSION_PATCH", to: String(coreVersion.patch)),
    .define("REALM_VERSION_EXTRA", to: "\"\(coreVersion.prereleaseIdentifiers.first ?? "")\""),
    .define("REALM_VERSION_STRING", to: "\"\(coreVersion)\""),
]

let applePlatforms: [Platform] = [.macOS, .macCatalyst, .iOS, .tvOS]

let cppSdkTarget: Target = .target(
    name: "realm-cpp-sdk",
    dependencies: [
        .product(name: "RealmCore", package: "realm-core"),
        .product(name: "RealmQueryParser", package: "realm-core"),
    ],
    path: "src/",
    exclude: [
        "cpprealm/internal/curl",
        "cpprealm/internal/network"
    ],
    publicHeadersPath: ".",
    cxxSettings: cxxSettings,
    linkerSettings: [
        .linkedFramework("Foundation", .when(platforms: applePlatforms)),
        .linkedFramework("Security", .when(platforms: applePlatforms)),
    ])

let package = Package(
    name: "realm-cpp-sdk",
    platforms: [
        .macOS(.v10_15),
        .macCatalyst(.v13),
        .iOS(.v13),
        .tvOS(.v13),
    ],
    products: [
        .library(
            name: "realm-cpp-sdk",
            targets: ["realm-cpp-sdk"]),
    ],
    dependencies: [
        .package(url: "https://github.com/realm/realm-core.git", exact: coreVersion)
    ],
    targets: [
        cppSdkTarget,
        .target(
            name: "Catch2Generated",
            path: "realm-core/external/generated",
            // this file was manually generated with catch v3.0.1
            // and should be regenerated when catch is upgraded
            resources: [.copy("catch2/catch_user_config.hpp")],
            publicHeadersPath: "."),
        .target(
            name: "Catch2",
            dependencies: ["Catch2Generated"],
            path: "Catch2/src",
            exclude: [
                "CMakeLists.txt",
                "catch2/catch_user_config.hpp.in",
                "catch2/internal/catch_main.cpp"
                ],
            publicHeadersPath: ".",
            cxxSettings: ([
                .headerSearchPath("catch2"),
                .define("CATCH_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS"),
                .define("CATCH_CONFIG_NO_POSIX_SIGNALS", .when(platforms: [.tvOS])),
            ] + cxxSettings) as [CXXSetting]),
        .executableTarget(
            name: "realm-tests",
            dependencies: ["realm-cpp-sdk", "Catch2"],
            path: "tests",
            exclude: [
                "sync",
            ],
            sources: [
                "main.hpp",
                "main.cpp",
            ],
            resources: [
                .copy("setup_baas.rb"),
                .copy("dependencies.list"),
                .copy("config_overrides.json")],
            cxxSettings: cxxSettings
        )
    ],
    cxxLanguageStandard: .cxx17
)
