#pragma once

#include <ranges>

namespace Utils
{
	// tries to find key in associative container
	// returns std::optional<ValueT>
	auto Lookup(const auto& container, auto&& key) {
		auto findResult = container.find(std::forward<decltype(key)>(key));
		if (findResult == container.end())
			return decltype(std::optional{findResult->second})(std::nullopt);

		return std::optional{ findResult->second };
	}

	// tries to find an item in a sequence by predicate
	// returns std::optional<T>
	auto FindIf(const auto &seq, auto &&pred) {
		auto findResult = std::ranges::find_if(seq, pred);
		if (findResult == seq.end())
			return decltype(std::optional{*findResult})(std::nullopt);

		return std::optional{ *findResult };
	}

	bool InRange(auto val, auto min, auto max) {
		return val >= min && val <= max;
	}
}
