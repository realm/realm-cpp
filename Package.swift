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
    .headerSearchPath("../src")
]

let package = Package(
    name: "realm-cpp-sdk",
    platforms: [
        .macOS(.v10_15),
        .iOS(.v11),
        .tvOS(.v9),
        .watchOS(.v2)
    ],
    products: [
        .library(
            name: "realm-cpp-sdk",
            targets: ["realm-cpp-sdk"]),
    ],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
         .package(url: "https://github.com/realm/realm-core", from: "11.6.1"),
    ],
    targets: [
        .systemLibrary(
            name: "libcurl",
            pkgConfig: "libcurl",
            providers: [
                .apt(["libcurl4-openssl-dev"]),
                .brew(["curl"])
            ]
        ),
        .target(
            name: "realm-cpp-sdk",
            dependencies: [
                .product(name: "RealmCore", package: "realm-core"),
                .product(name: "RealmQueryParser", package: "realm-core"),
                "libcurl"
            ],
            path: "src/cpprealm",
            publicHeadersPath: ".",
            cxxSettings: cxxSettings,
            linkerSettings: [
                .linkedFramework("Foundation", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS])),
                .linkedFramework("Security", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS])),
            ]),
        .executableTarget(
            name: "realm-cpp-sdkTests",
            dependencies: ["realm-cpp-sdk", "libcurl"],
            path: "tests",
            cxxSettings: testCxxSettings + [
                .define("REALM_DISABLE_METADATA_ENCRYPTION")
            ]),
    ],
    cxxLanguageStandard: .cxx20
)
