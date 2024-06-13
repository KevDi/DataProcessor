#include <filesystem>

#include <Core/Message.hpp>
#include <gmock/gmock.h>
#include <Poco/Path.h>

#include <WorkerThread.hpp>
#include <Poco/File.h>

#include "FileNotification.hpp"

using namespace testing;
namespace fi = filesysteminput;
namespace fs = std::filesystem;

const std::string FILENAME{ "exampleFile.dat" };
const std::string SECOND_FILENAME{ "exampleFile2.dat" };
const std::string FILENAME_NOT_FOUND{"exampleFileDoesNotExist.dat"};

class WorkerThreadTest : public Test{
public:
  Poco::NotificationQueue output_queue_;
  std::thread thread_;
  std::pair<std::string, std::string> received_data_;
  int callback_calls_{ 0 };
  fi::WorkerThread worker_{ output_queue_, [this](const std::string& id, const std::string& content) {
    received_data_.first = id;
    received_data_.second = content;
    ++callback_calls_;
  } };
  const std::string temp_path{ fs::path{ fs::current_path() / "resources" / "temp" }.string() };


  void SetUp() override {
    thread_ = std::thread{ &filesysteminput::WorkerThread::start, &worker_ };
  }

  void TearDown() override {
    output_queue_.wakeUpAll();
    worker_.stop();
    thread_.join();
    received_data_ = std::make_pair(std::string{}, std::string{});
  }

  void enqueTestFile(const std::string& filename = FILENAME, fi::FileNotification::MessageSource src = fi::FileNotification::MessageSource::DirectoryReader, fi::FileNotification::MessageType type = fi::FileNotification::MessageType::NewFile)
  {
    output_queue_.enqueueNotification(new filesysteminput::FileNotification{
      Poco::Path::current() + "resources\\" + filename,
      src, type
    });
  }
};

TEST_F(WorkerThreadTest, QueueIsEmptyAfterWorkerThreadCompletesRunMethod) {
  enqueTestFile();
  Sleep(100);
  ASSERT_TRUE(output_queue_.empty());
}

TEST_F(WorkerThreadTest, WorkerThreadSendsNotificationToObservers) {
  enqueTestFile();
  Sleep(100);
  ASSERT_THAT(callback_calls_, Eq(1));
  ASSERT_THAT(worker_.messagesSentToCallbackCount(), Eq(callback_calls_));
}

TEST_F(WorkerThreadTest, InputNotificationHasCorrectContent) {
  enqueTestFile();
  Sleep(100);

  ASSERT_THAT(received_data_.second, Eq("; Version: V01\n; Date:    12/07/2018\n; Time:    13:17:59\n;\n\\DataStructures\\ResultDataStruct\\FU1 #Index\n  _Format = REG_DWORD 0x0\n  _Crc32_Struct = REG_DWORD 0x5bd3e42f\n  _Id = \"0887fcf8-0612-48a9-affb-3025e25001bd\"\n  _VersionDate = \"4.344155415509e+004\"\n  _DBDataClass = REG_DWORD 0x0\n  _DBStructVersion = \"cdd7702acffd.eFu1.ResultData_5bd3e42f\"\n  Security = REG_DWORD 0x0\n  _SwVersion\n    VarType = REG_DWORD 0x8\n    Data = \"V02.05.000\"\n  ResHead #Index\n    StructName = \"DbResultDataHeadStruct\"\n    TypeNo\n      VarType = REG_DWORD 0x8\n      Data = \"0445116059\"\n    TypeVar\n      VarType = REG_DWORD 0x8\n      Data = \"\""));
}

TEST_F(WorkerThreadTest, IfFileNotFoundTheNotificationIsNotPutBackIntoQueue) {
  enqueTestFile(FILENAME_NOT_FOUND);
  Sleep(100);

  ASSERT_TRUE(output_queue_.empty());
  ASSERT_TRUE(received_data_.first.empty());
}

TEST_F(WorkerThreadTest, IfFileAccessIsDeniedTheNotificationIsPutBackIntoQueue) {
  if(!std::filesystem::exists(temp_path)) {
    std::filesystem::create_directory(temp_path);
  }

  Poco::File exampleFile{ Poco::Path{ temp_path, "WorkerThreadTest.txt"}};
  if (std::filesystem::exists(exampleFile.path())) {
    std::filesystem::permissions(exampleFile.path(), std::filesystem::perms::all);
    exampleFile.remove();
  }

  auto fHandle = CreateFileA(exampleFile.path().c_str(), GENERIC_READ, 0, NULL, CREATE_ALWAYS, 0, NULL);
  std::filesystem::permissions(exampleFile.path(), std::filesystem::perms::none);
  auto notification = new fi::FileNotification{ exampleFile.path(), fi::FileNotification::MessageSource::DirectoryReader, fi::FileNotification::MessageType::NewFile };
  output_queue_.enqueueNotification(notification);
  Sleep(100);
  
  ASSERT_THAT(worker_.retriedMessagesCount(), Eq(10));
  ASSERT_TRUE(received_data_.first.empty());

  CloseHandle(fHandle);
  std::filesystem::permissions(exampleFile.path(), fs::perms::all);
  exampleFile.remove();
}

