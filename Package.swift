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
    .unsafeFlags(["-lcurl"])
]

let cppSdkTarget: Target = .target(
    name: "realm-cpp-sdk",
    dependencies: [
        .product(name: "RealmCore", package: "realm-core"),
        .product(name: "RealmQueryParser", package: "realm-core"),
        .byNameItem(name: "libcurl", condition: .when(platforms: [.linux]))
    ],
    path: "src/",
    publicHeadersPath: ".",
    cxxSettings: cxxSettings,
    linkerSettings: [
        .linkedFramework("Foundation", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS])),
        .linkedFramework("Security", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS])),
    ])

let package = Package(
    name: "realm-cpp-sdk",
    platforms: [
        .macOS(.v10_15),
        .iOS(.v14),
        .tvOS(.v9),
        .watchOS(.v2)
    ],
    products: [
        .library(
            name: "realm-cpp-sdk",
            type: .dynamic,
            targets: ["realm-cpp-sdk"]),
    ],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
        .package(url: "https://github.com/realm/realm-core", .exact("12.7.0")),
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
        cppSdkTarget,
        .executableTarget(
            name: "realm-cpp-sdkTests",
            dependencies: ["realm-cpp-sdk", "libcurl"],
            path: "tests",
            cxxSettings: testCxxSettings + [
                .define("REALM_DISABLE_METADATA_ENCRYPTION")
            ],
            linkerSettings: [
                .linkedFramework("Foundation", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS])),
                .linkedFramework("CFNetwork", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS]))
            ]),
        .executableTarget(
            name: "helloworld",
            dependencies: ["realm-cpp-sdk"],
            path: "examples/cmake",
            cxxSettings: cxxSettings + [
                .define("REALM_DISABLE_METADATA_ENCRYPTION")
            ],
            linkerSettings: [
                .linkedFramework("Foundation", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS])),
                .linkedFramework("CFNetwork", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS]))
            ]
        )
    ],
    cxxLanguageStandard: .cxx20
)
