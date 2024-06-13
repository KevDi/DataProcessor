#pragma once
#include <memory>
#include <vector>

#include <Core/Message.hpp>
#include <Core/MessageQueue.hpp>
#include <Commands/Command.hpp>


namespace pipeline {

class PipelineWorker {
	class PipelineWorkerImpl;
public:
	PipelineWorker(const std::string& name, core::MessageQueueRawPtr queue, const std::vector<commands::CommandRawPtr>& commands, commands::CommandRawPtr error_command);
	virtual ~PipelineWorker();
	void run();
	std::string name() const;
	double pipelineProcessDuration() const;

private:
	std::unique_ptr<PipelineWorkerImpl> pimpl_;
};

}