TEST_F(WorkerThreadTest, QueueWillNotBeDequedAfterWorkerThreadHasStopped) {
  worker_.stop();
  enqueTestFile();
  Sleep(100);
  ASSERT_THAT(output_queue_.size(), Eq(1));
}

TEST_F(WorkerThreadTest, FilenameSetContainsAnElementAfterReceivingMessageFromDirectoryWatcher) {
  enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  Sleep(100);
  ASSERT_THAT(worker_.processedFilesSize(), Eq(1));
}

TEST_F(WorkerThreadTest, FilenameIsNotAddedToSetAfterReceivingMessageFromDirectoryReader) {
  enqueTestFile(FILENAME);
  Sleep(100);
  ASSERT_THAT(worker_.processedFilesSize(), Eq(0));
}

TEST_F(WorkerThreadTest, FilenameIsNotAddedToSetAfterReceivingARetryMessage) {
  enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher, fi::FileNotification::MessageType::Retry);
  Sleep(100);
  ASSERT_THAT(worker_.processedFilesSize(), Eq(0));
}

TEST_F(WorkerThreadTest, IfMessageAlreadyInTheSetDirectoryReaderMessageIsNotProcessed) {
  enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  enqueTestFile(FILENAME);
  Sleep(100);

  ASSERT_THAT(callback_calls_, Eq(1));
  ASSERT_THAT(worker_.messagesSentToCallbackCount(), Eq(callback_calls_));
}

TEST_F(WorkerThreadTest, CallbackIsNotCalledOnCloseMessage) {
  enqueTestFile("", fi::FileNotification::MessageSource::DirectoryReader, fi::FileNotification::MessageType::Close);
  Sleep(100);

  ASSERT_THAT(callback_calls_, Eq(0));
  ASSERT_THAT(worker_.messagesSentToCallbackCount(), Eq(callback_calls_));
  ASSERT_THAT(worker_.retriedMessagesCount(), Eq(0));
}

TEST_F(WorkerThreadTest, NoFilenameIsAddedToSetAfterCloseMessage) {
  enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  Sleep(100);

  ASSERT_THAT(worker_.processedFilesSize(), Eq(1));

  enqueTestFile("", fi::FileNotification::MessageSource::DirectoryReader, fi::FileNotification::MessageType::Close);
  enqueTestFile(SECOND_FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  Sleep(100);

  ASSERT_THAT(worker_.processedFilesSize(), Eq(0));
}

TEST_F(WorkerThreadTest, SetWithFilenamesIsClearedAfterCloseMessage) {
  enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  Sleep(100);

  ASSERT_THAT(worker_.processedFilesSize(), Eq(1));

  enqueTestFile("", fi::FileNotification::MessageSource::DirectoryReader, fi::FileNotification::MessageType::Close);
  enqueTestFile(SECOND_FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  Sleep(100);

  ASSERT_THAT(worker_.processedFilesSize(), Eq(0));
}

TEST_F(WorkerThreadTest, WorkerReturnsSizeOfQueue) {
  worker_.stop();

  ASSERT_THAT(worker_.currentQueueSize(), Eq(0));

  for (int i = 0; i < 5; ++i) {
    enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  }

  ASSERT_THAT(worker_.currentQueueSize(), Eq(5));

  for (int i = 0; i < 5; ++i) {
    enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryReader);
  }

  ASSERT_THAT(worker_.currentQueueSize(), Eq(10));
}

TEST_F(WorkerThreadTest, WorkerReturnsReadFileDuration) {
  enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  Sleep(100);

  ASSERT_THAT(worker_.readFileDuration(), Gt(0.0));
}

TEST_F(WorkerThreadTest, WorkerReturnsMessageProcessingDuration) {
  enqueTestFile(FILENAME, fi::FileNotification::MessageSource::DirectoryWatcher);
  Sleep(100);

  ASSERT_THAT(worker_.processMessageDuration(), Gt(0.0));
}