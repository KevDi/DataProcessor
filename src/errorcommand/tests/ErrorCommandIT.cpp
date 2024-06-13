#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <Commands/CommandFactory.hpp>
#include <ErrorCommand/ErrorCommand.hpp>
#include <Core/Message.hpp>

using namespace testing;

static const std::string COMMAND_NAME{"ErrorCommand"};

class ErrorCommandIT : public Test {

public:
  void SetUp() override {
    command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<commands::ErrorCommand>());
  }

  void TearDown() override {
    command_factory_.clearAllCommands();
  }

  commands::CommandFactory& command_factory_ = commands::CommandFactory::Instance();
  commands::commandsConfig config_{};

  core::MessagePtr message_one_ = std::make_unique<core::Message>("1", "ErrorCommand_ErrorCommandIT_MessageOne");
  core::MessagePtr message_two_ = std::make_unique<core::Message>("2", "ErrorCommand_ErrorCommandIT_MessageTwo");
};

TEST_F(ErrorCommandIT, CommandReturnsCorrectName) {
  auto cmd = command_factory_.allocateCommand(COMMAND_NAME, config_);
  ASSERT_THAT(cmd.get()->name(), Eq(COMMAND_NAME));
}

TEST_F(ErrorCommandIT, CommandCanBeAllocatedAndExecuted) {
  ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(1));

  auto cmd = command_factory_.allocateCommand(COMMAND_NAME, config_);
  ASSERT_TRUE(cmd);
  
  auto result_one = cmd->execute(std::move(message_one_));
  ASSERT_TRUE(result_one);
  
  auto result_two = cmd->execute(std::move(message_two_));
  ASSERT_TRUE(result_two);
}