#include "Core/Message.hpp"

namespace core {

  Message::Message(const std::string& id) : Message(id, {}) {
  }

  Message::Message(const std::string& id, const std::string& content) : id_(id), content_(content) {}

  Message::~Message() = default;

  std::optional<std::string> Message::getMetadata(const std::string& key) {
    auto tmp = metadata_.find(key);
    return tmp == metadata_.end() ? std::nullopt : std::optional<std::string>{ tmp->second };
  }

  void Message::addMetadata(const std::string& key, const std::string& value) {
    metadata_.emplace(key, value);
  }

  bool operator==(const Message& l_message, const Message& r_message) {
    return l_message.id_ == r_message.id_;
  }

  bool operator!=(const Message& l_message, const Message& r_message) {
    return !(l_message == r_message);
  }
}
