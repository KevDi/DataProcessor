
#include "FilesystemInput/FilesystemInput.hpp"
#include <thread>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>

namespace filesysteminput {
  

  FilesystemInput::FilesystemInput(std::function<void(const std::string&, const std::string&)> callback)
    : callback_(std::move(callback)) {
  }

  FilesystemInput::~FilesystemInput() = default;

  void FilesystemInput::start() {
    worker_ = std::make_unique<std::thread>(&FilesystemInput::run, this);
  }
  
  void FilesystemInput::stop() {
    stopped_ = true;
    worker_->join();
  }

  void FilesystemInput::run() {
    std::filesystem::path dir_to_check{"test"};
    int counter = 0;
    while(!stopped_) {
      for (auto const& dir_entry : std::filesystem::directory_iterator{dir_to_check}) {
        std::ifstream t(dir_entry.path());
        std::stringstream buffer;
        buffer << t.rdbuf();
        callback_(dir_entry.path(), buffer.str());
      }
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  }  
}
