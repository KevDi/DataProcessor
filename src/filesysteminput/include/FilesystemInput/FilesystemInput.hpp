#pragma once
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <thread>
#include "export.hpp"

namespace filesysteminput {

  class FILESYSTEMINPUT_EXPORT FilesystemInput {
    class FilesystemInputImpl;
  public:
    FilesystemInput(std::function<void(const std::string&, const std::string&)>);
    ~FilesystemInput();
    void run();
    void start();
    void stop();

  private:
    std::function<void(const std::string&, const std::string&)> callback_;
    std::unique_ptr<std::thread> worker_{};
    bool stopped_{ false };
  };
}