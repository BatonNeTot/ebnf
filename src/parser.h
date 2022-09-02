#ifndef parser_h
#define parser_h

#include "ebnf.h"

#include <string>
#include <stack>
#include <vector>
#include <list>
#include <memory>

namespace ebnf {

	class Parser {
	public:

		Parser(const std::string& source)
			: _source(source) {}

		Ebnf::IdInfo proceedNext();
		bool hasNext() const;

	private:

		inline Ebnf::IdInfo&& destroyAndReturn() {
			return std::move(_currentId);
		}

		bool throwError(const std::string& message = "");

		std::string _source;
		uint64_t _carret = 0;

		Ebnf::IdInfo _currentId;

		bool _errorFlag = false;
		std::string _errorMsg;
	};

}

#endif // !parser_h
