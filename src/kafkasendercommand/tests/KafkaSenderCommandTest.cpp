#include <gmock/gmock.h>
#include <Core/Message.hpp>
#include <KafkaSenderCommand/KafkaSenderCommand.hpp>
#include "KafkaProducerWrapper.hpp"
#include "KafkaProducerWrapperMock.hpp"
#include "KafkaSenderCommand/VersionInfo_KafkaSenderCommand.h"

using namespace testing;

class KafkaSenderCommand : public Test {
public:
  const std::string COMMAND_NAME = "KafkaSenderCommand";
  const std::string KAFKA_TOPIC = "hop1-test-topic";
  const std::string KAFKA_KEY_PASSWORD = "password";
  const size_t KAFKA_MAX_RETRIES = 3;
  const size_t METRICS_SIZE = 3;

  const commands::commandsConfig config_{
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_TOPIC, KAFKA_TOPIC },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_RETRIES, std::to_string(KAFKA_MAX_RETRIES) },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_HOSTS, "ho0vm165.de.bosch.com:9093,ho0vm166.de.bosch.com:9093,ho0vm167.de.bosch.com:9093" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_SECURITY, "SSL" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_CA_LOCATION, "cert/Bosch-CA-DE.cert.pem" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_CERT_LOCATION, "cert/hop1-wolfswood-kafka-client.cert.pem" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_LOCATION, "cert/hop1-wolfswood-kafka-client.key.pem" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_PASSWORD, KAFKA_KEY_PASSWORD }
  };

  commands::KafkaSenderCommand cmd_;
  std::unique_ptr<commands::Command> cloned_cmd_;

  std::shared_ptr<KafkaProducerWrapperMock> kafka_mock_ = std::make_shared<KafkaProducerWrapperMock>();

  core::MessagePtr msg_ = std::make_unique<core::Message>("1", "Test Content");

  void SetUp() override {
    cmd_.setProducerWrapper(kafka_mock_);
  }

};

TEST_F(KafkaSenderCommand, CommandReturnsCorrectName) {
  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);

  cloned_cmd_.reset(cmd_.clone(config_));

  ASSERT_THAT(cloned_cmd_->name(), COMMAND_NAME);
}

TEST_F(KafkaSenderCommand, CommandReturnsCorrectVersion) {
  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);

  cloned_cmd_.reset(cmd_.clone(config_));

  ASSERT_THAT(cloned_cmd_->version(), PRODUCT_VERSION_FULL_STR);
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnExecutionIfTopicIsMissingInConfig) {
  auto incomplete_config = config_;
  incomplete_config.erase(commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_TOPIC);

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);

  cloned_cmd_.reset(cmd_.clone(incomplete_config));
  ASSERT_THROW(msg_ = cloned_cmd_->execute(std::move(msg_)), std::invalid_argument);  
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnExecutionIfTopicIsEmpty) {
  auto incomplete_config = config_;
  incomplete_config[commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_TOPIC] = "";

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);
  
  cloned_cmd_.reset(cmd_.clone(incomplete_config));
  ASSERT_THROW(msg_ = cloned_cmd_->execute(std::move(msg_)), std::invalid_argument);
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnCloningIfRetriesIsMissingInConfig) {
  auto incomplete_config = config_;
  incomplete_config.erase(commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_RETRIES);

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(0);
  ASSERT_THROW(cloned_cmd_.reset(cmd_.clone(incomplete_config)), std::invalid_argument);
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnCloningIfHostsIsMissingInConfig) {
  auto incomplete_config = config_;
  incomplete_config.erase(commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_HOSTS);

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(0);
  ASSERT_THROW(cloned_cmd_.reset(cmd_.clone(incomplete_config)), std::invalid_argument);
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnCloningIfSecurityTypeIsMissingInConfig) {
  auto incomplete_config = config_;
  incomplete_config.erase(commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_SECURITY);

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(0);
  ASSERT_THROW(cloned_cmd_.reset(cmd_.clone(incomplete_config)), std::invalid_argument);
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnCloningIfCaLocationIsMissingInConfig) {
  auto incomplete_config = config_;
  incomplete_config.erase(commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_CA_LOCATION);

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(0);
  ASSERT_THROW(cloned_cmd_.reset(cmd_.clone(incomplete_config)), std::invalid_argument);
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnCloningIfCertLocationIsMissingInConfig) {
  auto incomplete_config = config_;
  incomplete_config.erase(commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_CERT_LOCATION);

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(0);
  ASSERT_THROW(cloned_cmd_.reset(cmd_.clone(incomplete_config)), std::invalid_argument);
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnCloningIfKeyLocationIsMissingInConfig) {
  auto incomplete_config = config_;
  incomplete_config.erase(commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_LOCATION);

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(0);
  ASSERT_THROW(cloned_cmd_.reset(cmd_.clone(incomplete_config)), std::invalid_argument);
}

