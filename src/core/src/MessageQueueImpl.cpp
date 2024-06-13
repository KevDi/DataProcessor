#include "Core/MessageQueueImpl.hpp"
#include <Poco/NotificationQueue.h>
#include <Core/Message.hpp>

namespace core {
  class MessageQueueImpl::MessageQueuePimpl {
  public:
    void enqueue(std::unique_ptr<Message> message);
    std::unique_ptr<Message> dequeue();
    int size() const;
    void close();
  private:
    Poco::NotificationQueue queue_;
  };


  void MessageQueueImpl::MessageQueuePimpl::enqueue(std::unique_ptr<Message> message) {
    auto raw_message = message.release();
    queue_.enqueueNotification(raw_message);
  }

  std::unique_ptr<Message> MessageQueueImpl::MessageQueuePimpl::dequeue() {
    auto notification = queue_.waitDequeueNotification();
    return std::unique_ptr<Message>(dynamic_cast<Message*>(notification));
  }

  int MessageQueueImpl::MessageQueuePimpl::size() const {
    return queue_.size();
  }

  void MessageQueueImpl::MessageQueuePimpl::close() {
    queue_.wakeUpAll();
  }


  MessageQueueImpl::MessageQueueImpl() : pimpl_(std::make_unique<MessageQueuePimpl>()) {
  }

  MessageQueueImpl::~MessageQueueImpl() = default;

  void MessageQueueImpl::enqueue(std::unique_ptr<Message> message) {
    pimpl_->enqueue(std::move(message));
  }

  std::unique_ptr<Message> MessageQueueImpl::dequeue() {
    return pimpl_->dequeue();
  }

  int MessageQueueImpl::size() const {
    return pimpl_->size();
  }

  void MessageQueueImpl::close() {
    pimpl_->close();
  }
}
