#include "parser.h"

#include "exp_parser.h"

#include <list>

namespace ebnf {

	Ebnf::IdInfo Parser::proceedNext() {
		_currentId = Ebnf::IdInfo();

		auto assignOffset = _source.find('=', _carret);
		auto assignPos = assignOffset;
		if (assignPos == std::string::npos) {
			throwError("Couldn't find assignment operator");
			return destroyAndReturn();
		}

		++assignOffset;

		if (_source[assignPos - 1] == '$') {
			_currentId.type = Ebnf::IdInfo::Type::Base;
			--assignPos;
		}

		if (_source[assignPos - 1] == '~') {
			_currentId.type = Ebnf::IdInfo::Type::Boilerplate;
			--assignPos;
		}

		static const std::string spaceCharacters = " \f\n\r\t\v";
		auto leftTrimmedId = _source.find_first_not_of(spaceCharacters, _carret);
		auto rightTrimmedId = _source.find_last_not_of(spaceCharacters, assignPos - 1) + 1;
		_currentId.id = _source.substr(leftTrimmedId, rightTrimmedId - leftTrimmedId);

		if (_currentId.id.empty()) {
			throwError("Empty declared id");
			return destroyAndReturn();
		}

		if (_currentId.id.find_first_of(spaceCharacters) != std::string::npos) {
			throwError("Id contains space");
			return destroyAndReturn();
		}

		auto expression = std::string_view(_source).substr(assignOffset);
		ExpParser expParser(_currentId);
		auto expSize = expParser.fillInfo(expression);
		_carret = assignOffset + expSize + 1;

		return destroyAndReturn();
	}

	bool Parser::hasNext() const {
		return _source.length() > _carret;
	}

	bool Parser::throwError(const std::string& message /*= ""*/) {
		_errorFlag = true;
		_errorMsg = message;
		return true;
	}

}