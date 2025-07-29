#pragma once

#include <zadeh/zadeh.h>
#include <zadeh/filter.h>
#include <zadeh/StringArrayFilterer.h>

template <class ArrayType, class ElementType>
inline std::vector<size_t> FuzzyFindIndices(const ArrayType &candidates, const std::string &query)
{
	zadeh::StringArrayFilterer<ArrayType, ArrayType, ElementType> filterer{};
	filterer.set_candidates(candidates);
	return filterer.filter_indices(query);
}
