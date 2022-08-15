#include "exp_lexer.h"

namespace ebnf {

	ExpLexer::Token ExpLexer::proceedNext() {
		char itSymbol;
		do {
			itSymbol = nextSymbol();
		} while (itSymbol && !proceedSymbol(itSymbol) && hasNext());

		if (_errorFlag) {
			return { _errorMsg };
		}

		auto token = _token;
		_token = {};
		return token;
	}

	bool ExpLexer::hasNext() const {
		return _source.length() > _carret && (_state == State::Literal || !isExpEnd(_source[_carret]));
	}

	char ExpLexer::nextSymbol() {
		if (!hasNext()) {
			throwError("Unexpected end of a string");
			return '\0';
		}
		return _source.at(_carret++);
	}

	bool ExpLexer::proceedSymbol(char symbol) {
		switch (_state) {
		case State::Default:
			return proceedDefault(symbol);
		case State::Id:
			return proceedId(symbol);
		case State::Literal:
			return proceedLiteral(symbol);
		default:
			return throwError(std::string("Unexpected state value: ") + std::to_string(static_cast<uint8_t>(_state)));
		}
	}

	bool ExpLexer::proceedDefault(char symbol) {
		if (isSpace(symbol)) {
			return false;
		}

		_token.type = decideTokenType(symbol);
		if (_errorFlag) {
			return true;
		}

		switch (_token.type) {
		case Token::Type::Id:
			_state = State::Id;
			return proceedId(symbol);
		case Token::Type::Literal:
			_state = State::Literal;
			return false;
		default:
			_token.value += symbol;
			return true;
		}
	}

	bool ExpLexer::proceedId(char symbol) {
		if (isSpace(symbol)) {
			return finishToken();
		}

		if (isQuote(symbol)
			|| isOr(symbol)
			|| isLeftBracket(symbol)
			|| isRightBracket(symbol)
			|| isLeftSquareBracket(symbol)
			|| isRightSquareBracket(symbol)
			|| isLeftCurlyBracket(symbol)
			|| isRightCurlyBracket(symbol)) {
			return backspaceCarret();
		}

		if (!(_token.value.empty()
			? &isProperStartingIdSymbol
			: &isProperIdSymbol
			)(symbol)) {
			return throwError(std::string("Unsupported symbol used in id: '") + symbol + '\'');
		}

		_token.value += symbol;

		if (!hasNext()) {
			return finishToken();
		}

		return false;
	}
	bool ExpLexer::proceedLiteral(char symbol) {
		if (isQuote(symbol)) {
			return finishToken();
		}

		_token.value += symbol;
		return false;
	}

	bool ExpLexer::isExpEnd(char symbol) {
		return symbol == '.';
	}

	bool ExpLexer::isSpace(char symbol) {
		return symbol == ' ';
	}

	bool ExpLexer::isOr(char symbol) {
		return symbol == '|';
	}

	bool ExpLexer::isLeftBracket(char symbol) {
		return symbol == '(';
	}
	bool ExpLexer::isRightBracket(char symbol) {
		return symbol == ')';
	}
	bool ExpLexer::isLeftSquareBracket(char symbol) {
		return symbol == '[';
	}
	bool ExpLexer::isRightSquareBracket(char symbol) {
		return symbol == ']';
	}
	bool ExpLexer::isLeftCurlyBracket(char symbol) {
		return symbol == '{';
	}
	bool ExpLexer::isRightCurlyBracket(char symbol) {
		return symbol == '}';
	}

	bool ExpLexer::isQuote(char symbol) {
		return symbol == '\'' || symbol == '"';
	}

	bool ExpLexer::isProperIdSymbol(char symbol) {
		return isProperStartingIdSymbol(symbol)
			|| symbol == '-'
			|| (symbol >= '0' && symbol <= '9');
	}
	bool ExpLexer::isProperStartingIdSymbol(char symbol) {
		return symbol == '_'
			|| (symbol >= 'a' && symbol <= 'z')
			|| (symbol >= 'A' && symbol <= 'Z');
	}

	ExpLexer::Token::Type ExpLexer::decideTokenType(char symbol) {
		if (isProperStartingIdSymbol(symbol)) {
			return Token::Type::Id;
		}
		if (isQuote(symbol)) {
			return Token::Type::Literal;
		}
		if (isOr(symbol)) {
			return Token::Type::Or;
		}

		if (isLeftBracket(symbol)) {
			return Token::Type::LeftBracket;
		}
		if (isRightBracket(symbol)) {
			return Token::Type::RightBracket;
		}
		if (isLeftSquareBracket(symbol)) {
			return Token::Type::LeftSquareBracket;
		}
		if (isRightSquareBracket(symbol)) {
			return Token::Type::RightSquareBracket;
		}
		if (isLeftCurlyBracket(symbol)) {
			return Token::Type::LeftCurlyBracket;
		}
		if (isRightCurlyBracket(symbol)) {
			return Token::Type::RightCurlyBracket;
		}

		throwError(std::string("Unsupported symbol: '") + symbol + '\'');
		return Token::Type::Id;
	}

	bool ExpLexer::backspaceCarret() {
		--_carret;
		return finishToken();
	}

	bool ExpLexer::finishToken() {
		auto output = !_token.value.empty();
		if (output) {
			_state = State::Default;
		}
		return output;
	}

	bool ExpLexer::throwError(const std::string& message /*= ""*/) {
		_errorFlag = true;
		_errorMsg = message;
		return true;
	}

}