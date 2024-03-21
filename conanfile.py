from conan import ConanFile
from conan.tools.build import check_min_cppstd, can_run
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.scm import Git

class cpprealmRecipe(ConanFile):
    name = "cpprealm"
    version = "1.0"

    # Optional metadata
    license = "Apache-2.0"
    url = "https://github.com/realm/realm-cpp"
    description = "Realm is a mobile database that runs directly inside phones, tablets or wearables."

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}

    def is_darwin(self):
        return self.settings.os == "Macos" or self.settings.os == "iOS" or self.settings.os == "watchOS"

    def validate(self):
        check_min_cppstd(self, "17")

    def requirements(self):
        self.requires(self.tested_reference_str)
        if not self.is_darwin() and not self.settings.os == "Emscripten":
            self.requires("zlib/1.3")
        if not self.is_darwin():
            self.requires("openssl/3.2.0")
            self.requires("libuv/1.48.0")
        if self.settings.os == "Linux":
            self.requires("libcurl/8.4.0")
    def source(self):
        git = Git(self)
        git.clone(url="https://github.com/realm/realm-cpp", target=".")
        git.folder = "."
        git.checkout(commit="316c31841b56fab7fe1f28f0aa0138e00f10a8cb")
        git.run("submodule update --init --recursive")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["USES_CONAN"] = "ON"
        tc.variables["REALM_CPP_NO_TESTS"] = "ON"
        tc.variables["REALM_CORE_SUBMODULE_BUILD"] = "ON"
        tc.variables["REALM_USE_SYSTEM_OPENSSL"] = "ON"
        if self.settings.os == "Windows":
            self.cpp_info.cxxflags = ["/Zc:preprocessor /bigobj"]
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        if self.settings.build_type == "Debug":
            self.cpp_info.libs = ["cpprealm-dbg", "realm-object-store-dbg", "realm-parser-dbg", "realm-sync-dbg", "realm-dbg"]
        else:
            self.cpp_info.libs = ["cpprealm", "realm-object-store", "realm-parser", "realm-sync", "realm"]
        if self.is_darwin():
            self.cpp_info.frameworks = ["Foundation", "Security", "Compression", "z"]
