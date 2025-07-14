#include "ITrieTag.h"

namespace INVENT
{
	ITagTrie::ITagTrie()
		: _id_counter(0)
	{
		root = new TrieNode(TrieNodeId(_id_counter++, 0));
	}

	ITagTrie::~ITagTrie()
	{
		delete root;
	}

	ITagTrie::TrieNodeId ITagTrie::Insert(const std::string& tag)
	{
		std::vector<std::string> words;
		ITagTrie::SplitStringWithDot(tag, words);
		auto node = root;
		for (auto& word : words)
		{
			if (node->children[word] == nullptr)
			{
				if (_reuse.empty())
					node->children[word] = new TrieNode(TrieNodeId(_id_counter++, 0));
				else
				{
					node->children[word] = new TrieNode(_reuse.top());
					_reuse.pop();
				}
			}
			node = node->children[word];
		}
		node->IsEnd = true;
		return node->NodeId;
	}

	ITagTrie::TrieNodeId ITagTrie::Erase(const std::string& tag)
	{
		std::vector<std::string> words;
		ITagTrie::SplitStringWithDot(tag, words);

		std::stack<std::pair<TrieNode*, std::string>> path;

		auto node = root;
		for (auto& word : words)
		{
			auto iter = node->children.find(word);
			if (iter == node->children.end())
				return TrieNodeId();
			path.emplace(node, word);
			node = iter->second;
		}
		if (!node->IsEnd)
			return TrieNodeId();
		node->IsEnd = false;
		++node->NodeId.Vaild;

		for (auto& it = path.top(); !path.empty();)
		{
			it = path.top();
			auto parent = it.first;
			auto& key = it.second;
			auto current = parent->children[key];
			if (current->IsEnd || !current->children.empty())
				break;

			_reuse.emplace(current->NodeId.Id, ++current->NodeId.Vaild);

			delete current;
			parent->children.erase(key);

			path.pop();
		}

		return node->NodeId;
	}

	ITagTrie::TrieNodeId ITagTrie::Search(const std::string& tag)
	{
		std::vector<std::string> words;
		ITagTrie::SplitStringWithDot(tag, words);

		auto node = root;
		for (auto& word : words)
		{
			auto it = node->children.find(word);
			if (it == node->children.end())
				return TrieNodeId();
			node = it->second;
		}
		return node->IsEnd ? node->NodeId : TrieNodeId();
	}

	void ITagTrie::SearchAbout(const std::string& tag, std::vector<TrieNodeId>& out_ids)
	{
		out_ids.clear();
		std::vector<std::string> words;
		ITagTrie::SplitStringWithDot(tag, words);

		auto node = root;
		for (auto& word : words)
		{
			auto it = node->children.find(word);
			if (it == node->children.end())
				return;
			node = it->second;
		}

		std::stack<TrieNode*> nodes;
		nodes.push(node);
		while (!nodes.empty())
		{
			auto it = nodes.top();
			nodes.pop();
			if (it->IsEnd)
				out_ids.push_back(it->NodeId);

			for (auto& pair : it->children)
			{
				nodes.push(pair.second);
			}
		}

	}

	inline void ITagTrie::SplitStringWithDot(const std::string & in_str, std::vector<std::string>&out_vector)
	{
		out_vector.clear();
		size_t start = 0, end;
		while ((end = in_str.find('.', start)) != std::string::npos)
		{
			if (end > start)
			{
				out_vector.emplace_back(in_str.substr(start, end - start));
			}
			start = end + 1;
		}
		if (start < in_str.size())
			out_vector.emplace_back(in_str.substr(start));

	}
}