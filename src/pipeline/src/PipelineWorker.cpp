#include <PipelineWorker.hpp>

namespace pipeline {

class PipelineWorker::PipelineWorkerImpl {
public:
  PipelineWorkerImpl(const std::string& name, core::MessageQueueRawPtr queue, const std::vector<commands::CommandRawPtr>& commands, commands::CommandRawPtr error_command);
  void run();
  std::string name() const;
  double pipelineProcessDuration() const;

private:
  std::string name_;
  core::MessageQueueRawPtr queue_;
  std::vector<commands::CommandRawPtr> commands_;
  commands::CommandRawPtr error_command_;
  std::chrono::duration<double> pipeline_process_duration_{};
};

std::string PipelineWorker::PipelineWorkerImpl::name() const {
  return name_;
}

double PipelineWorker::PipelineWorkerImpl::pipelineProcessDuration() const {
  return pipeline_process_duration_.count();
}

PipelineWorker::PipelineWorkerImpl::PipelineWorkerImpl(const std::string& name, core::MessageQueueRawPtr queue, const std::vector<commands::CommandRawPtr>& commands, commands::CommandRawPtr error_command) :
name_(name),
queue_(queue),
commands_(commands),
error_command_(error_command) {
}

void PipelineWorker::PipelineWorkerImpl::run() {
  while (auto message = queue_->dequeue()) {
    auto pipeline_process_duration_begin = std::chrono::steady_clock::now();
    for (auto command : commands_) {
      try {
        message = command->execute(std::move(message));
      } catch (const std::exception& e) {
        error_command_->execute(std::make_unique<core::Message>(e.what(), std::string{ "Worker: " + name() + "\nCommand: " + command->name() }));
        break;
      }
    }
    pipeline_process_duration_ = std::chrono::steady_clock::now() - pipeline_process_duration_begin;
  }
}

PipelineWorker::PipelineWorker(const std::string& name, core::MessageQueueRawPtr queue, const std::vector<commands::CommandRawPtr>& commands, commands::CommandRawPtr error_command) :
  pimpl_( std::make_unique<PipelineWorkerImpl>(name, queue, commands, error_command) ) {
}

PipelineWorker::~PipelineWorker() = default;

void PipelineWorker::run() {
  pimpl_->run();
}

std::string PipelineWorker::name() const {
  return pimpl_->name();
}

double PipelineWorker::pipelineProcessDuration() const {
  return pimpl_->pipelineProcessDuration();
}

}
