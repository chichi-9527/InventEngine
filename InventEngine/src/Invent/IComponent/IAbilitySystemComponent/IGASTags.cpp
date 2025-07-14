#include "IGASTags.h"

namespace INVENT
{
	static ITagTrie gas_tag_trie;


	ITagTrie& IGASTags::GetGASTagTrie()
	{
		return gas_tag_trie;
	}

}