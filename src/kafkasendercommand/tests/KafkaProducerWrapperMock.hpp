#pragma once
#include <gmock/gmock.h>
#include <KafkaProducerWrapper.hpp>

class KafkaProducerWrapperMock : public commands::kafkaproducer::KafkaProducerWrapper {
public:

  const std::string KAFKA_INVALID_TOPIC = "invalid-topic";

  KafkaProducerWrapperMock() {
    ON_CALL(*this, initialize).WillByDefault([this](const kafka::Properties& props) {
      std::cout << "KafkaProducerMock initializes with properties[";
      for (auto& [key, value] : props.map())
      {
        std::cout << key << "=" << value.toString() << "|";
      }
      std::cout << "]" << std::endl;
    });

    ON_CALL(*this, sendSync).WillByDefault([this](const std::string& topic, const std::string& key, const std::string& value, const core::Message::Metadata& metadata) {
      std::cout << "Record has been sent to kafka.";
      });
  }

  MOCK_METHOD(void, initialize, (const kafka::Properties&), (override));
  MOCK_METHOD(void, sendSync, (const std::string&, const std::string&, const std::string&, const core::Message::Metadata&), (override));

private:

};