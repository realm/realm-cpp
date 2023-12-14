// swift-tools-version: 5.6
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

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
            ])
    ],
    cxxLanguageStandard: .cxx17
)
