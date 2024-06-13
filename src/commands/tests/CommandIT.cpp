#include <gmock/gmock.h>
#include <Core/Message.hpp>
#include <Commands/CommandFactory.hpp>
#include <Commands/Command.hpp>

using namespace testing;

class ExampleCommand : public commands::Command {
public:
  ExampleCommand() = default;
  explicit ExampleCommand(const ExampleCommand& rhs) = default;
  ~ExampleCommand() override = default;

private:
  ExampleCommand(ExampleCommand&&) = delete;
  ExampleCommand& operator=(const ExampleCommand&) = delete;
  ExampleCommand& operator=(ExampleCommand&&) = delete;

  core::MessagePtr executeImpl(core::MessagePtr message) override {
    message->id(target_);
    message->content(std::to_string(data_));
    return message;
  }

  void checkPreconditionsImpl(core::MessageRawPtr message) override {
    if (message->id() != "Test") {
      throw std::runtime_error{"Message id is not correct"};
    }
  }

  ExampleCommand* cloneImpl() override {
    return new ExampleCommand{ *this };
  }
  void configureImpl(commands::commandsConfig config) override {
    data_ = stoi(config.find("data")->second);
    target_ = config.find("target")->second;

  }

  int data_{};
  std::string target_{};
  
};

class CommandIT : public Test {
public:

  void SetUp() override {
    command_factory_.registerCommand("Command_1", commands::MakeCommandPtr<ExampleCommand>());
    command_factory_.registerCommand("Command_2", commands::MakeCommandPtr<ExampleCommand>());
  }

  void TearDown() override {
    command_factory_.clearAllCommands();
  }

  commands::CommandFactory& command_factory_ = commands::CommandFactory::Instance();
  core::MessagePtr message_one_ = std::make_unique<core::Message>("Test");
  core::MessagePtr message_two_ = std::make_unique<core::Message>("Test");
  core::MessagePtr incorrect_msg = std::make_unique<core::Message>("Incorrect");
};

TEST_F(CommandIT, CorrectMessagesAreHandled) {
  ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(2));
  ASSERT_THAT(command_factory_.getAllCommandNames(), UnorderedElementsAre("Command_1", "Command_2"));

  commands::commandsConfig command_config_one{{"data", "100"}, { "target", "station" }};
  auto concrete_command_one = command_factory_.allocateCommand("Command_1", command_config_one);

  ASSERT_TRUE(concrete_command_one);

  auto result_one = concrete_command_one->execute(std::move(message_one_));
  
  ASSERT_TRUE(result_one);
  ASSERT_THAT(result_one->id(), Eq("station"));
  ASSERT_THAT(result_one->content(), Eq("100"));

  commands::commandsConfig command_config_two{{"data", "1"}, { "target", "line" }};
  auto concrete_command_two = command_factory_.allocateCommand("Command_2", command_config_two);

  ASSERT_TRUE(concrete_command_two);

  auto result_two = concrete_command_two->execute(std::move(message_two_));

  ASSERT_TRUE(result_one);
  ASSERT_THAT(result_one->id(), Eq("station"));
  ASSERT_THAT(result_one->content(), Eq("100")); 
}

TEST_F(CommandIT, IncorrectMessageThrowsException) {

  ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(2));
  ASSERT_THAT(command_factory_.getAllCommandNames(), UnorderedElementsAre("Command_1", "Command_2"));

  commands::commandsConfig command_config_one{{"data", "100"}, { "target", "station" }};
  auto concrete_command_one = command_factory_.allocateCommand("Command_1", command_config_one);

  ASSERT_TRUE(concrete_command_one);

  ASSERT_THROW(concrete_command_one->execute(std::move(incorrect_msg)), std::runtime_error);
}