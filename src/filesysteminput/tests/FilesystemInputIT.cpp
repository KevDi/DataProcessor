#include <filesystem>
#include <gmock/gmock.h>
#include <FilesystemInput/FilesystemInput.hpp>
#include <fstream>
#include <sstream>

using namespace testing;

class FilesystemInputIT : public Test {
public:
  void SetUp() override {
    if (std::filesystem::exists(directory_)) {
      std::filesystem::remove_all(directory_);
    }

    if (std::filesystem::exists(source_dir_)) {
      std::filesystem::remove_all(source_dir_);
    }

    std::filesystem::create_directories(directory_);
    std::filesystem::create_directories(source_dir_);

    fsi_ = std::make_unique<filesysteminput::FilesystemInput>(config_, [this](const std::string& filename, const std::string& content) {
      actual_content_.emplace(content);
      actual_filenames_.emplace(filename);
    });
  }

  void TearDown() override {
    fsi_->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::filesystem::remove_all(directory_);
    std::filesystem::remove_all(source_dir_);
  }

  std::filesystem::path directory_{ std::filesystem::temp_directory_path() / "fs_integration_test" };
  std::filesystem::path source_dir_{ std::filesystem::temp_directory_path() / "fs_integration_test_source" };
  std::unique_ptr<filesysteminput::FilesystemInput> fsi_;
  std::set<std::string> actual_filenames_;
  std::set<std::string> actual_content_;
  filesysteminput::FilesystemInput::ConfigMap config_{
    {"inputDirectory", directory_.string()},
    { "fileTypes", std::string{"dat"} }
  };
};

TEST_F(FilesystemInputIT, DataSendToTheWatchedFolderIsProcessed) {

  std::set<std::string> expected_filenames;
  std::set<std::string> expected_content;

  fsi_->start();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  for (int i = 0; i < 10; i++) {
    std::ostringstream oss;
    oss << "test_file_" << i << ".dat";
    expected_filenames.emplace(std::filesystem::path{ directory_ / oss.str() }.string());
    auto tmp_path = std::filesystem::path{ source_dir_ / oss.str() }.string();
    {
      std::ofstream tmp_file{ std::filesystem::path{source_dir_ / oss.str()} };
      tmp_file << "content " << i;
      expected_content.emplace("content " + std::to_string(i));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::filesystem::rename(tmp_path, std::filesystem::path{ directory_ / oss.str() });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

  }

  std::this_thread::sleep_for(std::chrono::seconds(5));

  ASSERT_THAT(actual_filenames_, UnorderedElementsAreArray(expected_filenames));
  ASSERT_THAT(actual_content_, UnorderedElementsAreArray(expected_content));

}

TEST_F(FilesystemInputIT, DataThatIsAlreadyInTheWatchedFolderIsProcessed) {
  std::set<std::string> expected_filenames;
  std::set<std::string> expected_content;

  for (int i = 0; i < 10; i++) {
    std::ostringstream oss;
    oss << "test_file_" << i << ".dat";
    expected_filenames.emplace(std::filesystem::path{ directory_ / oss.str() }.string());
    std::ofstream tmp_file{ std::filesystem::path{directory_ / oss.str()} };
    tmp_file << "content " << i;
    expected_content.emplace("content " + std::to_string(i));
  }

  fsi_->start();

  std::this_thread::sleep_for(std::chrono::seconds(2));

  ASSERT_THAT(actual_filenames_, UnorderedElementsAreArray(expected_filenames));
  ASSERT_THAT(actual_content_, UnorderedElementsAreArray(expected_content));
}

TEST_F(FilesystemInputIT, DataThatIsAlreadyInTheWatchedFolderAndAddedToItAreProcessed) {
  std::set<std::string> expected_filenames;
  std::set<std::string> expected_content;

  for (int i = 0; i < 10; i++) {
    std::ostringstream oss;
    oss << "test_file_" << i << ".dat";
    expected_filenames.emplace(std::filesystem::path{ directory_ / oss.str() }.string());
    std::ofstream tmp_file{ std::filesystem::path{directory_ / oss.str()} };
    tmp_file << "content " << i;
    expected_content.emplace("content " + std::to_string(i));
  }

  fsi_->start();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  for (int i = 0; i < 10; i++) {
    std::ostringstream oss;
    oss << "test_file_added_" << i << ".dat";
    expected_filenames.emplace(std::filesystem::path{ directory_ / oss.str() }.string());
    auto tmp_path = std::filesystem::path{ source_dir_ / oss.str() }.string();
    {
      std::ofstream tmp_file{ std::filesystem::path{source_dir_ / oss.str()} };
      tmp_file << "content added" << i;
      expected_content.emplace("content added" + std::to_string(i));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::filesystem::rename(tmp_path, std::filesystem::path{ directory_ / oss.str() });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  std::this_thread::sleep_for(std::chrono::seconds(5));
  ASSERT_THAT(actual_filenames_, UnorderedElementsAreArray(expected_filenames));
  ASSERT_THAT(actual_content_, UnorderedElementsAreArray(expected_content));
}

TEST_F(FilesystemInputIT, DataThatNotMatchesTheDatatypesAreIgnored) {
  std::set<std::string> expected_filenames;
  std::set<std::string> expected_content;

  for (int i = 0; i < 10; i++) {
    std::ostringstream oss;
    oss << "test_file_" << i << ".dat";
    expected_filenames.emplace(std::filesystem::path{ directory_ / oss.str() }.string());
    std::ofstream tmp_file{ std::filesystem::path{directory_ / oss.str()} };
    tmp_file << "content " << i;
    expected_content.emplace("content " + std::to_string(i));
  }

  {
    std::ofstream tmp_file{ std::filesystem::path{directory_ / "not_valid.txt"} };
    tmp_file << "content";
  }

  fsi_->start();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  for (int i = 0; i < 10; i++) {
    std::ostringstream oss;
    oss << "test_file_added_" << i << ".dat";
    expected_filenames.emplace(std::filesystem::path{ directory_ / oss.str() }.string());
    auto tmp_path = std::filesystem::path{ source_dir_ / oss.str() }.string();
    {
      std::ofstream tmp_file{ std::filesystem::path{source_dir_ / oss.str()} };
      tmp_file << "content added " << i;
      expected_content.emplace("content added " + std::to_string(i));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::filesystem::rename(tmp_path, std::filesystem::path{ directory_ / oss.str() });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  {
    std::ofstream tmp_file{ std::filesystem::path{source_dir_ / "not_valid_added.txt"} };
    tmp_file << "content";
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::filesystem::copy_file(std::filesystem::path{ source_dir_ / "not_valid_added.txt" }, std::filesystem::path{ directory_ / "not_valid_added.txt" });

  std::this_thread::sleep_for(std::chrono::seconds(5));

  ASSERT_THAT(actual_filenames_, UnorderedElementsAreArray(expected_filenames));
  ASSERT_THAT(actual_content_, UnorderedElementsAreArray(expected_content));
}
