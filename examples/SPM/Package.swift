// swift-tools-version: 5.6
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

let package = Package(
    name: "SPMRealmCxx",
    platforms: [.macOS(.v11)],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
        .package(path: "../../")
    ],
    targets: [
        // Targets are the basic building blocks of a package. A target can define a module or a test suite.
        // Targets can depend on other targets in this package, and on products in packages this package depends on.
        .executableTarget(
            name: "SPMRealmCxx",
            dependencies: [
                .product(name: "realm-cpp-sdk", package: "realm-cpp")
            ],
            cxxSettings: cxxSettings)
    ],
    cxxLanguageStandard: .cxx17
)
