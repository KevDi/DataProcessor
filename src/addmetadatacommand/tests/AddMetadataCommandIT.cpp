#include <gmock/gmock.h>
#include <Commands/CommandFactory.hpp>
#include <AddMetadataCommand/AddMetadataCommand.hpp>

#include <Core/Message.hpp>

using namespace testing;

static const std::string COMMAND_NAME{"AddMetadataCommand"};

class AddMetadataCommandIT : public Test {

public:
  void SetUp() override {
    command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<commands::AddMetadataCommand>());
  }

  void TearDown() override {
    command_factory_.clearAllCommands();
  }
  commands::CommandFactory& command_factory_ = commands::CommandFactory::Instance();
  commands::commandsConfig config_{};
  core::MessagePtr message_one_ = std::make_unique<core::Message>("MessageOne");
  core::MessagePtr message_two_ = std::make_unique<core::Message>("MessageTwo");
};

TEST_F(AddMetadataCommandIT, CommandReturnsCorrectName) {
  auto cmd = command_factory_.allocateCommand(COMMAND_NAME, config_);
  ASSERT_THAT(cmd.get()->name(), Eq(COMMAND_NAME));
}

TEST_F(AddMetadataCommandIT, MetadataIsAddedToTheMessage) {

  ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(1));

  commands::commandsConfig config_one{{"key_1", "value_1"}, { "key_2", "value_2" }};
  commands::commandsConfig config_two{{"key_1", "value_1"}, { "key_2", "value_2" }, { "key_3", "value_3" }};

  auto add_metadata_command_one = command_factory_.allocateCommand(COMMAND_NAME, config_one);
  ASSERT_TRUE(add_metadata_command_one);
  auto add_metadata_command_two = command_factory_.allocateCommand(COMMAND_NAME, config_two);
  ASSERT_TRUE(add_metadata_command_two);

  auto result_one = add_metadata_command_one->execute(std::move(message_one_));
  ASSERT_TRUE(result_one);
  ASSERT_THAT(result_one->getAllMetadata(), UnorderedElementsAreArray(config_one));

  auto result_two = add_metadata_command_two->execute(std::move(message_two_));
  ASSERT_TRUE(result_two);
  ASSERT_THAT(result_two->getAllMetadata(), UnorderedElementsAreArray(config_two));
}

TEST_F(AddMetadataCommandIT, NoMetadataIsAddedIfCommandDoesNotContainMetadata) {
  auto add_metadata_command_one = command_factory_.allocateCommand(COMMAND_NAME, {});
  auto result = add_metadata_command_one->execute(std::move(message_one_));

  ASSERT_TRUE(result);
  ASSERT_THAT(result->metadataSize(), Eq(0));
}