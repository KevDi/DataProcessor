#include <gmock/gmock.h>

#include "WinIOApiWrapperMock.hpp"
#include <filesystem>

#include <DirectoryWatcher.hpp>
#include <windows.h>
#include <fstream>
using namespace testing;

void emptyFunc(const filesysteminput::FileNotification& notification) {}

class DirectoryWatcher : public Test {

public:
  void SetUp() override {
    if (std::filesystem::exists(directory)) {
      std::filesystem::remove_all(directory);
    }
    std::filesystem::create_directories(directory);
  }

  void TearDown() override {
    if (std::filesystem::exists(directory)) {
      std::filesystem::remove_all(directory);
    }
  }

  std::filesystem::path directory{ std::filesystem::temp_directory_path() / "fswatcher" };
  std::filesystem::path invalid_directory{ "invalid_directory" };
  const std::vector<std::string> file_types_{ "dat", "xml"};
  const std::string invalid_file_type_{ "txt" };
  const std::string valid_file_type_different_case_{ "DAT" };
  std::shared_ptr<WinIOApiWrapperMock> wrapper_mock = std::make_shared<WinIOApiWrapperMock>();
};

TEST_F(DirectoryWatcher, IsValidAfterSuccessfullCreationOfFileHandle) {
  filesysteminput::DirectoryWatcher watcher(directory, file_types_, emptyFunc);

  ASSERT_TRUE(watcher);
}

TEST_F(DirectoryWatcher, IsInvalidIfFileHandleIsInvalid) {
  filesysteminput::DirectoryWatcher watcher(invalid_directory, file_types_, emptyFunc);

  ASSERT_FALSE(watcher);
}

TEST_F(DirectoryWatcher, MakesACallToCreateIOCompletionWrapperIfDirectoryHandleIsValid) {
  
  EXPECT_CALL(*wrapper_mock, CreateIOCompletionPortWrapper()).Times(1);
  EXPECT_CALL(*wrapper_mock, AssociateIOCompletionPortWithFileHandle(_, _)).Times(1);
  filesysteminput::DirectoryWatcher watcher(directory, file_types_, emptyFunc, wrapper_mock);
}

TEST_F(DirectoryWatcher, MakesNoCallToCreateIOCompletionWrapperIfDirectoryHandleIsInvalid) {
  EXPECT_CALL(*wrapper_mock, CreateIOCompletionPortWrapper()).Times(0);
  filesysteminput::DirectoryWatcher watcher(invalid_directory, file_types_, emptyFunc, wrapper_mock);
}

TEST_F(DirectoryWatcher, IsInvalidIfIoCompletionPortIsInvalid) {
  EXPECT_CALL(*wrapper_mock, CreateIOCompletionPortWrapper()).WillOnce(Return(nullptr));
  filesysteminput::DirectoryWatcher watcher(directory, file_types_, emptyFunc, wrapper_mock);

  ASSERT_FALSE(watcher);
}

TEST_F(DirectoryWatcher, IsValidIfCallToAssociateIOCompletionPortWasSuccessfull) {
  EXPECT_CALL(*wrapper_mock, CreateIOCompletionPortWrapper()).Times(1);
  EXPECT_CALL(*wrapper_mock, AssociateIOCompletionPortWithFileHandle(_, _)).Times(1);
  filesysteminput::DirectoryWatcher watcher(directory, file_types_, emptyFunc, wrapper_mock);
  ASSERT_TRUE(watcher);
}

TEST_F(DirectoryWatcher, WatchReturnsFalseIfWatcherIsInvalid) {
  filesysteminput::DirectoryWatcher watcher(invalid_directory, file_types_, emptyFunc);

  ASSERT_FALSE(watcher.watch());
}

TEST_F(DirectoryWatcher, CallsGetQueuedCompletionStatusAndReadDirectoryChangesInWatch) {
  EXPECT_CALL(*wrapper_mock, CreateIOCompletionPortWrapper()).Times(1);
  EXPECT_CALL(*wrapper_mock, AssociateIOCompletionPortWithFileHandle(_, _)).Times(1);
  EXPECT_CALL(*wrapper_mock, ReadDirectoryChangesWrapper(_, _, _, _)).Times(1);
  EXPECT_CALL(*wrapper_mock, GetQueuedCompletionStatusWrapper(_, _, _, _)).Times(AtLeast(1));
  filesysteminput::DirectoryWatcher watcher(directory, file_types_, emptyFunc, wrapper_mock);

  std::thread th1{ &filesysteminput::DirectoryWatcher::watch, &watcher };

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  watcher.stop();

  th1.join();
}


