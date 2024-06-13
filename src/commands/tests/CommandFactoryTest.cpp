#include <gmock/gmock.h>
#include <Commands/CommandFactory.hpp>

#include "CommandMock.h"

using namespace testing;

const static std::string COMMAND_NAME{"CommandMock"};

class CommandFactory : public Test {

public:

  void TearDown() override {
	  command_factory_.clearAllCommands();
  }

	commands::CommandFactory& command_factory_{ commands::CommandFactory::Instance() };
};

TEST_F(CommandFactory, AlwaysReturnTheSameInstance) {
	auto& other = commands::CommandFactory::Instance();

	ASSERT_THAT(&command_factory_, Eq(&other));
}

TEST_F(CommandFactory, getNumberOfCommandsReturns0AfterCreation) {

	ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(0));
}

TEST_F(CommandFactory, getNumberOfCommandsReturns1AfterRegisteringACommand) {

	command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<CommandMock>());

	ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(1));
}

TEST_F(CommandFactory, hasKeyReturnsFalseIfCommandIsNotRegistered) {
	ASSERT_FALSE(command_factory_.hasKey(COMMAND_NAME));
}

TEST_F(CommandFactory, hasKeyReturnsTrueIfCommandWasRegistered) {
	command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<CommandMock>());
	ASSERT_TRUE(command_factory_.hasKey(COMMAND_NAME));
}

TEST_F(CommandFactory, clearAllCommandsEmptiesTheRegisteredCommands) {
	command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<CommandMock>());

	ASSERT_TRUE(command_factory_.hasKey(COMMAND_NAME));
	ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(1));

  command_factory_.clearAllCommands();

  ASSERT_FALSE(command_factory_.hasKey(COMMAND_NAME));
  ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(0));
}

TEST_F(CommandFactory, ifCommandWasAlreadyRegisteredAnExceptionIsThrown) {
	command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<CommandMock>());

	ASSERT_TRUE(command_factory_.hasKey(COMMAND_NAME));

	ASSERT_THROW(command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<CommandMock>()), std::runtime_error);
}

TEST_F(CommandFactory, deregisterCommandReturnsPtrToTheDeregisteredCommand) {
	command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<CommandMock>());
	auto cmdPtr = command_factory_.deregisterCommand(COMMAND_NAME);

	ASSERT_TRUE(cmdPtr);
}

TEST_F(CommandFactory, deregisterCommandReturnsNullptrIfCommandIsNotRegistered) {
	auto cmdPtr = command_factory_.deregisterCommand(COMMAND_NAME);

	ASSERT_FALSE(cmdPtr);
}

TEST_F(CommandFactory, deregisterCommandRemovesCommandFromTheFactory) {
	command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<CommandMock>());
	auto cmdPtr = command_factory_.deregisterCommand(COMMAND_NAME);

	ASSERT_FALSE(command_factory_.hasKey(COMMAND_NAME));
}

TEST_F(CommandFactory, allocateCommandReturnsCloneOfTheRegisteredCommand) {
	CommandMock* new_command = new CommandMock();
	std::unique_ptr<CommandMock> mock_command = std::make_unique<CommandMock>();
	EXPECT_CALL(*mock_command, cloneImpl()).WillOnce(Return(new_command));
	EXPECT_CALL(*new_command, configureImpl(_)).Times(1);
	command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr(mock_command.release()));
	mock_command = nullptr;
	auto cmd_ptr = command_factory_.allocateCommand(COMMAND_NAME, { {"Data", "1"} });
	ASSERT_TRUE(cmd_ptr);
}


TEST_F(CommandFactory, allocateCommandReturnsNullptrIfRequestedCommandIsNotRegistered) {
	auto cmd_ptr = command_factory_.allocateCommand(COMMAND_NAME, { {"Data", "1"} });

	ASSERT_FALSE(cmd_ptr);
}

TEST_F(CommandFactory, getAllCommandNamesReturnsEmptySetIfNoCommandsAreRegistered) {
	auto names = command_factory_.getAllCommandNames();

	ASSERT_TRUE(names.empty());
}

TEST_F(CommandFactory, getAllCommandNamesReturnsSetWithAllRegisteredCommandNames) {
	command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<CommandMock>());
	command_factory_.registerCommand("SecondCommand", commands::MakeCommandPtr<CommandMock>());
	command_factory_.registerCommand("ThirdCommand", commands::MakeCommandPtr<CommandMock>());

	auto names = command_factory_.getAllCommandNames();

	ASSERT_THAT(names, UnorderedElementsAre(COMMAND_NAME, "SecondCommand", "ThirdCommand"));
}