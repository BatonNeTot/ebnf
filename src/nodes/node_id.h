#ifndef node_id_h
#define node_id_h

#include "node_literal.h"

namespace ebnf {

	class NodeId : public NodeLiteral {
	public:
		explicit NodeId(const std::string& id);

		std::string toStr() const override;

		Node* nextChild(const StateInfo& state) const override;

		bool updateStr(std::string_view& source) const override;

		const std::string& body() const override;

		std::unique_ptr<Token> token(const std::string_view&) const override;

		bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const override;

		void fetch(const Ebnf& ebnf) override;

	private:

		Node* node() const;

		Node* _node = nullptr;
	};
}

#endif // !node_id_h
