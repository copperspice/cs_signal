from conans import ConanFile, CMake, tools


class CsSignalConan(ConanFile):
    name = "cs_signal"
    version = "1.2.3"
    license = "BSD2"
    author = "Paul M. Bendixen paulbendixen@gmail.com"
    url = ""
    description = "A library for thread aware Signal/Slot delivery"
    topics = ("CopperSpice", "Signal", "Slot")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    generators = "cmake"
    exports_sources = "*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def validate(self):
        tools.check_min_cppstd(self,"17")

    def _configure(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure()
        cmake.build()

    def package(self):
        cmake = self._configure()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["CsSignal"]

