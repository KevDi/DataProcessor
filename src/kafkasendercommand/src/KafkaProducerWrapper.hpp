#pragma once
#include <Core/Message.hpp>
#include <kafka/KafkaProducer.h>
#include <kafka/Properties.h>

namespace commands::kafkaproducer {

class KafkaProducerWrapper {
protected:
  virtual ~KafkaProducerWrapper() = default;

  std::unique_ptr<kafka::clients::producer::KafkaProducer> producer_;

public:
  virtual void initialize(const kafka::Properties& props) = 0;
  virtual void sendSync(const std::string& topic, const std::string& key, const std::string& value, const core::Message::Metadata& metadata) = 0;

};

using KafkaProducerWrapperPtr = std::shared_ptr<KafkaProducerWrapper>;
using KafkaProducerWrapperRawPtr = KafkaProducerWrapper*;

}