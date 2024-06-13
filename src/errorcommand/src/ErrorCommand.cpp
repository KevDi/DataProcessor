#include <filesystem>
#include <iostream>
#include <Core/Message.hpp>
#include "ErrorCommand/ErrorCommand.hpp"
#include "ErrorCommand/VersionInfo_ErrorCommand.h"

namespace commands {
  std::string ErrorCommand::version() {
    return std::string{ PRODUCT_VERSION_FULL_STR };
  }

  ErrorCommand::ErrorCommand(const std::string& name) : Command(name) {
  }

  void ErrorCommand::checkPreconditionsImpl(core::MessageRawPtr message) {

  }

  ErrorCommand* ErrorCommand::cloneImpl() {
    return new ErrorCommand{ *this };
  }

  void ErrorCommand::configureImpl(commandsConfig config) {
    config_ = config;
  }

  core::MessagePtr ErrorCommand::executeImpl(core::MessagePtr message) {
    std::cerr << "Error processing message: '" << message.get()->id() << "'" << std::endl;
    std::cerr << message.get()->content() << std::endl;

    incMessageCount();

    return message;
  }

}  // namespace commands
