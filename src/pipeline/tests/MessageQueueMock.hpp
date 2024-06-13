#pragma once

#include <Core/MessageQueue.hpp>
#include <Core/Message.hpp>

class MessageQueueMock : public core::MessageQueue {
public:
  MOCK_METHOD(void, enqueue, (std::unique_ptr<core::Message>), (override));
  MOCK_METHOD(std::unique_ptr<core::Message>, dequeue, (), (override));
  MOCK_METHOD(int, size, (), (const, override));
  MOCK_METHOD(void, close, (), (override));

  MessageQueueMock() {
    ON_CALL(*this, dequeue).WillByDefault([this]() {
      return nullptr;
      });
  }
};