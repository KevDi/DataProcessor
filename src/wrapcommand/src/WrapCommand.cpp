#include "WrapCommand/WrapCommand.hpp"
#include "WrapCommand/VersionInfo_WrapCommand.h"
#include <nlohmann/json.hpp>
#include <Core/Message.hpp>

using json = nlohmann::ordered_json;

namespace commands {
  std::string WrapCommand::version() {
    return std::string{ PRODUCT_VERSION_FULL_STR };
  }

  WrapCommand::WrapCommand(const std::string& name) : Command(name) {
  }

  void WrapCommand::checkPreconditionsImpl(core::MessageRawPtr message) {
    validateConfig();

  }

  WrapCommand* WrapCommand::cloneImpl() {
    return new WrapCommand{ *this };
  }

  void WrapCommand::configureImpl(commandsConfig config) {
    config_ = config;
  }

  core::MessagePtr WrapCommand::executeImpl(core::MessagePtr message) {
    (this->*actions_[config_[CFG_CMD_WRAP_TYPE]])(message.get());
    incMessageCount();
    return message;
  }

  void WrapCommand::wrapInJson(core::MessageRawPtr message) {
    json new_content;

    if (add_metadata_) {
      for (auto& [key, value] : message->getAllMetadata()) {
        new_content[key] = value;
      }
    }

    std::string content = message->content();
    new_content[config_[CFG_CMD_WRAP_ELEMENT]] = content;
    ;
    message->content(to_string(new_content));
  }

  void WrapCommand::checkConfigHasKey(const std::string& key) {
    if (config_.find(key) == config_.end()) {
      throw std::invalid_argument("Argument '" + key + "' is missing");
    }
  }

  void WrapCommand::validateConfig() {
    checkConfigHasKey(CFG_CMD_WRAP_TYPE);
    if (actions_.find(config_[CFG_CMD_WRAP_TYPE]) == actions_.end()) {
      throw std::invalid_argument("Parameter '" + CFG_CMD_WRAP_TYPE + "' = " + config_[CFG_CMD_WRAP_TYPE] + " is not supported");
    }

    checkConfigHasKey(CFG_CMD_WRAP_ELEMENT);

    checkConfigHasKey(CFG_CMD_WRAP_METADATA);
    if (config_[CFG_CMD_WRAP_METADATA] == "true") {
      add_metadata_ = true;
    } else if (config_[CFG_CMD_WRAP_METADATA] == "false") {
      add_metadata_ = false;
    } else {
      throw std::invalid_argument("Parameter '" + CFG_CMD_WRAP_METADATA + "' = " + config_[CFG_CMD_WRAP_METADATA] + " is invalid. Value must be 'true' or 'false'");
    }
  }
}  // namespace commands
