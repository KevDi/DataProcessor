#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "export.hpp"
#include "Commands/Command.hpp"
#include "Core/MessageQueue.hpp"

namespace pipeline {

  std::string PIPELINE_EXPORT version();

  class PIPELINE_EXPORT Pipeline {
    class PipelineImpl;
  public:
    Pipeline(const std::string& name, std::vector<commands::CommandPtr> commands, commands::CommandPtr error_command);
    virtual ~Pipeline();
    void queue(core::MessageQueuePtr queue);
    void start();
    void stop();
    [[nodiscard]] std::string name() const;
    [[nodiscard]] size_t workerSize() const;
    [[nodiscard]] size_t availableWorkers() const;
    [[nodiscard]] size_t usedWorkers() const;

  private:
    std::unique_ptr<PipelineImpl> pimpl_;
  };


}
