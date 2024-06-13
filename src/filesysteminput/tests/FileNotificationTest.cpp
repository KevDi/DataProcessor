#include <gmock/gmock.h>

#include "FileNotification.hpp"

using namespace testing;

static const std::string PATH{ "D:\\Example.txt" };
static const std::string CHANGED_PATH{ "D:\\Example_1.txt" };
static const filesysteminput::FileNotification::MessageSource MESSAGE_SOURCE{ filesysteminput::FileNotification::MessageSource::DirectoryReader };
static const filesysteminput::FileNotification::MessageSource CHANGED_MESSAGE_SOURCE{ filesysteminput::FileNotification::MessageSource::DirectoryWatcher };
static const filesysteminput::FileNotification::MessageType MESSAGE_TYPE{ filesysteminput::FileNotification::MessageType::NewFile };
static const filesysteminput::FileNotification::MessageType CHANGED_MESSAGE_TYPE{ filesysteminput::FileNotification::MessageType::Close };


class FileNotificationTest : public Test {
public:
	filesysteminput::FileNotification notification_{ PATH,  MESSAGE_SOURCE, MESSAGE_TYPE };
};

TEST_F(FileNotificationTest, PathReturnsFilePathPassedToTheConstructor) {
	ASSERT_THAT(notification_.filePath(), Eq(PATH));
}

TEST_F(FileNotificationTest, MessageSourceReturnsMessageSourcePassedToTheConstructor) {
	ASSERT_THAT(notification_.messageSource(), Eq(MESSAGE_SOURCE));
}

TEST_F(FileNotificationTest, MessageTypeReturnsMessageTypePassedToTheConstructor) {
	ASSERT_THAT(notification_.messageType(), Eq(MESSAGE_TYPE));
}

TEST_F(FileNotificationTest, CopyConstructNewNotification) {
	notification_.incRetryCounter();
	notification_.incRetryCounter();

	filesysteminput::FileNotification newNotification{ notification_ };

  ASSERT_THAT(newNotification.filePath(), Eq(PATH));
	ASSERT_THAT(newNotification.messageSource(), Eq(MESSAGE_SOURCE));
	ASSERT_THAT(newNotification.messageType(), Eq(MESSAGE_TYPE));

	// Keeps value of RetryCounter after copy construction
	ASSERT_THAT(newNotification.retryCounter(), Eq(2));
}

TEST_F(FileNotificationTest, PathCalledWithStringChangesThePathOfTheNotification) {
	notification_.filePath(CHANGED_PATH);
	ASSERT_THAT(notification_.filePath(), Eq(CHANGED_PATH));
}

TEST_F(FileNotificationTest, MessageSourceCalledWithArgumentChangesTheMessageSourceOfTheNotification) {
	notification_.messageSource(CHANGED_MESSAGE_SOURCE);
	ASSERT_THAT(notification_.messageSource(), Eq(CHANGED_MESSAGE_SOURCE));
}

TEST_F(FileNotificationTest, MessageTypeCalledWithArgumentChangesTheMessageTypeOfTheNotification) {
	notification_.messageType(CHANGED_MESSAGE_TYPE);
	ASSERT_THAT(notification_.messageType(), Eq(CHANGED_MESSAGE_TYPE));
}