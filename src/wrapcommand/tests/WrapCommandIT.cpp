#include <gmock/gmock.h>
#include <Commands/CommandFactory.hpp>
#include <WrapCommand/WrapCommand.hpp>
#include <Core/Message.hpp>

using namespace testing;

static const std::string COMMAND_NAME{"WrapCommand"};

class WrapCommandIT : public Test {

public:
  void SetUp() override {
    command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<commands::WrapCommand>());
  }

  void TearDown() override {
    command_factory_.clearAllCommands();
  }

  commands::CommandFactory& command_factory_ = commands::CommandFactory::Instance();
  commands::commandsConfig config_wrap_with_metadata_{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "json"},
    { commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content" },
    { commands::WrapCommand::CFG_CMD_WRAP_METADATA, "true" }
  };
  commands::commandsConfig config_wrap_without_metadata_{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "json"},
    { commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content" },
    { commands::WrapCommand::CFG_CMD_WRAP_METADATA, "false" }
  };
  core::MessagePtr message_one_ = std::make_unique<core::Message>("1","MessageOne");
  core::MessagePtr message_two_ = std::make_unique<core::Message>("2","MessageTwo");
};

TEST_F(WrapCommandIT, CommandReturnsCorrectName) {
  auto cmd = command_factory_.allocateCommand(COMMAND_NAME, config_wrap_with_metadata_);
  ASSERT_THAT(cmd.get()->name(), Eq(COMMAND_NAME));
}

TEST_F(WrapCommandIT, MessageContentAndMetadataIsWrapped) {
  
  ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(1));

  message_one_->addMetadata("key_1", "value_1");
  message_one_->addMetadata("key_2", "value_2");
  message_one_->addMetadata("key_3", "value_3");
  message_two_->addMetadata("key_1", "value_1");
  message_two_->addMetadata("key_2", "value_2");
  message_two_->addMetadata("key_3", "value_3");

  auto wrap_with_metadata_cmd = command_factory_.allocateCommand(COMMAND_NAME, config_wrap_with_metadata_);
  ASSERT_TRUE(wrap_with_metadata_cmd);
  auto wrap_without_metadata_cmd = command_factory_.allocateCommand(COMMAND_NAME, config_wrap_without_metadata_);
  ASSERT_TRUE(wrap_without_metadata_cmd);

  auto result_one = wrap_with_metadata_cmd->execute(std::move(message_one_));
  ASSERT_TRUE(result_one);
  ASSERT_THAT(result_one->content(), R"({"key_1":"value_1","key_2":"value_2","key_3":"value_3","content":"MessageOne"})");

  auto result_two = wrap_without_metadata_cmd->execute(std::move(message_two_));
  ASSERT_TRUE(result_two);
  ASSERT_THAT(result_two->content(), R"({"content":"MessageTwo"})");
}