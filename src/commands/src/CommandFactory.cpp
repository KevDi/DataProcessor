#include "Commands/CommandFactory.hpp"
#include <sstream>

namespace commands {
  CommandFactory& CommandFactory::Instance() {
    static CommandFactory instance;
    return instance;
  }

  void CommandFactory::registerCommand(const std::string& name, CommandPtr command) {
    if(hasKey(name)) {
      std::ostringstream oss;
      oss << "Command " << name << " already registered";
      throw std::runtime_error{oss.str()};
    }
    factory_.emplace(name, std::move(command));
  }

  CommandPtr CommandFactory::deregisterCommand(const std::string& name) {
    if(hasKey(name)) {
      auto it = factory_.find(name);
      auto tmp = MakeCommandPtr(it->second.release());
      factory_.erase(it);
      return tmp;
    }
    return MakeCommandPtr(nullptr);
  }

  CommandPtr CommandFactory::allocateCommand(const std::string& name, commandsConfig config) const {
    if (hasKey(name)) {
      const auto& command = factory_.find(name)->second;
      return MakeCommandPtr(command->clone(config));
    }
    return MakeCommandPtr(nullptr);
  }

  size_t CommandFactory::getNumberOfCommands() const {
    return factory_.size();
  }

  std::set<std::string> CommandFactory::getAllCommandNames() const {
    std::set<std::string> tmp;
    for(auto& pair : factory_) {
      tmp.insert(pair.first);
    }
    return tmp;
  }

  bool CommandFactory::hasKey(const std::string& name) const {
    return factory_.find(name) != factory_.end();
  }

  void CommandFactory::clearAllCommands() {
    factory_.clear();
  }
}
