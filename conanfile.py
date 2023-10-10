from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.scm import Git
from conan.tools.files import copy

required_conan_version = ">=1.54.0"


class PoptConan(ConanFile):
    build_policy = "missing"
    name = "popt"
    description = "This is the popt(3) command line option parsing library. While it is similar to getopt, it contains a number of enhancements"
    topics = ("popt", "parsing", "command-line")
    package_type = "library"
    license = "MIT"
    homepage = "https://github.com/rpm-software-management/popt"
    url = "https://github.com/rpm-software-management/popt.git"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "doc": [True, False]
    }
    exports_sources = (
        "CMakeLists.txt",
        "src/*",
        "doc/*",
        "tests/*",
        "README",
        "cmake/*",
        "popt.3"
    )
    generators = "CMakeDeps"
    default_options = {
        "shared": False,
        "fPIC": True,
        "doc": False
    }

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_SHARED_LIBS"] = "ON" if self.options.shared else "OFF"
        tc.variables["BUILD_DOC"] = "ON" if self.options.doc else "OFF"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "*.h", self.source_folder, self.package_folder)
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["popt"]
        if self.settings.os == "Macos":
            self.cpp_info.system_libs.append("iconv")

    def requirements(self):
        if self.options.doc:
            self.build_requires("doxygen/1.9.4")
