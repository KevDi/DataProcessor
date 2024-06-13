#include <FilesystemInput/FilesystemInput.hpp>
#include <filesystem>
#include <iostream>
#include <thread>

int main() {

  int counter = 0;

  std::cout << std::filesystem::current_path() << '\n';
  std::cout << "Create FSI\n";
  auto fsi = std::make_unique<filesysteminput::FilesystemInput>([&counter](const std::string& filename, const std::string& content) {
    std::cout << "FSI 1: " << filename << '\n';
    try {
      if(counter == 0) {
        std::filesystem::rename(filename, std::filesystem::path{ filename }.replace_extension("proc"));
        counter++;
      }
      
      //std::filesystem::remove(filename);
    } catch(const std::exception& exc) {
      std::cout << "Error: " << exc.what() << '\n';
    }
  });

  
  std::cout << "Start\n";

  fsi->start();

  std::this_thread::sleep_for(std::chrono::minutes(2));

  fsi->stop();

  std::cout << "Ende\n";

}
