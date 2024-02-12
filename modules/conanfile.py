from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class WelabRecipe(ConanFile):
    name = "welab"
    version = "0.0.1"
    package_type = "library"

    # Optional metadata
    license = "MIT"
    author = "gavin"
    url = "https://github.com/hitgavin/welab"
    description = "welab cpp modules"
    topics = ("conan", "welab")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": False}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "*", "!build/*"

    def requirements(self):
        self.requires("spdlog/1.12.0")
        self.requires("fmt/10.1.1")
        self.requires("robin-hood-hashing/3.11.5")
        self.requires("pybind11/2.11.1")
        self.test_requires("gtest/1.14.0")

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")
        if self.options.get_safe("shared"):
            self.options.rm_safe("fPIC")
        self.options["spdlog/*"].header_only = True
        self.options["fmt/*"].header_only = True

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["core"]





