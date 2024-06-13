#pragma once
#include <memory>
#include "export.hpp"

namespace core {
  class Message;

  class CORE_EXPORT MessageQueue {

  public:
    virtual void enqueue(std::unique_ptr<Message> message) = 0;
    virtual std::unique_ptr<Message> dequeue() = 0;
    virtual int size() const = 0;
    virtual void close() = 0;
  };

  using MessageQueuePtr = std::unique_ptr<MessageQueue>;
  using MessageQueueRawPtr = MessageQueue*;
}