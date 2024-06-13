#pragma once
#include "export.hpp"
#include <vector>
#include <Commands/Command.hpp>

namespace commands {

  class ADDMETADATACOMMAND_EXPORT AddMetadataCommand : public Command {
  public:
    static std::string version();

    explicit  AddMetadataCommand(const std::string& name = "AddMetadataCommand");
    size_t size() const;

  private:
    AddMetadataCommand(AddMetadataCommand&&) = delete;
    AddMetadataCommand(const AddMetadataCommand&) = default;
    AddMetadataCommand& operator=(const AddMetadataCommand&) = delete;
    AddMetadataCommand& operator=(AddMetadataCommand&&) = delete;
    void checkPreconditionsImpl(core::Message* message) override;
    AddMetadataCommand* cloneImpl() override;
    void configureImpl(commandsConfig config) override;
    std::unique_ptr<core::Message> executeImpl(std::unique_ptr<core::Message> message) override;

    std::unordered_map<std::string, std::string> metadata_{};
  };

}  // namespace commands