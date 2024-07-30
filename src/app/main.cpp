#include <FilesystemInput/FilesystemInput.hpp>
#include <filesystem>
#include <iostream>
#include <thread>
#include <Commands/Command.hpp>
#include <Commands/CommandFactory.hpp>
#include <AddMetadataCommand/AddMetadataCommand.hpp>
#include <WrapCommand/WrapCommand.hpp>
#include <ErrorCommand/ErrorCommand.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Core/Message.hpp>

class TestCommand : public commands::Command {
  void checkPreconditionsImpl(core::Message* msg) override {
    std::cout << "Check Precondition...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << "...done\n";
  }

  Command* cloneImpl() override {
    return new TestCommand{ *this };
  }

  void configureImpl(commands::commandsConfig config) override {
    std::cout << "Configure\n";
  }

  std::unique_ptr<core::Message> executeImpl(std::unique_ptr<core::Message> msg) override {
    std::cout << "Message received write through\n";
    std::cout << "Message ID: " << msg->id() << '\n';
    std::cout << "Message Content: " << msg->content() << '\n';
    return msg;
  }
};

int main() {

  auto& cmd_factory = commands::CommandFactory::Instance();


  cmd_factory.registerCommand("AddMetadataCommand", commands::MakeCommandPtr<commands::AddMetadataCommand>());
  cmd_factory.registerCommand("WrapCommand", commands::MakeCommandPtr<commands::WrapCommand>());
  cmd_factory.registerCommand("TestCommand", commands::MakeCommandPtr<TestCommand>());
  cmd_factory.registerCommand("ErrorCommand", commands::MakeCommandPtr<commands::ErrorCommand>());

  commands::commandsConfig addmetadata_config{ {"key_1", "value_1"}, { "key_2", "value_2" } };
  commands::commandsConfig wrap_config{
    {commands::WrapCommand::CFG_CMD_WRAP_TYPE, "json"},
    { commands::WrapCommand::CFG_CMD_WRAP_ELEMENT, "content" },
    { commands::WrapCommand::CFG_CMD_WRAP_METADATA, "true" }
  };

  std::vector<commands::CommandPtr> pipe_commands{};

  pipe_commands.push_back(cmd_factory.allocateCommand("AddMetadataCommand", addmetadata_config));
  pipe_commands.push_back(cmd_factory.allocateCommand("WrapCommand", wrap_config));
  pipe_commands.push_back(cmd_factory.allocateCommand("TestCommand", {}));

  auto pipeline = std::make_unique<pipeline::Pipeline>("Pipeline 1", std::move(pipe_commands), std::move(cmd_factory.allocateCommand("ErrorCommand", {})));

  std::cout << "Start Pipeline...\n";

  pipeline->start();

  std::cout << "Wait 2 Minutes...\n";

  std::this_thread::sleep_for(std::chrono::minutes(2));

  std::cout << "Stop Pipeline...\n";

  pipeline->stop();

}
