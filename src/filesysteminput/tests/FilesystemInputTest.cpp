#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <FilesystemInput/FilesystemInput.hpp>

using namespace testing;

class FilesystemInput : public Test {
public:
  using ConfigMap = std::unordered_map<std::string, std::variant<std::string, int>>;

  std::filesystem::path directory{ std::filesystem::temp_directory_path() / "fswatcher" };
  const std::string file_type_{ "xml"};
  const std::string invalid_file_type_{ "txt" };
  const ConfigMap config_map_{
    {"inputDirectory", std::variant<std::string, int>{directory.string()}},
    {"fileTypes", std::variant<std::string, int>{"dat, xml"}}
  };
  bool callback_called{ false };
  int callback_counter{ 0 };
  bool already_stopped{ false };
  std::unique_ptr<filesysteminput::FilesystemInput> fsi;

  void SetUp() override {
    if (std::filesystem::exists(directory)) {
      std::filesystem::remove_all(directory);
    }

    std::filesystem::create_directories(directory);

    callback_counter = 0;

    fsi = std::make_unique<filesysteminput::FilesystemInput>(config_map_, [this](const std::string& filename, const std::string& content) {
      callback_called = true;
      ++callback_counter;
    });
  }

  void TearDown() override {
    stopFSI();

    if (std::filesystem::exists(directory)) {
      std::filesystem::remove_all(directory);
    }
  }

  void startFSI() {
    fsi->start();
    already_stopped = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  void stopFSI() {
    if (!already_stopped) {
      fsi->stop();
      already_stopped = true;
    }
  }
};

TEST_F(FilesystemInput, CallsCallbackIfFileIsAddedToTheConfiguredDirectory) {
  startFSI();
  {
    std::ofstream data{ std::filesystem::path{directory / ("dummy." + file_type_) } };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_TRUE(callback_called);
}

TEST_F(FilesystemInput, DoesNotCallsCallbackWhenStopped) {
  startFSI();
  {
    std::ofstream data{ std::filesystem::path{directory / ("dummy." + file_type_) } };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_TRUE(callback_called);

  callback_called = false;

  stopFSI();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  {
    std::ofstream data{ std::filesystem::path{directory / ("dummy2." + file_type_)} };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_FALSE(callback_called);
}

TEST_F(FilesystemInput, CallbackIsCalledForFilesThatAreAlreadyInTheDirectory) {

  {
    for (int i = 0; i < 5; ++i) {
      std::string filename{ "dummy_" + std::to_string(i) + "." + file_type_};
      std::ofstream data{ std::filesystem::path{directory / filename} };
    }
  }

  startFSI();

  ASSERT_TRUE(callback_called);
  ASSERT_THAT(callback_counter, Eq(5));
}

TEST_F(FilesystemInput, DoesNotCallCallbackIfInvalidFiletypeIsAddedToTheConfiguredDirectory) {
  startFSI();
  {
    std::ofstream data{ std::filesystem::path{directory / ("dummy." + invalid_file_type_) } };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_FALSE(callback_called);
  ASSERT_THAT(callback_counter, Eq(0));
}

TEST_F(FilesystemInput, DoesNotCallCallbackIfInvalidFiletypesAreAlreadyInTheDirectory) {

  {
    for (int i = 0; i < 5; ++i) {
      std::string filename{ "dummy_" + std::to_string(i) + "." + invalid_file_type_ };
      std::ofstream data{ std::filesystem::path{directory / filename} };
    }
  }

  startFSI();

  ASSERT_FALSE(callback_called);
  ASSERT_THAT(callback_counter, Eq(0));
}

TEST_F(FilesystemInput, CollectReturnsListWithAllConfiguredMetrics) {
  startFSI();

  auto collection = fsi->calculateMetrics();

  ASSERT_FALSE(collection.empty());
  ASSERT_THAT(collection.size(), Eq(3));

}