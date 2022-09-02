#include "exp_parser.h"

#include "exp_lexer.h"

#include <list>

namespace ebnf {

	size_t ExpParser::fillInfo(const std::string_view& expression) {
		_lastNode = nullptr;
		_lastHolder = nullptr;

		ExpLexer lexer(expression);

		std::list<ExpLexer::Token> tokens;
		while (lexer.hasNext()) {
			auto token = lexer.proceedNext();
			tokens.emplace_back(token);

			switch (token.type) {
			case ExpLexer::Token::Type::Id: {
				auto node = create<NodeId>(token.value);
				proceedNode(node);
				_lastNode = node;
				break;
			}
			case ExpLexer::Token::Type::Literal: {
				auto node = create<NodeLiteral>(token.value);
				proceedNode(node);
				_lastNode = node;
				break;
			}
			case ExpLexer::Token::Type::Or: {
				if (!_lastNode) {
					throwError("Unexpected or operator");
					return 0;
				}

				Ebnf::Node* holder;
				if (!_lastNode->parent()) {
					auto node = create<NodeOr>();
					node->add(_lastNode);
					_info.tree = node;
					holder = node;
				}
				else {
					holder = _lastNode;
					if (dynamic_cast<NodeConcat*>(holder->parent())) {
						holder = holder->parent();
					}
					if (dynamic_cast<NodeOr*>(holder->parent())) {
						holder = holder->parent();
					}
					else {
						auto node = create<NodeOr>();
						if (holder->parent()) {
							auto parent = static_cast<NodeHolder*>(holder->parent());
							parent->pop();
							parent->insert(node);
						}
						else {
							_info.tree = node;
						}
						node->add(holder);
						holder = node;
					}
				}
				_lastHolder = holder;
				_lastNode = nullptr;
				break;
			}
			case ExpLexer::Token::Type::LeftBracket: {
				_brackets.emplace(Bracket::Regular);
				auto node = create<NodeBracket>();
				proceedNode(node);
				_lastHolder = node;
				_lastNode = nullptr;
				break;
			}
			case ExpLexer::Token::Type::RightBracket: {
				if (_lastHolder || _brackets.top() != Bracket::Regular) {
					throwError("Unexpected closing bracket");
					return 0;
				}

				if (_lastNode == nullptr) {
					throwError("TODO");
					return 0;
				}

				_brackets.pop();
				_lastNode = _lastNode->parent();
				if (dynamic_cast<NodeConcat*>(_lastNode)) {
					_lastNode = _lastNode->parent();
				}
				if (dynamic_cast<NodeOr*>(_lastNode)) {
					_lastNode = _lastNode->parent();
				}

				break;
			}
			case ExpLexer::Token::Type::LeftSquareBracket: {
				_brackets.emplace(Bracket::Square);
				auto node = create<NodeConditional>();
				proceedNode(node);
				_lastHolder = node;
				_lastNode = nullptr;
				break;
			}
			case ExpLexer::Token::Type::RightSquareBracket: {
				if (_lastHolder || _brackets.top() != Bracket::Square) {
					throwError("Unexpected closing square bracket");
					return 0;
				}

				_brackets.pop();
				_lastNode = _lastNode->parent();
				if (dynamic_cast<NodeConcat*>(_lastNode)) {
					_lastNode = _lastNode->parent();
				}
				if (dynamic_cast<NodeOr*>(_lastNode)) {
					_lastNode = _lastNode->parent();
				}

				break;
			}
			case ExpLexer::Token::Type::LeftCurlyBracket: {
				_brackets.emplace(Bracket::Curly);
				auto node = create<NodeRepetition>();
				proceedNode(node);
				_lastHolder = node;
				_lastNode = nullptr;
				break;
			}
			case ExpLexer::Token::Type::RightCurlyBracket: {
				if (_lastHolder || _brackets.top() != Bracket::Curly) {
					throwError("Unexpected closing curly bracket");
					return 0;
				}

				_brackets.pop();
				_lastNode = _lastNode->parent();
				if (dynamic_cast<NodeConcat*>(_lastNode)) {
					_lastNode = _lastNode->parent();
				}
				if (dynamic_cast<NodeOr*>(_lastNode)) {
					_lastNode = _lastNode->parent();
				}

				break;
			}
			}
		}

		return lexer.carret();
	}

	void ExpParser::proceedNode(Ebnf::Node* node) {
		if (!_lastNode) {
			if (!_info.tree) {
				_info.tree = node;
			}
			if (_lastHolder) {
				auto* container = static_cast<NodeHolder*>(_lastHolder);
				container->insert(node);
				_lastHolder = nullptr;
			}
		}
		else if (!_lastNode->parent()) {
			auto container = create<NodeConcat>();
			container->add(_lastNode);
			container->add(node);
			_info.tree = container;
		}
		else if (auto parContainer = dynamic_cast<NodeConcat*>(_lastNode->parent())) {
			parContainer->add(node);
		}
		else {
			auto holder = static_cast<NodeHolder*>(_lastNode->parent());
			auto container = create<NodeConcat>();
			holder->pop();
			holder->insert(container);

			container->add(_lastNode);
			container->add(node);
		}
	}

	bool ExpParser::throwError(const std::string& message /*= ""*/) {
		_errorFlag = true;
		_errorMsg = message;
		return true;
	}

}