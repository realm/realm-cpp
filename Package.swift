// swift-tools-version:5.5
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

var cxxSettings: [CXXSetting] = [
    .headerSearchPath("."),
    .headerSearchPath("../"),
    .define("REALM_ENABLE_SYNC", to: "1"),
    .define("REALM_VERSION", to: "0.0.1"),

    .define("REALM_DEBUG", .when(configuration: .debug)),
    .define("REALM_NO_CONFIG"),
    .define("REALM_INSTALL_LIBEXECDIR", to: ""),
    .define("REALM_ENABLE_ASSERTIONS", to: "1"),
    .define("REALM_ENABLE_ENCRYPTION", to: "1"),

    .define("REALM_VERSION_MAJOR", to: "0"),
    .define("REALM_VERSION_MINOR", to: "0"),
    .define("REALM_VERSION_PATCH", to: "1"),
    .define("REALM_VERSION_EXTRA", to: "\"\""),
    .define("REALM_VERSION_STRING", to: "\"0.0.1\"")
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
        "cpprealm/internal/android"
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
            type: .dynamic,
            targets: ["realm-cpp-sdk"]),
    ],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
        .package(path: "realm-core")
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
            name: "realm-cpp-sdkTests",
            dependencies: ["realm-cpp-sdk", "Catch2"],
            path: "tests",
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
