from os.path import join

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import copy
from conan.tools.scm import Git

class ECSSServicesRecipe(ConanFile):
    name = "ecss-services"
    version = "1.0"

    # Optional metadata
    license = "MIT"
    author = "<Put your name here> <And your email here>"
    url = "gitlab.com/acubesat/obc/ecss-services"
    description = "<Description of logger package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "inc/*"
    generators = "CMakeDeps"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        self.folders.build = "cmake-build-debug"
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, pattern="*.hpp", src=join(self.source_folder, "inc"), dst=join(self.package_folder, "inc"), keep_path=False)
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["ecss-services"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["inc"]

    def requirements(self):
        self.requires("etl/20.32.1")
        self.requires("catch2/3.3.1")
        self.requires("logger/1.0")
