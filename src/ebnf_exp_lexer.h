#ifndef ebnf_exp_lexer_h
#define ebnf_exp_lexer_h

#include <string>

namespace ebnf {

	class EbnfExpLexer {
	public:

		EbnfExpLexer(const std::string_view& source)
			: _source(source) {}

		struct Token {

			enum class Type : uint8_t {
				Id,
				Literal,
				Any,
				Or,
				LeftBracket,
				RightBracket,
				LeftSquareBracket,
				RightSquareBracket,
				LeftCurlyBracket,
				RightCurlyBracket,
			};

			Token() = default;
			Token(const std::string& value_)
				: value(value_) {};

			std::string value;
			Type type = Type::Id;
		};

		Token proceedNext();

		bool hasNext() const;

		uint64_t carret() const {
			return _carret;
		}

	private:

		enum class State : uint8_t {
			Default,
			Id,
			Literal,
		};

		char nextSymbol();
		bool proceedSymbol(char symbol);

		Token::Type decideTokenType(char symbol);

		bool proceedDefault(char symbol);
		bool proceedId(char symbol);
		bool proceedLiteral(char symbol);

		static bool isExpEnd(char symbol);

		static bool isProperIdSymbol(char symbol);
		static bool isProperStartingIdSymbol(char symbol);

		static bool isSpace(char symbol);

		static bool isAny(char symbol);

		static bool isOr(char symbol);

		static bool isLeftBracket(char symbol);
		static bool isRightBracket(char symbol);
		static bool isLeftSquareBracket(char symbol);
		static bool isRightSquareBracket(char symbol);
		static bool isLeftCurlyBracket(char symbol);
		static bool isRightCurlyBracket(char symbol);

		static bool isQuote(char symbol);

		bool backspaceCarret();
		bool finishToken();

		bool throwError(const std::string& message = "");

		std::string_view _source;

		uint64_t _carret = 0;
		State _state = State::Default;
		Token _token;

		bool _errorFlag = false;
		std::string _errorMsg;
	};

}

#endif // !ebnf_exp_lexer_h
