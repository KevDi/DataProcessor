#pragma once
#include "KafkaProducerWrapper.hpp"

namespace commands::kafkaproducer {

	class KafkaProducerWrapperImpl : public KafkaProducerWrapper {
	public:
		void initialize(const kafka::Properties& props) override;
		void sendSync(const std::string& topic, const std::string& key, const std::string& value, const core::Message::Metadata& metadata) override;
	};

}