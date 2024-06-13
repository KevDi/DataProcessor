#include "KafkaSenderCommand/KafkaSenderCommand.hpp"
#include "KafkaSenderCommand/VersionInfo_KafkaSenderCommand.h"
#include <Core/Message.hpp>
#include <Poco/NumberParser.h>
#include "KafkaProducerWrapperImpl.hpp"

namespace commands {
  std::string KafkaSenderCommand::version() {
    return std::string{ PRODUCT_VERSION_FULL_STR };
  }

  KafkaSenderCommand::KafkaSenderCommand(const std::string& name) : Command( name ) {
  }

  void KafkaSenderCommand::setProducerWrapper(
    kafkaproducer::KafkaProducerWrapperPtr producer_wrapper_ptr) {
    producer_wrapper_ = producer_wrapper_ptr;
  }

  size_t KafkaSenderCommand::errorCountKafkaException() const {
    return error_count_kafka_exception_;
  }

  void KafkaSenderCommand::checkPreconditionsImpl(core::MessageRawPtr message) {
    validateConfig();
    if (!producer_wrapper_) {
      throw std::runtime_error("Kafka producer is not set");
    }
  }

  KafkaSenderCommand* KafkaSenderCommand::cloneImpl() {
    return new KafkaSenderCommand{ *this };
  }

  void KafkaSenderCommand::configureImpl(commandsConfig config) {
    config_ = config;

    checkConfigHasKey(CFG_CMD_KAFKA_SENDER_RETRIES);
    max_retries_ = Poco::NumberParser::parseUnsigned(config.at(CFG_CMD_KAFKA_SENDER_RETRIES));

    validateKafkaProps();

    if (!producer_wrapper_) {
      producer_wrapper_ = std::make_unique<kafkaproducer::KafkaProducerWrapperImpl>();
    }

    if (producer_wrapper_) {
      producer_wrapper_->initialize(kafka_properties_);
    }
  }
  
  core::MessagePtr KafkaSenderCommand::executeImpl(core::MessagePtr message) {
    for (size_t i = 0; i <= max_retries_; i++)
    {
      try
      {
        producer_wrapper_->sendSync(config_[CFG_CMD_KAFKA_SENDER_TOPIC], message->id(), message->content(), message->getAllMetadata());
        incMessageCount();
        return message;
      }
      catch (const kafka::KafkaException&)
      {
        error_count_kafka_exception_++;
        if (i >= max_retries_) {
          throw;
        }
      }
    }

    return message;
  }

  void KafkaSenderCommand::checkConfigHasKey(const std::string& key) {
    if (config_.find(key) == config_.end()) {
      throw std::invalid_argument("Argument '" + key + "' is missing");
    }
  }

  void KafkaSenderCommand::validateConfig() {
    checkConfigHasKey(CFG_CMD_KAFKA_SENDER_TOPIC);
    if (config_.at(CFG_CMD_KAFKA_SENDER_TOPIC).empty()) {
      throw std::invalid_argument("No topic provided");
    }
  }

  void KafkaSenderCommand::copyConfigKeyToKafkaProps(const std::string& key) {
    kafka_properties_.put(key, config_.at(key));
  }

  void KafkaSenderCommand::validateKafkaProps() {
    checkConfigHasKey(CFG_CMD_KAFKA_SENDER_HOSTS);
    checkConfigHasKey(CFG_CMD_KAFKA_SENDER_SECURITY);
    checkConfigHasKey(CFG_CMD_KAFKA_SENDER_CA_LOCATION);
    checkConfigHasKey(CFG_CMD_KAFKA_SENDER_CERT_LOCATION);
    checkConfigHasKey(CFG_CMD_KAFKA_SENDER_KEY_LOCATION);
    checkConfigHasKey(CFG_CMD_KAFKA_SENDER_KEY_PASSWORD);

    // Set Kafka Properties
    copyConfigKeyToKafkaProps(CFG_CMD_KAFKA_SENDER_HOSTS);
    copyConfigKeyToKafkaProps(CFG_CMD_KAFKA_SENDER_SECURITY);
    copyConfigKeyToKafkaProps(CFG_CMD_KAFKA_SENDER_CA_LOCATION);
    copyConfigKeyToKafkaProps(CFG_CMD_KAFKA_SENDER_CERT_LOCATION);
    copyConfigKeyToKafkaProps(CFG_CMD_KAFKA_SENDER_KEY_LOCATION);
    copyConfigKeyToKafkaProps(CFG_CMD_KAFKA_SENDER_KEY_PASSWORD);
    kafka_properties_.put("enable.idempotence", "true");
    kafka_properties_.put("message.timeout.ms", "86400000");
  }

}  // namespace commands
