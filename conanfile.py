from conans import ConanFile, CMake, tools


class KcodecsConan(ConanFile):
    name = "kcodecs"
    version = "5.61.0"
    license = "GPLv2"
    url = "https://api.kde.org/frameworks/kcodecs/html/index.html"
    description = "KCodecs provide a collection of methods to manipulate strings using various encodings"
    settings = "os", "compiler", "build_type", "arch"
    #options = {"no_karchive": [True, False]}
    #default_options = { "no_karchive" : False }

    requires = (
        "extra-cmake-modules/[>=5.60.0]@kde/testing", # CMakeLists.txt requires 5.49.0
        "qt/[>=5.11.0]@bincrafters/stable",
        "gperf_installer/3.1@conan/stable"
    )

    generators = "cmake"

    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
     }

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.resdirs = ["share"]
