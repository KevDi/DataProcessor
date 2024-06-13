#include <Commands/Command.hpp>
#include <Commands/VersionInfo_Commands.h>

#include <Core/Message.hpp>

namespace commands {

  std::string Command::version() {
    return std::string{PRODUCT_VERSION_FULL_STR};
  }

  Command::Command(const std::string& name) : name_(name) {
  }

  Command::~Command() = default;

  core::MessagePtr Command::execute(core::MessagePtr message) {
    auto command_execution_duration_begin = std::chrono::steady_clock::now();
    checkPreconditionsImpl(message.get());
    auto result = executeImpl(std::move(message));
    total_duration_ = std::chrono::steady_clock::now() - command_execution_duration_begin;
    return result;
  }

  Command* Command::clone(commandsConfig config) {
    std::unique_ptr<Command> new_cmd(cloneImpl());
    new_cmd->configureImpl(config);
    return new_cmd.release();
  }

  std::string Command::name() const {
    return name_;
  }

  size_t Command::messageCount() const {
    return message_count_;
  }

  double Command::totalDuration() const {
    return total_duration_.count();
  }

  void Command::incMessageCount() {
    message_count_++;
  }
}
