#ifndef _ITRIETAG_
#define _ITRIETAG_

#include <unordered_map>
#include <string>
#include <vector>
#include <stack>

namespace INVENT
{
	class ITagTrie 
	{
	public:
		struct TrieNodeId
		{
			TrieNodeId(){}
			TrieNodeId(size_t id, size_t vaild)
				: Id(id)
				, Vaild(vaild)
			{}

			size_t Id = 0;
			size_t Vaild = 0;

			bool operator==(const TrieNodeId& other)
			{
				return this->Id == other.Id && this->Vaild == other.Vaild;
			}
		};

		struct TrieNode
		{
			TrieNode(const TrieNodeId& id)
				: IsEnd(false)
				, NodeId(id)
			{}

			~TrieNode()
			{
				for (auto& pair : children)
				{
					delete pair.second;
					pair.second = nullptr;
				}
			}

			bool IsEnd;
			TrieNodeId NodeId;
			std::unordered_map<std::string, TrieNode*> children;
		};

		ITagTrie();
		~ITagTrie();

		TrieNodeId Insert(const std::string& tag);
		void Erase(const std::string& tag);
		TrieNodeId Search(const std::string& tag);
		void SearchAbout(const std::string& tag, std::vector<TrieNodeId>& out_ids);


		static inline void SplitStringWithDot(const std::string& in_str, std::vector<std::string>& out_vector);
	private:
		size_t _id_counter;

		std::stack<TrieNodeId> _reuse;

		TrieNode* root;
	};
}

#endif // !_ITRIETAG_
