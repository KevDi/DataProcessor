#pragma once
#include "export.hpp"
#include <Commands/Command.hpp>
#include <vector>

namespace commands {
  class CLEANUPCOMMAND_EXPORT CleanupCommand : public Command {
  public:

    static std::string version();

    explicit CleanupCommand(const std::string& name = "CleanupCommand");

    size_t errorCountFileNotExists() const;

  private:
    CleanupCommand(CleanupCommand&&) = delete;
    CleanupCommand(const CleanupCommand&) = default;
    CleanupCommand& operator=(const CleanupCommand&) = delete;
    CleanupCommand& operator=(CleanupCommand&&) = delete;
    void checkPreconditionsImpl(core::Message* message) override;
    CleanupCommand* cloneImpl() override;
    void configureImpl(commandsConfig config) override;
    std::unique_ptr<core::Message> executeImpl(std::unique_ptr<core::Message> message) override;

    commandsConfig config_{};
    size_t error_count_file_not_exists_{ 0 };
  };

}  // namespace commands