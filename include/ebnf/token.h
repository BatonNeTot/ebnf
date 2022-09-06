#ifndef ebnf_token_h
#define ebnf_token_h

#include <string>
#include <vector>
#include <memory>

namespace ebnf {

	class Ebnf;

	struct Token {
		Token() = default;
		Token(const std::string& value);
		Token(const std::string& id, const std::string& value);
		Token(uint64_t line, uint64_t offset);

		Token(Token& token) = delete;
		Token(const Token& token) = delete;
		Token(Token&& token);

		Token& operator= (Token&& token);

		std::string toStr() const;

		void compress(const Ebnf& ebnf);

		void fetch(const Ebnf& ebnf);

		std::string id;
		std::string value;
		uint64_t line = 0;
		uint64_t offset = 0;
		std::vector<std::unique_ptr<Token>> parts;
	};

}

#endif // !ebnf_token_h
