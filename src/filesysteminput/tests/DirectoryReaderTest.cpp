#include <Core/Message.hpp>
#include <gmock/gmock.h>
#include <Poco/Path.h>
#include <Poco/String.h>
#include <Poco/File.h>

#include <DirectoryReader.hpp>
#include <fstream>

using namespace testing;

const size_t AMMOUNT_OF_FILES{ 5 };
const size_t AMMOUNT_OF_IGNORED_FILES{ 9 };
const size_t AMMOUNT_OF_MESSAGES{ AMMOUNT_OF_FILES + 1 }; // NewFileMessages + 1 CloseMessage

class DirectoryReaderTest : public Test {
public:
	std::thread thread_{};
	std::vector<filesysteminput::FileNotification> received_messages_;
	const std::string temp_path_{ std::filesystem::path{ std::filesystem::current_path() / "resources" / "temp" }.string() };
	const std::vector<std::string> file_types_{ "dat", "xml" };
	const std::string invalid_file_type_{ "txt" };
	filesysteminput::DirectoryReader reader_{ temp_path_, file_types_, [this](filesysteminput::FileNotification message) {
		received_messages_.push_back(message);
	} };

	void SetUp() override {
		CleanUpTemp(temp_path_);
		CreateDummyFiles(temp_path_, AMMOUNT_OF_IGNORED_FILES, invalid_file_type_);
		CreateDummyFiles(temp_path_, AMMOUNT_OF_FILES, "DAT");
		thread_ = std::thread{ &filesysteminput::DirectoryReader::run, &reader_ };
	}

	void TearDown() override {
		CleanUpTemp(temp_path_);
		received_messages_.clear();
		thread_.join();
	}

	void CreateDummyFiles(std::string_view path, size_t ammount, std::string_view file_type) const {
		if (!std::filesystem::exists(path)) {
			std::filesystem::create_directory(path);
		}
    for (size_t i = 0; i < ammount; i++) {
			std::string fileName = "Filename_" + std::to_string(i) + "." + file_type.data();
			{
				std::ofstream tempFile{ std::filesystem::path{path} / fileName };
			}
    }
	}

	void CleanUpTemp(const std::string& path) {
	  if(std::filesystem::exists(path)) {
			std::filesystem::remove_all(path);
	  }
	}
};

TEST_F(DirectoryReaderTest, ReaderSendsNotifications) {
	Sleep(100);

	ASSERT_THAT(received_messages_.size(), Eq(AMMOUNT_OF_MESSAGES));

	std::vector<std::string> fileSuffix;
	for (int i = 0; i < AMMOUNT_OF_FILES; i++) {
		fileSuffix.push_back(std::to_string(i));
	}
	std::sort(fileSuffix.begin(), fileSuffix.end(), std::less<std::string>());

	// Every file with configured file type is received
  for (int i = 0; i < AMMOUNT_OF_FILES; i++) {
		ASSERT_THAT(Poco::toLower(received_messages_.at(i).filePath()), Eq(Poco::toLower(Poco::Path{temp_path_, "Filename_" + fileSuffix[i] + ".dat"}.toString())));
		ASSERT_THAT(received_messages_.at(i).messageSource(), Eq(filesysteminput::FileNotification::MessageSource::DirectoryReader));
		ASSERT_THAT(received_messages_.at(i).messageType(), Eq(filesysteminput::FileNotification::MessageType::NewFile));
  }

	// Last Message must be CloseMessage
	ASSERT_THAT(received_messages_.back().messageSource(), Eq(filesysteminput::FileNotification::MessageSource::DirectoryReader));
	ASSERT_THAT(received_messages_.back().messageType(), Eq(filesysteminput::FileNotification::MessageType::Close));

	// Metrics are delivered by reader
	ASSERT_THAT(reader_.processedFilesCount(), Eq(AMMOUNT_OF_FILES));
	ASSERT_THAT(reader_.ignoredFilesCount(), Eq(AMMOUNT_OF_IGNORED_FILES));
}

