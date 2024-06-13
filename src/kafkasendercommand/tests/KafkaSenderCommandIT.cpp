#include <filesystem>
#include <gmock/gmock.h>
#include <fstream>
#include <Commands/CommandFactory.hpp>
#include <KafkaSenderCommand/KafkaSenderCommand.hpp>
#include <Core/Message.hpp>

using namespace testing;

static const std::string COMMAND_NAME{"KafkaSenderCommand"};

class KafkaSenderCommandIT : public Test {

public:
  void SetUp() override {
    command_factory_.registerCommand(COMMAND_NAME, commands::MakeCommandPtr<commands::KafkaSenderCommand>());

    try {
      std::ifstream pw_file("cert/password.txt");
      if (pw_file.is_open()) {
        std::getline(pw_file, kafka_key_password);
      }
      config_[commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_PASSWORD] = kafka_key_password;
    } catch(std::exception) {}

  }

  void TearDown() override {
    command_factory_.clearAllCommands();
  }
    
  commands::CommandFactory& command_factory_ = commands::CommandFactory::Instance();
  std::string kafka_key_password{};

  const std::string KAFKA_TOPIC = "hop1-test-topic";
  const size_t KAFKA_MAX_RETRIES = 3;

  commands::commandsConfig config_{
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_TOPIC, KAFKA_TOPIC },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_RETRIES, std::to_string(KAFKA_MAX_RETRIES) },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_HOSTS, "ho0vm165.de.bosch.com:9093,ho0vm166.de.bosch.com:9093,ho0vm167.de.bosch.com:9093" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_SECURITY, "SSL" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_CA_LOCATION, "cert/Bosch-CA-DE.cert.pem" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_CERT_LOCATION, "cert/hop1-wolfswood-kafka-client.cert.pem" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_LOCATION, "cert/hop1-wolfswood-kafka-client.key.pem" },
    { commands::KafkaSenderCommand::CFG_CMD_KAFKA_SENDER_KEY_PASSWORD, kafka_key_password }
  };

  core::MessagePtr message_one_ = std::make_unique<core::Message>("1", "KafkaSenderCommand_KafkaSenderCommandIT_MessageOne");
  core::MessagePtr message_two_ = std::make_unique<core::Message>("2", "KafkaSenderCommand_KafkaSenderCommandIT_MessageTwo");

  };

TEST_F(KafkaSenderCommandIT, CommandReturnsCorrectName) {
  auto cmd = command_factory_.allocateCommand(COMMAND_NAME, config_);
  ASSERT_THAT(cmd.get()->name(), Eq(COMMAND_NAME));
}

TEST_F(KafkaSenderCommandIT, DISABLED_CmdSendsMessagesToTopic) {
  ASSERT_THAT(command_factory_.getNumberOfCommands(), Eq(1));

  message_one_->addMetadata("key_1", "value_1");
  message_one_->addMetadata("key_2", "value_2");
  message_one_->addMetadata("key_3", "value_3");

  auto kafka_sender_cmd = command_factory_.allocateCommand(COMMAND_NAME, config_);
  ASSERT_TRUE(kafka_sender_cmd);

  auto result_one = kafka_sender_cmd->execute(std::move(message_one_));
  ASSERT_TRUE(result_one);

  auto result_two = kafka_sender_cmd->execute(std::move(message_two_));
  ASSERT_TRUE(result_two);
}