#pragma once
#include <Core/Message.hpp>
#include <Commands/Command.hpp>

class CommandMock : public commands::Command {
public:
  explicit CommandMock(const std::string& name) : Command(name) {
    ON_CALL(*this, executeImpl).WillByDefault([this](core::MessagePtr message) { return message; });

  }

  MOCK_METHOD(std::unique_ptr<core::Message>, executeImpl, (std::unique_ptr<core::Message>), (override));
  MOCK_METHOD(void, checkPreconditionsImpl, (core::Message*), (override));
  MOCK_METHOD(Command*, cloneImpl, (), (override));
  MOCK_METHOD(void, configureImpl, (commands::commandsConfig), (override));
};