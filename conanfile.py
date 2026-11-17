from hashlib import sha256
from os.path import join
from pathlib import Path

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import copy

class ECSSServicesRecipe(ConanFile):
    name = "ecss-services"
    version = "1.2"
    revision_mode = "scm"

    # Optional metadata
    license = "MIT"
    author = "SpaceDot - AcubeSAT, acubesat.obc@spacedot.gr"
    url = "gitlab.com/acubesat/obc/ecss-services"
    description = "ECSS Services implementation for the AcubeSAT nanosatellite"
    topics = ("satellite", "acubesat", "ecss", "ecss-services")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "platform_definitions_path": ["ANY"]}
    default_options = {"shared": False, "fPIC": True, "platform_definitions_path": "inc/Platform/x86/"}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "inc/*"
    generators = "CMakeDeps"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def package_id(self):
        # The library is built against the headers in platform_definitions_path, so hash them into the package ID.
        # A relative path points into this recipe's own sources, which the recipe revision already covers.
        path = Path(str(self.info.options.platform_definitions_path))
        if path.is_absolute():
            headers = b"".join(str(header).encode() + header.read_bytes() for header in sorted(path.rglob("*.hpp")))
            self.info.options.platform_definitions_path = f"{path} {sha256(headers).hexdigest()}"

    def layout(self):
        cmake_layout(self, build_folder=".")
        self.cpp.source.includedirs = ["inc"]

    def generate(self):
        tc = CMakeToolchain(self)
        if self.settings.arch != 'armv7':
            tc.variables["X86_BUILD"] = True
        tc.variables["PLATFORM_DEFINITIONS_PATH"] = self.options.platform_definitions_path

        # Instead of prefixing all cmake presets with "conan", we prefix them with the selected build directory by the
        # user, in case they are working with multiple build folders.
        tc.presets_prefix = self.build_path.parent.name

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
        self.cpp_info.libs = ["common"]
        self.cpp_info.set_property("cmake_target_name", "common")
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["inc"]

    def requirements(self):
        self.requires("etl/20.37.2", transitive_headers=True)
        self.requires("logger/1.1", transitive_headers=True)
        if self.settings.arch != 'armv7':
            self.requires("catch2/3.3.1")
