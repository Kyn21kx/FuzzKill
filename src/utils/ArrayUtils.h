#pragma once

#include <zadeh/zadeh.h>
#include <zadeh/filter.h>
#include <zadeh/StringArrayFilterer.h>

template <class ArrayType, class ElementType>
using FuzzyStringFilter_t = zadeh::StringArrayFilterer<ArrayType, ArrayType, ElementType>;

template <class ArrayType, class ElementType>
inline std::vector<size_t> FuzzyFindIndices(FuzzyStringFilter_t<ArrayType, ElementType>* filterer, const ArrayType &candidates, const std::string &query)
{
	filterer->set_candidates(candidates);
	return filterer->filter_indices(query);
}