TEST_F(KafkaSenderCommand, ExceptionIsThrownOnCloningIfKeyPasswordIsMissingInConfig) {
  auto incomplete_config = config_;
  incomplete_config.erase(commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_PASSWORD);

  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(0);
  ASSERT_THROW(cloned_cmd_.reset(cmd_.clone(incomplete_config)), std::invalid_argument);
}

TEST_F(KafkaSenderCommand, SendSyncIsCalledOnCommandExecution) {
  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);
  EXPECT_CALL(*kafka_mock_, sendSync(KAFKA_TOPIC, "1", "Test Content", std::unordered_map<std::string, std::string>{{"key1","value1"},{"key2","value2"}})).Times(1);

  msg_->addMetadata("key1", "value1");
  msg_->addMetadata("key2", "value2");

  cloned_cmd_.reset(cmd_.clone(config_));
  msg_ = cloned_cmd_->execute(std::move(msg_));
}

TEST_F(KafkaSenderCommand, IfSendSyncFailsRepeatedlyOnExecutionCallWillBeRepeatedForMaxRetriesTimesAndExceptionIsThrown) {
  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);
  EXPECT_CALL(*kafka_mock_, sendSync(KAFKA_TOPIC, "1", "Test Content", std::unordered_map<std::string, std::string>{}))
  .Times(KAFKA_MAX_RETRIES + 1)
  .WillRepeatedly(Throw(kafka::KafkaException{"file", 1337, kafka::Error{}}));
  
  cloned_cmd_.reset(cmd_.clone(config_));
  ASSERT_THROW(cloned_cmd_->execute(std::move(msg_)), kafka::KafkaException);
}

TEST_F(KafkaSenderCommand, IfSendSyncFailsLessThanMaxRetriesTimesOnExecutionNoExceptionIsThrown) {
  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);
  const Expectation failed_three_times = EXPECT_CALL(*kafka_mock_, sendSync(KAFKA_TOPIC, "1", "Test Content", std::unordered_map<std::string, std::string>{}))
  .WillOnce(Throw(kafka::KafkaException{"file", 1337, kafka::Error{}}))
  .WillOnce(Throw(kafka::KafkaException{"file", 1337, kafka::Error{}}))
  .WillOnce(Throw(kafka::KafkaException{"file", 1337, kafka::Error{}}));
  EXPECT_CALL(*kafka_mock_, sendSync(KAFKA_TOPIC, "1", "Test Content", std::unordered_map<std::string, std::string>{}))
  .After(failed_three_times);

  cloned_cmd_.reset(cmd_.clone(config_));
  cloned_cmd_->execute(std::move(msg_));
}

TEST_F(KafkaSenderCommand, KafkaInitializesWithKafkaProducerImplOnCloningWithMinimumConfig) {
  commands::commandsConfig impl_config{
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_TOPIC, KAFKA_TOPIC },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_RETRIES, std::to_string(KAFKA_MAX_RETRIES) },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_HOSTS, "localhost" },
      { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_SECURITY, "" },
      { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_CA_LOCATION, "" },
      { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_CERT_LOCATION, "" },
      { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_LOCATION, "" },
      { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_PASSWORD, ""}
  };

  // Reset Producer Wrapper, to force command to initialize with WrapperImpl
  cmd_.setProducerWrapper(nullptr);
  ASSERT_NO_THROW(cloned_cmd_.reset(cmd_.clone(impl_config)));
}

TEST_F(KafkaSenderCommand, MessageCountIsIncrementedAfterSuccesfullExecution) {
  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);
  EXPECT_CALL(*kafka_mock_, sendSync(KAFKA_TOPIC, "1", "Test Content", std::unordered_map<std::string, std::string>{}))
    .Times(2);

  cloned_cmd_.reset(cmd_.clone(config_));

  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
  msg_ = cloned_cmd_->execute(std::move(msg_));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(1));
  msg_ = cloned_cmd_->execute(std::move(msg_));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(2));
}

TEST_F(KafkaSenderCommand, IfSendSyncFailsRepeatedlyOnExecutionMessageCountIsNotIncremented) {
  EXPECT_CALL(*kafka_mock_, initialize(_)).Times(1);
  EXPECT_CALL(*kafka_mock_, sendSync(KAFKA_TOPIC, "1", "Test Content", std::unordered_map<std::string, std::string>{}))
    .Times(KAFKA_MAX_RETRIES + 1)
    .WillRepeatedly(Throw(kafka::KafkaException{"file", 1337, kafka::Error{}}));

  cloned_cmd_.reset(cmd_.clone(config_));
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
  ASSERT_THROW(cloned_cmd_->execute(std::move(msg_)), kafka::KafkaException);
  ASSERT_THAT(cloned_cmd_->messageCount(), Eq(0));
}