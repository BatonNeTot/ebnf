#include "ebnf_parser.h"

#include "ebnf_exp_parser.h"

#include <list>

namespace ebnf {

	Ebnf::IdInfo EbnfParser::proceedNext() {
		_currentId = Ebnf::IdInfo();

		static const std::string spaceCharacters = " \f\n\r\t\v";
		_carret = _source.find_first_not_of(spaceCharacters, _carret);

		if (_carret == std::string::npos) {
			return destroyAndReturn();
		}

		auto assignOffset = _source.find('=', _carret);
		auto assignPos = assignOffset;
		if (assignPos == std::string::npos) {
			throwError("Couldn't find assignment operator");
			return destroyAndReturn();
		}

		++assignOffset;

		if (_source[assignPos - 1] == '$') {
			_currentId.type = Ebnf::IdType::Base;
			--assignPos;
		}

		if (_source[assignPos - 1] == '~') {
			_currentId.type = Ebnf::IdType::Boilerplate;
			--assignPos;
		}

		auto rightTrimmedId = _source.find_last_not_of(spaceCharacters, assignPos - 1) + 1;
		_currentId.id = _source.substr(_carret, rightTrimmedId - _carret);

		if (_currentId.id.empty()) {
			throwError("Empty declared id");
			return destroyAndReturn();
		}

		if (_currentId.id.find_first_of(spaceCharacters) != std::string::npos) {
			throwError("Id contains space");
			return destroyAndReturn();
		}

		auto expression = std::string_view(_source).substr(assignOffset);
		EbnfExpParser expParser(_currentId);
		auto expSize = expParser.fillInfo(expression);
		_carret = assignOffset + expSize + 1;

		return destroyAndReturn();
	}

	bool EbnfParser::hasNext() const {
		return _source.length() > _carret;
	}

	bool EbnfParser::throwError(const std::string& message /*= ""*/) {
		_errorFlag = true;
		_errorMsg = message;
		return true;
	}

}