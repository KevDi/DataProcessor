#pragma once
#include "export.hpp"
#include <Commands/Command.hpp>

namespace commands {
  class WRAPCOMMAND_EXPORT WrapCommand : public Command {
  public:

    using TransformAction = void(WrapCommand::*)(core::Message*);
    using TransformActions = std::unordered_map<std::string, TransformAction>;

    static std::string version();
    // Config keys

    // specifies the format type of the wrapped message
    //   e.g. 'json' or 'xml'
    inline static const std::string CFG_CMD_WRAP_TYPE{"type"};
    // specifies the node name of the message content
    //   e.g. 'content' or 'data'
    inline static const std::string CFG_CMD_WRAP_ELEMENT{"element"};
    // specifies if metadata is added to the wrapped message
    //   'true' = metadata is added
    //   'false' = metadata is not added
    inline static const std::string CFG_CMD_WRAP_METADATA{"metadata"};

    explicit WrapCommand(const std::string& name = "WrapCommand");
    
  private:
    WrapCommand(WrapCommand&&) = delete;
    WrapCommand(const WrapCommand&) = default;
    WrapCommand& operator=(const WrapCommand&) = delete;
    WrapCommand& operator=(WrapCommand&&) = delete;
    void checkPreconditionsImpl(core::Message* message) override;
    WrapCommand* cloneImpl() override;
    void configureImpl(commandsConfig config) override;
    std::unique_ptr<core::Message> executeImpl(std::unique_ptr<core::Message> message) override;

    void wrapInJson(core::Message*);
    void checkConfigHasKey(const std::string& key);
    void validateConfig();

    commandsConfig config_{};
    TransformActions actions_{
      {"json", &WrapCommand::wrapInJson}
    };
    bool add_metadata_{ false };
  };

}  // namespace commands