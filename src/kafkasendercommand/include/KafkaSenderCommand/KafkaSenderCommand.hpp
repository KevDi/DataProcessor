#pragma once
#include "export.hpp"
#include <kafka/Properties.h>
#include <Commands/Command.hpp>

namespace kafka::clients::producer {
  class KafkaProducer;
}

namespace commands {

  namespace kafkaproducer {

    class KafkaProducerWrapper;

  }

  class KAFKASENDERCOMMAND_EXPORT KafkaSenderCommand : public Command {
  public:

    static std::string version();

    // Config keys

    // Name of the producer topic
    inline static const std::string CFG_CMD_KAFKA_SENDER_TOPIC{"topic"};
    // Max. retries on fail send message to kafka
    inline static const std::string CFG_CMD_KAFKA_SENDER_RETRIES{"retries"};
    // Initial list of brokers as a CSV list of broker host or host:port
    inline static const std::string CFG_CMD_KAFKA_SENDER_HOSTS{"bootstrap.servers"};
    // Protocol used to communicate with brokers
    //   e.g. 'ssl'
    inline static const std::string CFG_CMD_KAFKA_SENDER_SECURITY{"security.protocol"};
    // File or directory path to CA certificate(s) for verifying the broker's key
    inline static const std::string CFG_CMD_KAFKA_SENDER_CA_LOCATION{"ssl.ca.location"};
    // Path to client's public key (PEM) used for authentication
    inline static const std::string CFG_CMD_KAFKA_SENDER_CERT_LOCATION{"ssl.certificate.location"};
    // Path to client's private key (PEM) used for authentication
    inline static const std::string CFG_CMD_KAFKA_SENDER_KEY_LOCATION{"ssl.key.location"};
    //Private key passphrase
    inline static const std::string CFG_CMD_KAFKA_SENDER_KEY_PASSWORD{"ssl.key.password"};

    explicit KafkaSenderCommand(const std::string& name = "KafkaSenderCommand");
    void setProducerWrapper(std::shared_ptr<kafkaproducer::KafkaProducerWrapper> producer_wrapper_ptr);

    size_t errorCountKafkaException() const;

  private:
    KafkaSenderCommand(KafkaSenderCommand&&) = delete;
    KafkaSenderCommand(const KafkaSenderCommand&) = default;
    KafkaSenderCommand& operator=(const KafkaSenderCommand&) = delete;
    KafkaSenderCommand& operator=(KafkaSenderCommand&&) = delete;
    void checkPreconditionsImpl(core::Message* message) override;
    KafkaSenderCommand* cloneImpl() override;
    void configureImpl(commandsConfig config) override;
    std::unique_ptr<core::Message> executeImpl(std::unique_ptr<core::Message> message) override;

    void checkConfigHasKey(const std::string& key);
    void validateConfig();
    void copyConfigKeyToKafkaProps(const std::string& key);
    void validateKafkaProps();

    commandsConfig config_{};
    kafka::Properties kafka_properties_{};
    size_t max_retries_{};
    std::shared_ptr<kafkaproducer::KafkaProducerWrapper> producer_wrapper_;

    size_t error_count_kafka_exception_{ 0 };
  };

}  // namespace commands