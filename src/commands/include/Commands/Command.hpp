#pragma once
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include "export.hpp"


namespace core {
  class Message;
}

namespace commands {

  using commandsConfig = std::unordered_map<std::string, std::string>;

  class COMMANDS_EXPORT Command {
  public:
    static std::string version();
    explicit Command(const std::string& name = "");
    virtual ~Command();
    std::unique_ptr<core::Message> execute(std::unique_ptr<core::Message> message);
    Command* clone(commandsConfig config);

    std::string name() const;
    size_t messageCount() const;
    double totalDuration() const;

  protected:
    void incMessageCount();

  private:
    virtual std::unique_ptr<core::Message> executeImpl(std::unique_ptr<core::Message>  message) = 0;
    virtual void checkPreconditionsImpl(core::Message* message) = 0;
    virtual Command* cloneImpl() = 0;
    virtual void configureImpl(commandsConfig config) = 0;

    std::string name_{};
    size_t message_count_{ 0 };
    std::chrono::duration<double> total_duration_{};
  };

  using CommandPtr = std::unique_ptr<Command>;
  using CommandRawPtr = Command*;

  template<typename T, typename... Args>
  auto MakeCommandPtr(Args&&... args) {
    return CommandPtr{ new T{std::forward<Args>(args)...} };
  }

  inline auto MakeCommandPtr(Command* p) {
    return CommandPtr{ p };
  }
}