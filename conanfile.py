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

    def source(self):
        # This small hack might be useful to guarantee proper /MT /MD linkage
        # in MSVC if the packaged project doesn't have variables to set it
        # properly
        tools.replace_in_file("CMakeLists.txt", "project(cs_signal)",
                              '''project(cs_signal)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()''')

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include", src="src")
        self.copy("*CsSignal.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["CsSignal"]

