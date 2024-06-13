#include <gmock/gmock.h>
#include <Core/Message.hpp>
#include <AddMetadataCommand/AddMetadataCommand.hpp>
#include "AddMetadataCommand/VersionInfo_AddMetadataCommand.h"

using namespace testing;

class AddMetadataCommand : public Test {
public: 
  void SetUp() override {

    cloned_cmd_.reset(cmd_.clone(metadata_));

  }

  const std::string COMMAND_NAME = "AddMetadataCommand";
  const size_t METRICS_SIZE = 2;

  commands::commandsConfig metadata_ {
    {"metadata_1", "value_1"},
    { "metadata_2", "value_2" }
  };
  commands::AddMetadataCommand cmd_;
  std::unique_ptr<commands::Command> cloned_cmd_;
  core::MessagePtr msg = std::make_unique<core::Message>("1", "Content");
};

TEST_F(AddMetadataCommand, CommandReturnsCorrectName) {
  ASSERT_THAT(cloned_cmd_->name(), COMMAND_NAME);
}

TEST_F(AddMetadataCommand, CommandReturnsCorrectVersion) {
  EXPECT_THAT(cloned_cmd_->version(), PRODUCT_VERSION_FULL_STR);
}

TEST_F(AddMetadataCommand, ReturnsZeroForTheMetadataSizeAfterCreation) {
  ASSERT_THAT(cmd_.size(), Eq(0));
}

TEST_F(AddMetadataCommand, ReturnsTheSizeOfTheGivenMetadataAfterCloningIt) {
  ASSERT_THAT((dynamic_cast<commands::AddMetadataCommand*>(cloned_cmd_.get()))->size(), Eq(metadata_.size()));
}

TEST_F(AddMetadataCommand, ReturnsMessageWithMetadataAdded) {
  
  ASSERT_THAT(msg->metadataSize(), Eq(0));

  msg = cloned_cmd_->execute(std::move(msg));

  ASSERT_THAT(msg->metadataSize(), Eq(2));

  ASSERT_THAT(msg->getAllMetadata(), UnorderedElementsAreArray(metadata_));
}

TEST_F(AddMetadataCommand, IfMessageContainsMetadataAdditionalMetadataWillBeAppended) {
  msg->addMetadata("existing", "data");

  ASSERT_THAT(msg->metadataSize(), Eq(1));

  msg = cloned_cmd_->execute(std::move(msg));

  ASSERT_THAT(msg->metadataSize(), Eq(3));

  ASSERT_THAT(msg->getMetadata("existing"), Eq("data"));
}

TEST_F(AddMetadataCommand, IfCommandDoesNotContainsMetadataNothingHappens) {
  std::unique_ptr<commands::Command> new_clone{cmd_.clone({})};

  msg = new_clone->execute(std::move(msg));

  ASSERT_THAT(msg->metadataSize(), Eq(0));
}

TEST_F(AddMetadataCommand, MessageCountIsIncrementedAfterSuccesfullExecution) {
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
  msg = cloned_cmd_->execute(std::move(msg));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(1));
  msg = cloned_cmd_->execute(std::move(msg));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(2));
}