TEST_F(DirectoryWatcher, CallbackIsCalledAfterFileIsAddedToTheDirectory) {

  std::filesystem::path tmp_file{directory / ("dummy." + file_types_[0])};

  bool callback_called{false};

  filesysteminput::DirectoryWatcher watcher(directory, file_types_,
                                            [tmp_file, &callback_called](const filesysteminput::FileNotification& path) {
                                              callback_called = true;
                                              ASSERT_THAT(path.filePath(), Eq(tmp_file.string()));
                                            });

  std::thread th1{&filesysteminput::DirectoryWatcher::watch, &watcher};

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  {
    std::ofstream data{ tmp_file };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  watcher.stop();

  th1.join();

  ASSERT_TRUE(callback_called);

  ASSERT_THAT(watcher.processedFilesCount(), Eq(1));
  ASSERT_THAT(watcher.ignoredFilesCount(), Eq(0));
}

TEST_F(DirectoryWatcher, CallbackIsNotCalledIfWatcherIsStopped) {
  std::filesystem::path tmp_file{ directory / ("dummy." + file_types_[0]) };
  std::filesystem::path tmp2_file{ directory / ("dummy2." + file_types_[0]) };

  bool callback_called{ false };

  filesysteminput::DirectoryWatcher watcher(directory, file_types_,
    [tmp_file, &callback_called](const filesysteminput::FileNotification& path) {
    callback_called = true;
  ASSERT_THAT(path.filePath(), Eq(tmp_file.string()));
  });

  std::thread th1{ &filesysteminput::DirectoryWatcher::watch, &watcher };

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_THAT(watcher.processedFilesCount(), Eq(0));

  {
    std::ofstream data{ tmp_file };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_TRUE(callback_called);

  watcher.stop();

  callback_called = false;

  {
    std::ofstream data{ tmp2_file };
  }

  th1.join();

  ASSERT_FALSE(callback_called);

  ASSERT_THAT(watcher.processedFilesCount(), Eq(1));
  ASSERT_THAT(watcher.ignoredFilesCount(), Eq(0));
}

TEST_F(DirectoryWatcher, CallbackIsNotCalledAfterInvalidFiletypeIsAddedToTheDirectory) {

  std::filesystem::path tmp_file{ directory / ("dummy." + invalid_file_type_) };
  std::filesystem::path second_temp_file{ directory / ("dummy." + valid_file_type_different_case_) };

  bool callback_called{ false };

  filesysteminput::DirectoryWatcher watcher(directory, file_types_,
    [tmp_file, &callback_called](const filesysteminput::FileNotification& path) {
      callback_called = true;
  ASSERT_THAT(path.filePath(), Eq(tmp_file.string()));
    });

  std::thread th1{ &filesysteminput::DirectoryWatcher::watch, &watcher };

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  {
    std::ofstream data{ tmp_file };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
 
  watcher.stop();

  th1.join();

  ASSERT_FALSE(callback_called);
  ASSERT_THAT(watcher.processedFilesCount(), Eq(0));
  ASSERT_THAT(watcher.ignoredFilesCount(), Eq(1));
}

TEST_F(DirectoryWatcher, CallbackIsCalledAfterFileWithDifferentExtensionCaseIsProcessed) {

  std::filesystem::path tmp_file{ directory / ("dummy." + valid_file_type_different_case_) };

  bool callback_called{ false };

  filesysteminput::DirectoryWatcher watcher(directory, file_types_,
    [tmp_file, &callback_called](const filesysteminput::FileNotification& path) {
    callback_called = true;
    ASSERT_THAT(path.filePath(), Eq(tmp_file.string()));
  });

  std::thread th1{ &filesysteminput::DirectoryWatcher::watch, &watcher };

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  {
    std::ofstream data{ tmp_file };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_TRUE(callback_called);

  watcher.stop();

  th1.join();

  ASSERT_THAT(watcher.processedFilesCount(), Eq(1));
  ASSERT_THAT(watcher.ignoredFilesCount(), Eq(0));
}

TEST_F(DirectoryWatcher, CallbackIsCalledOnlyForAddedFiles) {
  std::filesystem::path tmp_file{ directory / ("dummy." + file_types_[0]) };
  int counter = 0;

  filesysteminput::DirectoryWatcher watcher(directory, file_types_, [&counter](const filesysteminput::FileNotification& path) {
    counter++;
  });

  std::thread th1{ &filesysteminput::DirectoryWatcher::watch, &watcher };

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  {
    std::ofstream data{ tmp_file };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  try {
    std::filesystem::rename(tmp_file, std::filesystem::path{ tmp_file }.replace_extension(invalid_file_type_));
  } catch (const std::exception& exc) {
    std::cerr << exc.what() << '\n';
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  watcher.stop();

  th1.join();

  ASSERT_THAT(counter, Eq(1));
  
  ASSERT_THAT(watcher.processedFilesCount(), Eq(1));
  ASSERT_THAT(watcher.ignoredFilesCount(), Eq(2));
}

TEST_F(DirectoryWatcher, CallbackIsCalledForRenamedFileWithValidExtensions) {
  std::filesystem::path tmp_file{ directory / ("dummy." + invalid_file_type_) };
  int counter = 0;

  filesysteminput::DirectoryWatcher watcher(directory, file_types_, [&counter](const filesysteminput::FileNotification& path) {
    counter++;
  });

  std::thread th1{ &filesysteminput::DirectoryWatcher::watch, &watcher };

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  {
    std::ofstream data{ tmp_file };
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  try {
    std::filesystem::rename(tmp_file, std::filesystem::path{ tmp_file }.replace_extension(file_types_[0]));
  } catch (const std::exception& exc) {
    std::cerr << exc.what() << '\n';
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  watcher.stop();

  th1.join();

  ASSERT_THAT(counter, Eq(1));

  ASSERT_THAT(watcher.processedFilesCount(), Eq(1));
  ASSERT_THAT(watcher.ignoredFilesCount(), Eq(2));
}
