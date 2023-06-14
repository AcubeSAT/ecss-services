from os.path import join

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import copy

class ECSSServicesRecipe(ConanFile):
    name = "ecss-services"
    version = "1.0"
    revision_mode = "scm"

    # Optional metadata
    license = "MIT"
    author = "SpaceDot - AcubeSAT, acubesat.obc@spacedot.gr"
    url = "gitlab.com/acubesat/obc/ecss-services"
    description = "ECSS Services implementation for the AcubeSAT nanosatellite"
    topics = ("satellite", "acubesat", "ecss", "ecss-services")

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
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, pattern="*.hpp", src=join(self.source_folder, "inc"), dst=join(self.package_folder, "inc"),
             keep_path=True)
        copy(self, pattern="*.tpp", src=join(self.source_folder, "inc"), dst=join(self.package_folder, "inc"),
             keep_path=True)
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # self.cpp_info.components["common"].libs = ["common"]
        # self.cpp_info.components["common"].set_property("cmake_target_name", "common")

        # self.cpp_info.components["common"].libdirs = ["lib"]
        # self.cpp_info.components["common"].includedirs = ["inc"]

        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["inc"]

    def requirements(self):
        self.requires("etl/20.32.1")
        self.requires("catch2/3.3.1")
        self.requires("logger/1.0")
