#include <gmock/gmock.h>

#include <PipelineWorker.hpp>
#include <thread>
#include <exception>
#include "CommandMock.hpp"
#include "MessageQueueMock.hpp"

using namespace testing;

class PipelineWorkerTest : public Test {
public:

  void SetUp() override {
    std::vector<commands::CommandRawPtr> pipe_commands = { cmd_1_.get(), cmd_2_.get(), cmd_3_.get() };
    worker_ = std::make_unique<pipeline::PipelineWorker>(WORKER_NAME, queue_.get(), pipe_commands, test_);
  }

  void TearDown() override {
  }

  const std::string WORKER_NAME = "Pipeline_Worker_1";

  std::thread thread_;

  std::unique_ptr<MessageQueueMock> queue_ = std::make_unique<MessageQueueMock>();
  std::unique_ptr<CommandMock> cmd_1_ = std::make_unique<CommandMock>("Command_1");
  std::unique_ptr<CommandMock> cmd_2_ = std::make_unique<CommandMock>("Command_2");
  std::unique_ptr<CommandMock> cmd_3_ = std::make_unique<CommandMock>("Command_3");
  std::unique_ptr<CommandMock> error_cmd_ = std::make_unique<CommandMock>("ErrorCommand");
  commands::CommandRawPtr test_ = error_cmd_.get();
  std::unique_ptr<pipeline::PipelineWorker> worker_;
};

TEST_F(PipelineWorkerTest, WorkerRunsPipelineCommandsInCorrectOrderOnRun) {
  testing::Sequence s1;
  
  EXPECT_CALL(*queue_, dequeue()).Times(1).InSequence(s1).WillOnce(Return(core::MessagePtr { std::make_unique<core::Message>("1", "Message") }));
  EXPECT_CALL(*cmd_1_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_1_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*queue_, dequeue()).Times(1).InSequence(s1).WillOnce(Return(nullptr));

  worker_->run();
}

TEST_F(PipelineWorkerTest, WorkerProceedsExecutionUntilNullptrGetsDequeued) {
  testing::Sequence s1;

  EXPECT_CALL(*queue_, dequeue()).Times(4)
    .WillOnce(Return(core::MessagePtr { std::make_unique<core::Message>("1", "Message") }))
    .WillOnce(Return(core::MessagePtr { std::make_unique<core::Message>("2", "Message") }))
    .WillOnce(Return(core::MessagePtr { std::make_unique<core::Message>("3", "Message") }))
    .WillOnce(Return(nullptr));

  EXPECT_CALL(*cmd_1_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_1_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_1_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_1_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_1_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_1_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, executeImpl(_)).Times(1).InSequence(s1);

  worker_->run();
}

TEST_F(PipelineWorkerTest, WhenACommandThrowsAnExceptionTheErrorCommandIsExecutedAndNextMessageIsDequeued) {
  testing::Sequence s1;

  EXPECT_CALL(*queue_, dequeue()).Times(1).InSequence(s1).WillOnce(Return(core::MessagePtr { std::make_unique<core::Message>("1", "Message") }));
  EXPECT_CALL(*cmd_1_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_1_, executeImpl(_)).Times(1).InSequence(s1).WillOnce(Throw(std::exception{}));
  EXPECT_CALL(*error_cmd_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*error_cmd_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*queue_, dequeue()).Times(1).InSequence(s1).WillOnce(Return(nullptr));

  // Command 2 and 3 should not be executed
  EXPECT_CALL(*cmd_2_, checkPreconditionsImpl(_)).Times(0);
  EXPECT_CALL(*cmd_2_, executeImpl(_)).Times(0);
  EXPECT_CALL(*cmd_3_, checkPreconditionsImpl(_)).Times(0);
  EXPECT_CALL(*cmd_3_, executeImpl(_)).Times(0);

  worker_->run();
}

TEST_F(PipelineWorkerTest, FunctionNameReturnsNameOfPipelineWorker) {
  ASSERT_THAT(worker_->name(), Eq(WORKER_NAME));
}

TEST_F(PipelineWorkerTest, FunctionPipelineProcessDurationReturnsLastDuration) {
  testing::Sequence s1;

  EXPECT_CALL(*queue_, dequeue()).Times(1).InSequence(s1).WillOnce(Return(core::MessagePtr { std::make_unique<core::Message>("1", "Message") }));
  EXPECT_CALL(*cmd_1_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_1_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_2_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*cmd_3_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*queue_, dequeue()).Times(1).InSequence(s1).WillOnce(Return(nullptr));

  ASSERT_THAT(worker_->pipelineProcessDuration(), Eq(0.0));

  worker_->run();

  ASSERT_TRUE(worker_->pipelineProcessDuration() > 0.0);
}