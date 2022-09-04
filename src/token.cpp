#include "ebnf/token.h"

#include "ebnf/ebnf.h"

namespace ebnf {

	Token::Token(const std::string & value_)
		: value(value_) {}
	Token::Token(const std::string & id_, const std::string & value_)
		: id(id_)
		, value(value_) {}
	Token::Token(uint64_t line_, uint64_t offset_)
		: line(line_)
		, offset(offset_) {}

	Token::Token(Token && token)
		: id(std::move(token.id))
		, value(std::move(token.value))
		, line(token.line)
		, offset(token.offset)
		, parts(std::move(token.parts)) {}

	Token& Token::operator=(Token && token) {
		id = std::move(token.id);
		value = std::move(token.value);
		line = token.line;
		offset = token.offset;
		parts = std::move(token.parts);
		return *this;
	}

	std::string Token::toStr() const {
		if (!value.empty()) {
			return value;
		}

		std::string str;

		for (auto& part : parts) {
			str += part->toStr();
		}

		return str;
	}

	void Token::compress(const Ebnf& ebnf) {
		if (!value.empty()) {
			return;
		}

		if (!id.empty()) {
			auto idType = ebnf.getIdType(id);
			switch (idType) {
			case Ebnf::IdType::Base: {
				value = toStr();
				parts.clear();
				return;
			}
			case Ebnf::IdType::Boilerplate: {
				value.clear();
				parts.clear();
				return;
			}
			}
		}

		auto tempParts = std::move(parts);
		std::string tempValue;
		for (auto& part : tempParts) {
			part->compress(ebnf);

			if (!part->id.empty()) {
				if (part->value.empty() && part->parts.empty()) {
					continue;
				}

				if (!tempValue.empty()) {
					parts.emplace_back(std::make_unique<Token>(tempValue));
					tempValue.clear();
				}

				parts.emplace_back(std::move(part));
				continue;
			}

			if (!part->value.empty()) {
				tempValue += part->value;
				continue;
			}

			if (!part->parts.empty() && !tempValue.empty()) {
				parts.emplace_back(std::make_unique<Token>(tempValue));
				tempValue.clear();
			}

			for (auto& childPart : part->parts) {
				parts.emplace_back(std::move(childPart));
			}
		}

		if (!tempValue.empty()) {
			if (!parts.empty()) {
				parts.emplace_back(std::make_unique<Token>(tempValue));
				tempValue.clear();
			}
			else {
				value = tempValue;
			}
		}
	}
}