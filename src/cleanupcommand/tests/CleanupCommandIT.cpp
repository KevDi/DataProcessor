#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <Commands/CommandFactory.hpp>
#include <CleanupCommand/CleanupCommand.hpp>
#include <Core/Message.hpp>

using namespace testing;

static const std::string COMMAND_NAME{"CleanupCommand"};

class CleanupCommandIT : public Test {

public:
  void SetUp() override {
    if (std::filesystem::exists(directory)) {
      std::filesystem::remove_all(directory);
    }
    std::filesystem::create_directories(directory);

    command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<commands::CleanupCommand>());
  }

  void TearDown() override {
    if (std::filesystem::exists(directory)) {
      std::filesystem::remove_all(directory);
    }

    command_factory_.clearAllCommands();
  }

  void CreateDummyFile(std::filesystem::path filepath) {
    std::ofstream tempFile{ filepath };
    tempFile.close();
  }

  std::filesystem::path directory{ std::filesystem::temp_directory_path() / "cleanup" };

  commands::CommandFactory& command_factory_ = commands::CommandFactory::Instance();
  commands::commandsConfig config_{};

  std::string file{"File.dat"};
  core::MessagePtr message_one_ = std::make_unique<core::Message>("1","MessageOne");
};

TEST_F(CleanupCommandIT, CommandReturnsCorrectName) {
  auto cmd = command_factory_.allocateCommand(COMMAND_NAME, config_);
  ASSERT_THAT(cmd.get()->name(), Eq(COMMAND_NAME));
}

TEST_F(CleanupCommandIT, CommandDeletesFileAfterAllocationAndExecution) {
  std::filesystem::path file_path{ directory / file };
  CreateDummyFile(file_path);
  ASSERT_TRUE(std::filesystem::exists(file_path));

  message_one_ = std::make_unique<core::Message>(file_path.string(), "MessageOne");

  ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(1));

  auto cmd = command_factory_.allocateCommand(COMMAND_NAME, config_);
  ASSERT_TRUE(cmd);

  auto result_one = cmd->execute(std::move(message_one_));
  ASSERT_TRUE(result_one);
  ASSERT_FALSE(std::filesystem::exists(file_path));
}