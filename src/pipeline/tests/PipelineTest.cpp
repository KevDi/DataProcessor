#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>

#include <Pipeline/Pipeline.hpp>
#include <Core/Message.hpp>
#include <Core/MessageQueueImpl.hpp>
#include <FilesystemInput/FilesystemInput.hpp>

#include "CommandMock.hpp"

using namespace testing;

class PipelineTest : public Test {
public:

  void SetUp() override {
    if (std::filesystem::exists(directory_)) {
      std::filesystem::remove_all(directory_);
    }

    std::filesystem::create_directories(directory_);

    pipe_commands_.push_back(std::move(cmd_1_));
    pipe_commands_.push_back(std::move(cmd_2_));
    pipe_commands_.push_back(std::move(cmd_3_));

    pipeline_ = std::make_unique<pipeline::Pipeline>(PIPELINE_NAME, std::move(pipe_commands_), std::move(error_cmd_));
  }

  void TearDown() override {
    std::filesystem::remove_all(directory_);
  }

  const std::string PIPELINE_NAME = "Pipeline_1";
  const std::string FILE_TYPE = "dat";

  std::filesystem::path directory_{ std::filesystem::temp_directory_path() / "pipeline_test" };

  core::MessageQueuePtr queue_{ std::make_unique<core::MessageQueueImpl>() };
  core::MessageQueueRawPtr p_queue_{ queue_.get() };

  std::unique_ptr<CommandMock> cmd_1_ = std::make_unique<CommandMock>("Command_1");
  std::unique_ptr<CommandMock> cmd_2_ = std::make_unique<CommandMock>("Command_2");
  std::unique_ptr<CommandMock> cmd_3_ = std::make_unique<CommandMock>("Command_3");
  std::unique_ptr<CommandMock> error_cmd_ = std::make_unique<CommandMock>("ErrorCommand");

  CommandMock* p_cmd_1_ = cmd_1_.get();
  CommandMock* p_cmd_2_ = cmd_2_.get();
  CommandMock* p_cmd_3_ = cmd_3_.get();


  std::vector<commands::CommandPtr> pipe_commands_{};
  std::unique_ptr<pipeline::Pipeline> pipeline_{};
};

TEST_F(PipelineTest, PipelineReturnsName) {
  ASSERT_THAT(pipeline_->name(), Eq(PIPELINE_NAME));
}

TEST_F(PipelineTest, PipelineReturns16AvailableWorkersAfterCreation) {
  ASSERT_THAT(pipeline_->availableWorkers(), Eq(16));
}

TEST_F(PipelineTest, PipelineReturns0AvailableWorkersAfterStart) {
  pipeline_->start();

  ASSERT_THAT(pipeline_->availableWorkers(), Eq(0));

  pipeline_->stop();
}

TEST_F(PipelineTest, PipelineReturns0UsedWorkersAfterCreation) {
  ASSERT_THAT(pipeline_->usedWorkers(), Eq(0));
}

TEST_F(PipelineTest, PipelineReturns16UsedWorkersAfterStart) {
  pipeline_->start();

  ASSERT_THAT(pipeline_->usedWorkers(), Eq(16));

  pipeline_->stop();
}

TEST_F(PipelineTest, PipelineReturnsWorkerSize) {
  ASSERT_THAT(pipeline_->workerSize(), Eq(16));
}

TEST_F(PipelineTest, PipelineProcessesFileWithAllCommandsInCorrectOrder) {
  pipeline_->queue(std::move(queue_));
  p_queue_->enqueue(std::make_unique<core::Message>("MessageOne", "TestContent"));

  testing::Sequence s1;
  EXPECT_CALL(*p_cmd_1_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*p_cmd_1_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*p_cmd_2_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*p_cmd_2_, executeImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*p_cmd_3_, checkPreconditionsImpl(_)).Times(1).InSequence(s1);
  EXPECT_CALL(*p_cmd_3_, executeImpl(_)).Times(1).InSequence(s1);

  pipeline_->start();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  pipeline_->stop();  
}