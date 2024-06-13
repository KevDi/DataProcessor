#include <gmock/gmock.h>

#include <Core/Message.hpp>

using namespace testing;

const static std::string FIRST_MESSAGE_ID{"Test"};
const static std::string SECOND_MESSAGE_ID{"Other"};
const static std::string FIRST_CONTENT{"First Content"};
const static std::string SECOND_CONTENT{"Second Content"};

TEST(Message, WithSameIDsAreEqual) {
    const core::Message message_one{FIRST_MESSAGE_ID};
    const core::Message message_two{FIRST_MESSAGE_ID};

    ASSERT_EQ(message_one, message_two);
}

TEST(Message, WithDifferentIDsAreNotEqual) {
    const core::Message message_one{FIRST_MESSAGE_ID};
    const core::Message message_two{SECOND_MESSAGE_ID};

    ASSERT_NE(message_one, message_two);
}

TEST(Message, WithSameIDsAndDifferentContentAreEqual) {
    const core::Message message_one{FIRST_MESSAGE_ID, FIRST_CONTENT};
    const core::Message message_two{FIRST_MESSAGE_ID, SECOND_CONTENT};

    ASSERT_EQ(message_one, message_two);
}

TEST(Message, ContainsZeroMetadataElementsAfterCreation) {
  const core::Message message{FIRST_MESSAGE_ID};

  ASSERT_THAT(message.metadataSize(), Eq(0));
}

TEST(Message, AddMetadataAddsTheMetadataElementsToTheMessage) {
  core::Message message{FIRST_MESSAGE_ID};

  message.addMetadata("Meta", "Data");

  ASSERT_THAT(message.metadataSize(), Eq(1));
}

TEST(Message, GetMetadataReturnsFilledOptionalIfMetadataIsInTheMap) {
  core::Message message{FIRST_MESSAGE_ID};

  message.addMetadata("Meta", "Data");

  auto metadata = message.getMetadata("Meta");

  ASSERT_TRUE(metadata);
}

TEST(Message, GetMetadataReturnsEmptyOptionalIfMetadataIsNotInTheMap) {
  core::Message message{FIRST_MESSAGE_ID};

  auto metadata = message.getMetadata("Meta");

  ASSERT_FALSE(metadata);
}

TEST(Message, GetAllMetadataReturnsMapWithAllTheMetadata) {
  core::Message message{FIRST_MESSAGE_ID};

  message.addMetadata("Meta", "Data");
  message.addMetadata("Meta1", "Data1");
  message.addMetadata("Meta2", "Data2");

  std::unordered_map<std::string, std::string> expected{{"Meta", "Data"}, { "Meta1", "Data1" }, { "Meta2", "Data2" }};

  auto actual = message.getAllMetadata();

  ASSERT_THAT(actual, UnorderedElementsAreArray(expected));
}