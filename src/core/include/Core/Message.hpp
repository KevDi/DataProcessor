#pragma once
#include <Poco/Notification.h>

#include <string>
#include <memory>
#include <unordered_map>
#include <optional>

#include "export.hpp"

namespace core {

  class CORE_EXPORT Message : public Poco::Notification {
  public:

    using Metadata = std::unordered_map<std::string, std::string>;

    explicit Message(const std::string& id);
    Message(const std::string& id, const std::string& content);
    ~Message();

    std::string content() const {
      return content_;
    }

    std::string id() const {
      return id_;
    }

    void content(const std::string& content) {
      content_ = content;
    }

    void id(const std::string& id) {
      id_ = id;
    }

    size_t metadataSize() const {
      return metadata_.size();
    }

    std::optional<std::string> getMetadata(const std::string& key);

    Metadata getAllMetadata() const {
      return metadata_;
    }

    void addMetadata(const std::string& key, const std::string& value);

    friend CORE_EXPORT bool operator==(const Message& l_message, const Message& r_message);
    friend CORE_EXPORT bool operator!=(const Message& l_message, const Message& r_message);

  private:
    std::string id_;
    std::string content_;
    Metadata metadata_;
  };

  using MessagePtr = std::unique_ptr<Message>;
  using MessageRawPtr = Message*;
}  // namespace Core
