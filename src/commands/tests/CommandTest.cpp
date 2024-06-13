#include <gmock/gmock.h>
#include <Commands/Command.hpp>
#include <Core/Message.hpp>
#include "CommandMock.h"
#include "Commands/VersionInfo_Commands.h"

using namespace testing;

class Command : public Test {
public:

  void SetUp() override {
    ON_CALL(command_mock_, cloneImpl()).WillByDefault(Return(&new_command));
  }

  core::MessagePtr message_{ std::make_unique<core::Message>("1", "Message") };
  NiceMock<CommandMock> command_mock_;
  NiceMock<CommandMock> new_command;
};

TEST_F(Command, CommandReturnsCorrectVersion) {
  EXPECT_THAT(new_command.version(), PRODUCT_VERSION_FULL_STR);
}

TEST_F(Command, ExecuteCallsExecuteImpl) {
  EXPECT_CALL(command_mock_, executeImpl(_)).Times(1);

  command_mock_.execute(std::move(message_));
}

TEST_F(Command, ExecuteCallsCheckPreconditionsImpl) {
  EXPECT_CALL(command_mock_, checkPreconditionsImpl(_)).Times(1);
  command_mock_.execute(std::move(message_));
}

TEST_F(Command, ExecuteDoesNotCallExecuteImplIfCheckPreconditionsImplThrowsException) {
  EXPECT_CALL(command_mock_, checkPreconditionsImpl(_)).WillOnce(Throw(std::runtime_error{"Test"}));
  EXPECT_CALL(command_mock_, executeImpl(_)).Times(0);
  try {
    command_mock_.execute(std::move(message_));
    FAIL();
  } catch (const std::runtime_error&) {
    SUCCEED();
  }
}

TEST_F(Command, CallsCloneImplWhenCloneIsCalled) {
  commands::commandsConfig config{{"1", "2"}};
  EXPECT_CALL(command_mock_, cloneImpl()).Times(1);
  command_mock_.clone(config);
}

TEST_F(Command, CloneCallsConfigureImplAfterCallingCloneImpl) {
  
  commands::commandsConfig config{{"1", "2"}};
  InSequence seq;

  EXPECT_CALL(command_mock_, cloneImpl()).WillOnce(Return(&new_command));
  EXPECT_CALL(new_command, configureImpl(config)).Times(1);

  command_mock_.clone(config);
}

TEST_F(Command, MakeCommandCreatesACommandPointer) {
  auto cmdPtr = commands::MakeCommandPtr<CommandMock>();

  ASSERT_TRUE(cmdPtr);
}