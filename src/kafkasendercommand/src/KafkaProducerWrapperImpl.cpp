#include "KafkaProducerWrapperImpl.hpp"

namespace commands::kafkaproducer {

void KafkaProducerWrapperImpl::initialize(const kafka::Properties& props) {
	producer_ = std::make_unique<kafka::clients::producer::KafkaProducer>(props);
}

void KafkaProducerWrapperImpl::sendSync(const std::string& topic, const std::string& key, const std::string& value, const core::Message::Metadata& metadata) {
	auto record = kafka::clients::producer::ProducerRecord{ topic, kafka::Key{ key.c_str(), key.size() }, kafka::Value{ value.c_str(), value.size() } };
	auto& headers = record.headers();
	for (const auto& [k, v] : metadata) {
		headers.emplace_back(k, kafka::Header::Value{v.c_str(), v.size()});
	}

  producer_->syncSend(record);
}
}
