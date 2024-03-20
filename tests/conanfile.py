import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.build import can_run


class helloTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def is_darwin(self):
        return self.settings.os == "Macos" or self.settings.os == "iOS" or self.settings.os == "watchOS"
        
    def requirements(self):
        self.requires(self.tested_reference_str)
        self.requires("libuv/1.48.0")
        if not self.is_darwin():
            self.requires("openssl/3.2.0")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["USES_CONAN"] = "ON"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "cpprealm_db_tests")
            self.run(cmd, env="conanrun")