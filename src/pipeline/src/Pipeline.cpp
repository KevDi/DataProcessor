#include <Pipeline/Pipeline.hpp>

#include <Poco/ThreadPool.h>
#include <Poco/RunnableAdapter.h>

#include <FilesystemInput/FilesystemInput.hpp>
#include <Commands/Command.hpp>
#include <PipelineWorker.hpp>
#include <Core/Message.hpp>
#include "Core/MessageQueueImpl.hpp"
#include "Pipeline/VersionInfo_Pipeline.h"

namespace pipeline {

  std::string version() {
    return std::string{ PRODUCT_VERSION_FULL_STR };
  }

class Pipeline::PipelineImpl {
public:
  PipelineImpl(const std::string& name, std::vector<commands::CommandPtr> commands, commands::CommandPtr error_command);

  void start();
  void stop();
  void queue(core::MessageQueuePtr queue);
  std::string name() const;
  size_t workerSize() const;
  size_t availableWorkers() const;
  size_t usedWorkers() const;

private:
  void internalCallback(const std::string& id, const std::string& content);

  std::vector<commands::CommandRawPtr> commandsRawPtr() const;

  std::string name_;
  std::unique_ptr<filesysteminput::FilesystemInput> fsi_;
  core::MessageQueuePtr queue_ = std::make_unique<core::MessageQueueImpl>();
  std::vector<commands::CommandPtr> commands_;
  commands::CommandPtr error_command_;
  std::vector<std::unique_ptr<PipelineWorker>> workers_{};
  std::vector<Poco::RunnableAdapter<PipelineWorker>> worker_adapters_{};
  Poco::ThreadPool worker_pool_{ 2, 16 };
};

Pipeline::PipelineImpl::PipelineImpl(const std::string& name, std::vector<commands::CommandPtr> commands, commands::CommandPtr error_command) :
name_(name),
fsi_(std::make_unique<filesysteminput::FilesystemInput>([this](const std::string& id, const std::string& content) {this->internalCallback(id, content); })),
commands_(std::move(commands)),
error_command_(std::move(error_command)) {
}

void Pipeline::PipelineImpl::start() {
  fsi_->start();

  for (int i = 0; i < worker_pool_.capacity(); ++i) {
    std::string name{ "PipelineWorker_" + std::to_string(i) };
    workers_.emplace_back(std::make_unique<PipelineWorker>(name, queue_.get(), commandsRawPtr(), error_command_.get()));
    worker_adapters_.emplace_back( *workers_[i], &PipelineWorker::run );
  }

  for (auto& worker : worker_adapters_) {
    worker_pool_.start(worker);
  }
}

void Pipeline::PipelineImpl::stop() {
  fsi_->stop();
  queue_->close();
  worker_pool_.joinAll();
}

void Pipeline::PipelineImpl::queue(core::MessageQueuePtr queue) {
  queue_ = std::move(queue);
}

std::string Pipeline::PipelineImpl::name() const {
  return name_;
}

size_t Pipeline::PipelineImpl::workerSize() const {
  return worker_pool_.capacity();
}

size_t Pipeline::PipelineImpl::availableWorkers() const {
  return worker_pool_.available();
}

size_t Pipeline::PipelineImpl::usedWorkers() const {
  return worker_pool_.used();
}

void Pipeline::PipelineImpl::internalCallback(const std::string& id, const std::string& content) {
  queue_->enqueue(std::make_unique<core::Message>(id, content));
}

std::vector<commands::CommandRawPtr> Pipeline::PipelineImpl::commandsRawPtr() const {
  std::vector<commands::CommandRawPtr> commands;
  for (const auto& command : commands_) {
    commands.emplace_back(command.get());
  }
  return commands;
}

Pipeline::Pipeline(const std::string& name, std::vector<commands::CommandPtr> commands, commands::CommandPtr error_command) :
  pimpl_(std::make_unique<PipelineImpl>(name, std::move(commands), std::move(error_command))) {
}

Pipeline::~Pipeline() = default;

void Pipeline::queue(core::MessageQueuePtr queue) {
  pimpl_->queue(std::move(queue));
}

void Pipeline::start() {
  pimpl_->start();
}

void Pipeline::stop() {
  pimpl_->stop();
}

std::string Pipeline::name() const {
  return pimpl_->name();
}

size_t Pipeline::workerSize() const {
  return pimpl_->workerSize();
}

size_t Pipeline::availableWorkers() const {
  return pimpl_->availableWorkers();
}

size_t Pipeline::usedWorkers() const {
  return pimpl_->usedWorkers();
}
}
