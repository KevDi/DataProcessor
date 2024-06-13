#pragma once
#include "Command.hpp"
#include "export.hpp"
#include <set>

namespace commands {

class COMMANDS_EXPORT CommandFactory {

public:
  static CommandFactory& Instance();

  void registerCommand(const std::string& name, CommandPtr command);
  CommandPtr deregisterCommand(const std::string& name);
  CommandPtr allocateCommand(const std::string& name, commandsConfig config) const;

  size_t getNumberOfCommands() const;
  std::set<std::string> getAllCommandNames() const;
  bool hasKey(const std::string& name) const;
  void clearAllCommands();

private:
  CommandFactory() = default;
  ~CommandFactory() = default;

  CommandFactory(CommandFactory&) = delete;
  CommandFactory(CommandFactory&&) = delete;
  CommandFactory& operator=(CommandFactory&) = delete;
  CommandFactory& operator=(CommandFactory&&) = delete;

  using Factory = std::unordered_map<std::string, commands::CommandPtr>;
  Factory factory_;

};

}