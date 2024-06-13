#include "AddMetadataCommand/AddMetadataCommand.hpp"
#include "AddMetadataCommand/VersionInfo_AddMetadataCommand.h"
#include <Core/Message.hpp>

namespace commands {
  std::string AddMetadataCommand::version() {
    return std::string{ PRODUCT_VERSION_FULL_STR };
  }

  AddMetadataCommand::AddMetadataCommand(const std::string& name) : Command(name) {
  }

  size_t AddMetadataCommand::size() const {
    return metadata_.size();
  }

  void AddMetadataCommand::checkPreconditionsImpl(core::MessageRawPtr message) {
    return;
  }

  AddMetadataCommand* AddMetadataCommand::cloneImpl() {
    return new AddMetadataCommand{ *this };
  }

  void AddMetadataCommand::configureImpl(commandsConfig config) {
    metadata_ = config;
  }

  core::MessagePtr AddMetadataCommand::executeImpl(core::MessagePtr message) {
    for (const auto& [key, value] : metadata_) {
      message->addMetadata(key, value);
    }

    incMessageCount();

    return message;
  }
}  // namespace commands
