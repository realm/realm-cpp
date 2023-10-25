// swift-tools-version:5.6
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let coreVersion = Version("13.23.1")

var cxxSettings: [CXXSetting] = [
    .headerSearchPath("."),
    .headerSearchPath("../"),
    .define("REALM_ENABLE_SYNC", to: "1"),

    .define("REALM_DEBUG", .when(configuration: .debug)),
    .define("REALM_NO_CONFIG"),
    .define("REALM_INSTALL_LIBEXECDIR", to: ""),
    .define("REALM_ENABLE_ASSERTIONS", to: "1"),
    .define("REALM_ENABLE_ENCRYPTION", to: "1"),

    .define("REALMCXX_VERSION_MAJOR", to: "0"),
    .define("REALMCXX_VERSION_MINOR", to: "4"),
    .define("REALMCXX_VERSION_PATCH", to: "0"),
    .define("REALMCXX_VERSION_STRING", to: "\"0.4.0\""),
    // Realm Core
    .define("REALM_VERSION_MAJOR", to: String(coreVersion.major)),
    .define("REALM_VERSION_MINOR", to: String(coreVersion.minor)),
    .define("REALM_VERSION_PATCH", to: String(coreVersion.patch)),
    .define("REALM_VERSION_EXTRA", to: "\"\(coreVersion.prereleaseIdentifiers.first ?? "")\""),
    .define("REALM_VERSION_STRING", to: "\"\(coreVersion)\""),
]

let testCxxSettings: [CXXSetting] = cxxSettings + [
    // Command-line `swift build` resolves header search paths
    // relative to the package root, while Xcode resolves them
    // relative to the target root, so we need both.
    .headerSearchPath("../src"),
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
        .linkedFramework("CFNetwork", .when(platforms: applePlatforms)),
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
            type: .dynamic,
            targets: ["realm-cpp-sdk"]),
    ],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
        .package(url: "https://github.com/realm/realm-core.git", revision: "ab8eaa654fe2ecd52dea215bcab613cd7f7659d8")
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
            name: "realm-beta-tests",
            dependencies: ["realm-cpp-sdk", "Catch2"],
            path: "tests",
            exclude: [
                "experimental/sync",
                "experimental/alpha",
            ],
            sources: [
                "experimental",
                "main.hpp",
                "main.cpp",
            ],
            resources: [
                .copy("setup_baas.rb"),
                .copy("dependencies.list"),
                .copy("config_overrides.json")],
            cxxSettings: testCxxSettings + [
                .define("REALM_DISABLE_METADATA_ENCRYPTION")
            ]
        ),
        .executableTarget(
            name: "helloworld",
            dependencies: ["realm-cpp-sdk"],
            path: "examples/cmake",
            cxxSettings: cxxSettings + [
                .define("REALM_DISABLE_METADATA_ENCRYPTION")
            ]
        )
    ],
    cxxLanguageStandard: .cxx17
)
