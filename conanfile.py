from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.build import check_max_cppstd, check_min_cppstd
from conan.tools.files import copy, load
from conan.tools.env import Environment
from os.path import join


class dataProcessorRecipe(ConanFile):
    name = "dataprocessor"
    # Optional metadata
    license = "MIT Lizenz"
    author = "Jan Kevin Dick"
    url = "https://github.com/KevDi/DataProcessor"
    description = "DataProcessor"
    topics = ("DataProcessor")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": True,
        "poco*:shared": True,
        "poco*:enable_activerecord":False,
        "poco*:enable_activerecord_compiler":False,
        "poco*:enable_apacheconnector":False,
        "poco*:enable_cppparser":False,
        "poco*:enable_crypto":False,
        "poco*:enable_data":False,
        "poco*:enable_data_mysql":False,
        "poco*:enable_data_odbc":False,
        "poco*:enable_data_postgresql":False,
        "poco*:enable_data_sqlite":False,
        "poco*:enable_encodings":False,
        "poco*:enable_json":False,
        "poco*:enable_jwt":False,
        "poco*:enable_mongodb":False,
        "poco*:enable_net":False,
        "poco*:enable_netssl":False,
        "poco*:enable_netssl_win":False,
        "poco*:enable_pagecompiler":False,
        "poco*:enable_pagecompiler_file2page":False,
        "poco*:enable_pdf":False,
        "poco*:enable_pocodoc":False,
        "poco*:enable_prometheus":False,
        "poco*:enable_redis":False,
        "poco*:enable_sevenzip":False,
        "poco*:enable_util":False,
        "poco*:enable_xml":False,
        "poco*:enable_zip":False,
        "prometheus-cpp*:with_pull":True,
        "prometheus-cpp*:with_push":False,
        "librdkafka*:shared":True,
        "librdkafka*:ssl":True,
    }

    generators = "CMakeDeps"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "cmake/*", "VERSION"

    def validate(self):
        check_min_cppstd(self, "17")

    def set_version(self):
        self.version = load(self, "VERSION").lower()

    def requirements(self):
        self.requires("openssl/3.1.0")
        self.requires("poco/1.13.3", transitive_headers=True, transitive_libs=True)
        self.requires("prometheus-cpp/1.1.0", transitive_headers=True)
        self.requires("nlohmann_json/3.11.3")
        self.requires("modern-cpp-kafka/2023.03.07")
        self.test_requires("gtest/1.14.0")
    
    def config_options(self):
        pass

    def layout(self):
        cmake_layout(self)

    def generate(self):
        for dep in self.dependencies.values():
            if dep.cpp_info.bindirs:
                copy(self, "*.dll", dep.cpp_info.bindirs[0], join(self.build_folder, f"bin/test/{self.settings.build_type}"))
        tc = CMakeToolchain(self)
        tc.cache_variables["ENABLE_TESTING"] = "ON"
        tc.generate()        
        env = Environment()
        env.append_path("PATH", join(self.build_folder, f"bin/{self.settings.build_type}"))
        env.vars(self).save_script("conanbuild_path")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        self.run("ctest --output-on-failure -R IT\.")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        suffix = "d" if self.settings.build_type == "Debug" else ""
        self.cpp_info.components["core"].libs = [f"Core{suffix}"]
        self.cpp_info.components["core"].set_property("cmake_target_name", "JKD::DataProcessor::Core")
        
        self.cpp_info.components["filesysteminput"].libs = [f"FilesystemInput{suffix}"]
        self.cpp_info.components["filesysteminput"].requires = ["core"]
        self.cpp_info.components["filesysteminput"].set_property("cmake_target_name", "JKD::DataProcessor::FilesystemInput")
        
        self.cpp_info.components["commands"].libs = [f"Commands{suffix}"]
        self.cpp_info.components["commands"].requires = ["core"]
        self.cpp_info.components["commands"].set_property("cmake_target_name", "JKD::DataProcessor::Commands")
        
        self.cpp_info.components["addmetadatacommand"].libs = [f"AddMetadataCommand{suffix}"]
        self.cpp_info.components["addmetadatacommand"].requires = ["core", "commands"]
        self.cpp_info.components["addmetadatacommand"].set_property("cmake_target_name", "JKD::DataProcessor::AddMetadataCommand")
        
        self.cpp_info.components["wrapcommand"].libs = [f"WrapCommand{suffix}"]
        self.cpp_info.components["wrapcommand"].requires = ["core", "commands"]
        self.cpp_info.components["wrapcommand"].set_property("cmake_target_name", "JKD::DataProcessor::WrapCommand")
		
        self.cpp_info.components["kafkasendercommand"].libs = [f"KafkaSenderCommand{suffix}"]
        self.cpp_info.components["kafkasendercommand"].requires = ["core", "commands"]
        self.cpp_info.components["kafkasendercommand"].set_property("cmake_target_name", "JKD::DataProcessor::KafkaSenderCommand")
		
        self.cpp_info.components["cleanupcommand"].libs = [f"CleanupCommand{suffix}"]
        self.cpp_info.components["cleanupcommand"].requires = ["core", "commands"]
        self.cpp_info.components["cleanupcommand"].set_property("cmake_target_name", "JKD::DataProcessor::CleanupCommand")

        self.cpp_info.components["errorcommand"].libs = [f"ErrorCommand{suffix}"]
        self.cpp_info.components["errorcommand"].requires = ["core", "commands"]
        self.cpp_info.components["errorcommand"].set_property("cmake_target_name", "JKD::DataProcessor::ErrorCommand")
        
        self.cpp_info.components["pipeline"].libs = [f"Pipeline{suffix}"]
        self.cpp_info.components["pipeline"].requires = ["core", "commands", "filesysteminput"]
        self.cpp_info.components["pipeline"].set_property("cmake_target_name", "JKD::DataProcessor::Pipeline")