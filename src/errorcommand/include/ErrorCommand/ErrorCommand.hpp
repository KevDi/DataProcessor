#pragma once
#include "export.hpp"
#include <Commands/Command.hpp>

namespace commands {
  class ERRORCOMMAND_EXPORT ErrorCommand : public Command {
  public:

    static std::string version();

    explicit ErrorCommand(const std::string& name = "ErrorCommand");
    
  private:
    ErrorCommand(ErrorCommand&&) = delete;
    ErrorCommand(const ErrorCommand&) = default;
    ErrorCommand& operator=(const ErrorCommand&) = delete;
    ErrorCommand& operator=(ErrorCommand&&) = delete;
    void checkPreconditionsImpl(core::Message* message) override;
    ErrorCommand* cloneImpl() override;
    void configureImpl(commandsConfig config) override;
    std::unique_ptr<core::Message> executeImpl(std::unique_ptr<core::Message> message) override;

    commandsConfig config_{};
  };

}  // namespace commands