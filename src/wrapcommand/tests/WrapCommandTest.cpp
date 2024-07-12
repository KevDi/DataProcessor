#include <gmock/gmock.h>
#include <Core/Message.hpp>
#include <WrapCommand/WrapCommand.hpp>

#include "WrapCommand/VersionInfo_WrapCommand.h"

using namespace testing;

class WrapCommand : public Test {
public: 
  void SetUp() override {
    cloned_cmd_.reset(cmd_.clone(config_));
  }

  const std::string COMMAND_NAME = "WrapCommand";
  const size_t METRICS_SIZE = 2;

  commands::commandsConfig config_{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "json"},
    {commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content"},
    {commands::WrapCommand::CFG_CMD_WRAP_METADATA, "true"}
  };

  commands::WrapCommand cmd_;
  std::unique_ptr<commands::Command> cloned_cmd_;
  core::MessagePtr msg = std::make_unique<core::Message>("1", "Test Content");
};

TEST_F(WrapCommand, CommandReturnsCorrectName) {
  cloned_cmd_.reset(cmd_.clone(config_));

  ASSERT_THAT(cloned_cmd_->name(), COMMAND_NAME);
}

TEST_F(WrapCommand, CommandReturnsCorrectVersion) {
  EXPECT_THAT(cloned_cmd_->version(), PRODUCT_VERSION_FULL_STR);
}

TEST_F(WrapCommand, ExceptionIsThrownIfWrapTypeIsMissingInConfig) {
  const commands::commandsConfig incomplete_config{
    {commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content"},
    {commands::WrapCommand::CFG_CMD_WRAP_METADATA, "true"}
  };

  cloned_cmd_.reset(cmd_.clone(incomplete_config));
  ASSERT_THROW(msg = cloned_cmd_->execute(std::move(msg)), std::invalid_argument);
}

TEST_F(WrapCommand, ExceptionIsThrownIfWrapElementIsMissingInConfig) {
  const commands::commandsConfig incomplete_config{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "json"},
    {commands::WrapCommand::CFG_CMD_WRAP_METADATA, "true"}
  };

  cloned_cmd_.reset(cmd_.clone(incomplete_config));
  ASSERT_THROW(msg = cloned_cmd_->execute(std::move(msg)), std::invalid_argument);
}

TEST_F(WrapCommand, ExceptionIsThrownIfWrapMetadataIsMissingInConfig) {
  const commands::commandsConfig incomplete_config{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "json"},
    {commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content"}
  };

  cloned_cmd_.reset(cmd_.clone(incomplete_config));
  ASSERT_THROW(msg = cloned_cmd_->execute(std::move(msg)), std::invalid_argument);
}

TEST_F(WrapCommand, ExceptionIsThrownIfWrapTypeIsNotSupported) {
  const commands::commandsConfig invalid_config{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "yaml"},
    {commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content"},
    { commands::WrapCommand::CFG_CMD_WRAP_METADATA, "true" }
  };

  cloned_cmd_.reset(cmd_.clone(invalid_config));
  ASSERT_THROW(msg = cloned_cmd_->execute(std::move(msg)), std::invalid_argument);
}

TEST_F(WrapCommand, ExceptionIsThrownIfMetadataConfigIsInvalid) {
  const commands::commandsConfig invalid_config{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "json"},
    { commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content" },
    { commands::WrapCommand::CFG_CMD_WRAP_METADATA, "all_metadata" }
  };

  cloned_cmd_.reset(cmd_.clone(invalid_config));
  ASSERT_THROW(msg = cloned_cmd_->execute(std::move(msg)), std::invalid_argument);
}

TEST_F(WrapCommand, MessageContentIsWrappedIntoJsonIfWrapTypeIsJson) {
  msg = cloned_cmd_->execute(std::move(msg));

  ASSERT_THAT(msg->content(), "{\"content\":\"Test Content\"}");
}

TEST_F(WrapCommand, MetadataIsNotAddedToContentIfMetadataSetToFalse) {
  const commands::commandsConfig config_no_metadata{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "json"},
    { commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content" },
    { commands::WrapCommand::CFG_CMD_WRAP_METADATA, "false" }
  };

  cloned_cmd_.reset(cmd_.clone(config_no_metadata));

  msg->addMetadata("metadata_1", "value_1");
  msg->addMetadata("metadata_2", "value_2");

  msg = cloned_cmd_->execute(std::move(msg));

  ASSERT_THAT(msg->content(), "{\"content\":\"Test Content\"}");
}

TEST_F(WrapCommand, MetadataIsAddedToContentIfMetadataSetToTrue) {
  msg->addMetadata("metadata_1", "value_1");
  msg->addMetadata("metadata_2", "value_2");

  msg = cloned_cmd_->execute(std::move(msg));

  ASSERT_THAT(msg->content(), "{\"metadata_1\":\"value_1\",\"metadata_2\":\"value_2\",\"content\":\"Test Content\"}");
}

TEST_F(WrapCommand, MessageCountIsIncrementedAfterSuccesfullExecution) {
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
  msg = cloned_cmd_->execute(std::move(msg));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(1));
  msg = cloned_cmd_->execute(std::move(msg));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(2));
}