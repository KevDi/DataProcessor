#pragma once
#include <gmock/gmock.h>
#include <Core/Message.hpp>
using namespace testing;

class CommandMock : public commands::Command {
public:
  MOCK_METHOD(void, configureImpl, (commands::commandsConfig), (override));
  MOCK_METHOD(Command*, cloneImpl, (), (override));
  MOCK_METHOD(void, checkPreconditionsImpl, (core::MessageRawPtr), (override));
  MOCK_METHOD(core::MessagePtr, executeImpl, (core::MessagePtr), (override));
};