#include <gmock/gmock.h>
#include <Core/MessageQueueImpl.hpp>
#include <Core/Message.hpp>
#include <thread>

using namespace testing;

class MessageQueue : public Test {
public:
  core::MessageQueuePtr msg_queue_impl_{ std::make_unique<core::MessageQueueImpl>() };
  std::unique_ptr<core::Message> message_{ std::make_unique<core::Message>("1", "Message") };
};

TEST_F(MessageQueue, HasSizeZeroAfterCreation) {
  ASSERT_THAT(msg_queue_impl_->size(), Eq(0));
}

TEST_F(MessageQueue, EnqueueIncreasesSizeByOne) {
  ASSERT_THAT(msg_queue_impl_->size(), Eq(0));

  msg_queue_impl_->enqueue(std::move(message_));

  ASSERT_THAT(msg_queue_impl_->size(), Eq(1));
}

TEST_F(MessageQueue, DequeueDecreasesSizeByOne) {
  msg_queue_impl_->enqueue(std::move(message_));
  ASSERT_THAT(msg_queue_impl_->size(), Eq(1));

  msg_queue_impl_->dequeue();

  ASSERT_THAT(msg_queue_impl_->size(), Eq(0));
}

TEST_F(MessageQueue, DequeueReturnsMessage) {
  msg_queue_impl_->enqueue(std::move(message_));
  auto msg = msg_queue_impl_->dequeue();

  ASSERT_THAT(msg->id(), Eq("1"));
  ASSERT_THAT(msg->content(), Eq("Message"));
}

TEST_F(MessageQueue, ThreadWaitsOnDequeueIfNoMessageInQueue) {
  std::thread worker([this]() {
    auto val = msg_queue_impl_->dequeue();
    ASSERT_TRUE(val);
  });

  ASSERT_TRUE(worker.joinable());

  msg_queue_impl_->enqueue(std::move(message_));

  worker.join();

  ASSERT_FALSE(worker.joinable());
}

TEST_F(MessageQueue, CloseWokeUpWaitingThread) {
  std::thread worker([this]() {
    auto val = msg_queue_impl_->dequeue();
  });

  ASSERT_TRUE(worker.joinable());

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  msg_queue_impl_->close();

  worker.join();

  ASSERT_FALSE(worker.joinable());
}

TEST_F(MessageQueue, CloseWokeUpWaitingThreadAndReturnsNullMessage) {
  std::thread worker([this]() {
    auto val = msg_queue_impl_->dequeue();
    ASSERT_FALSE(val);
  });

  ASSERT_TRUE(worker.joinable());

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  msg_queue_impl_->close();

  worker.join();

  ASSERT_FALSE(worker.joinable());
}