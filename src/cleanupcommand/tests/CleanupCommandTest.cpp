#include <filesystem>
#include <gmock/gmock.h>
#include <fstream>
#include <Core/Message.hpp>
#include <CleanupCommand/CleanupCommand.hpp>

#include "CleanupCommand/VersionInfo_CleanupCommand.h"

using namespace testing;

class CleanupCommand : public Test {
public: 
  void SetUp() override {
    if (std::filesystem::exists(directory)) {
      std::filesystem::remove_all(directory);
    }
    std::filesystem::create_directories(directory);

    cloned_cmd_.reset(cmd_.clone(config_));
  }

  void TearDown() override {
    if (std::filesystem::exists(directory)) {
      std::filesystem::remove_all(directory);
    }
  }

  void CreateDummyFile(std::filesystem::path filepath) {
    std::ofstream tempFile{ filepath };
    tempFile.close();
  }

  const std::string COMMAND_NAME = "CleanupCommand";
  const size_t METRICS_SIZE = 3;
  
  std::filesystem::path directory{ std::filesystem::temp_directory_path() / "cleanup" };

  commands::commandsConfig config_{};

  commands::CleanupCommand cmd_;
  std::unique_ptr<commands::Command> cloned_cmd_;

  std::string file{"File.dat"};
  core::MessagePtr msg = std::make_unique<core::Message>("File.dat", "Test Content");
  core::MessagePtr msg_file_not_exists = std::make_unique<core::Message>("Invalid_File.dat", "Test Content");
};

TEST_F(CleanupCommand, CommandReturnsCorrectName) {
  cloned_cmd_.reset(cmd_.clone(config_));

  ASSERT_THAT(cloned_cmd_->name(), COMMAND_NAME);
}

TEST_F(CleanupCommand, CommandReturnsCorrectVersion) {
  EXPECT_THAT(cloned_cmd_->version(), PRODUCT_VERSION_FULL_STR);
}

TEST_F(CleanupCommand, ExceptionIsThrownIfFileDoesNotExist) {
  cloned_cmd_.reset(cmd_.clone(config_));
  ASSERT_THROW(msg_file_not_exists = cloned_cmd_->execute(std::move(msg_file_not_exists)), std::filesystem::filesystem_error);
}

TEST_F(CleanupCommand, CommandDeletesFileOnExecution) {
  std::filesystem::path file_path{ directory / file };
  CreateDummyFile(file_path);
  ASSERT_TRUE(std::filesystem::exists(file_path));

  msg = std::make_unique<core::Message>(file_path.string(), "Test Content");

  cloned_cmd_.reset(cmd_.clone(config_));
  msg = cloned_cmd_->execute(std::move(msg));

  ASSERT_FALSE(std::filesystem::exists(file_path));
}

TEST_F(CleanupCommand, MessageCountIsIncrementedAfterSuccesfullExecution) {
  std::filesystem::path file_path{ directory / file };
  msg = std::make_unique<core::Message>(file_path.string(), "Test Content");

  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
  CreateDummyFile(file_path);
  msg = cloned_cmd_->execute(std::move(msg));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(1));
  CreateDummyFile(file_path);
  msg = cloned_cmd_->execute(std::move(msg));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(2));
}

TEST_F(CleanupCommand, MessageCountIsNotIncrementedIfExecutionFails) {
  cloned_cmd_.reset(cmd_.clone(config_));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
  ASSERT_THROW(msg_file_not_exists = cloned_cmd_->execute(std::move(msg_file_not_exists)), std::filesystem::filesystem_error);
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
}