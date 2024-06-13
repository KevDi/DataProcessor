#pragma once
#include <memory>
#include "export.hpp"
#include "MessageQueue.hpp"

namespace core {
  class Message;

  class CORE_EXPORT MessageQueueImpl : public MessageQueue {
    class MessageQueuePimpl;
  public:
    MessageQueueImpl();
    virtual ~MessageQueueImpl();
    void enqueue(std::unique_ptr<Message> message) override;
    std::unique_ptr<Message> dequeue() override;
    int size() const override;
    void close() override;

  private:
    std::unique_ptr<MessageQueuePimpl> pimpl_;
  };

}