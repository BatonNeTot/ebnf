#ifndef node_any_h
#define node_any_h

#include "nodes/node_base.h"

namespace ebnf {

	class NodeAny : public NodeBase {
	public:
		std::string toStr() const override;

		Node* nextChild(const StateInfo&) const override;

		bool updateStr(std::string_view& source) const override;

		const std::string& body() const override;

		std::unique_ptr<Token> token(const std::string_view& source) const override;

		bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const override;
	};
}

#endif // !node_any_h
