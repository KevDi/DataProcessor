#include <filesystem>
#include "CleanupCommand/CleanupCommand.hpp"
#include "CleanupCommand/VersionInfo_CleanupCommand.h"
#include <Core/Message.hpp>

namespace commands {
  std::string CleanupCommand::version() {
    return std::string{ PRODUCT_VERSION_FULL_STR };
  }

  CleanupCommand::CleanupCommand(const std::string& name) : Command(name) {
  }

  size_t CleanupCommand::errorCountFileNotExists() const {
    return error_count_file_not_exists_;
  }

  void CleanupCommand::checkPreconditionsImpl(core::MessageRawPtr message) {

  }

  CleanupCommand* CleanupCommand::cloneImpl() {
    return new CleanupCommand{ *this };
  }

  void CleanupCommand::configureImpl(commandsConfig config) {
    config_ = config;
  }

  core::MessagePtr CleanupCommand::executeImpl(core::MessagePtr message) {
    if (!std::filesystem::remove(message->id())) {
      error_count_file_not_exists_++;
      throw std::filesystem::filesystem_error{"File '" + message->id() + "' does not exist.", {}};
    }

    incMessageCount();

    return message;
  }

}  // namespace commands
