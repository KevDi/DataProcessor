#include <filesystem>
#include <gmock/gmock.h>
#include <fstream>
#include <Core/Message.hpp>
#include <ErrorCommand/ErrorCommand.hpp>

#include "ErrorCommand/VersionInfo_ErrorCommand.h"

using namespace testing;

class ErrorCommand : public Test {
public: 
  void SetUp() override {
    cloned_cmd_.reset(cmd_.clone(config_));
  }

  void TearDown() override {

  }
  const std::string COMMAND_NAME = "ErrorCommand";
    const size_t METRICS_SIZE = 2;

  commands::commandsConfig config_{};

  commands::ErrorCommand cmd_;
  std::unique_ptr<commands::Command> cloned_cmd_;
  core::MessagePtr msg = std::make_unique<core::Message>("1", "Test Content");
};

TEST_F(ErrorCommand, CommandReturnsCorrectName) {
  cloned_cmd_.reset(cmd_.clone(config_));

  ASSERT_THAT(cloned_cmd_->name(), COMMAND_NAME);
}

TEST_F(ErrorCommand, CommandReturnsCorrectVersion) {
  EXPECT_THAT(cloned_cmd_->version(), PRODUCT_VERSION_FULL_STR);
}

TEST_F(ErrorCommand, MessageCountIsIncrementedAfterSuccesfullExecution) {
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
  msg = cloned_cmd_->execute(std::move(msg));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(1));
  msg = cloned_cmd_->execute(std::move(msg));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(2));
}