#pragma once

#include <algorithm>
#include <optional>
#include <cassert>
#include <vector>
#include <queue>
#include "common.h"

namespace cht {

template <class KeyType>
class CompactHistTree {
 public:
  CompactHistTree() = default;

  CompactHistTree(KeyType min_key, KeyType max_key, size_t num_keys,
              size_t num_bins, size_t log_num_bins, size_t max_error, size_t shift,
              std::vector<uint32_t> table)
      : min_key_(min_key),
        max_key_(max_key),
        num_keys_(num_keys),
        num_bins_(num_bins),
        log_num_bins_(log_num_bins),
        max_error_(max_error),
        shift_(shift),
        table_(std::move(table)) {}

	// Returns a search bound [`begin`, `end`) around the estimated position.
  SearchBound GetSearchBound(const KeyType key) const {
    const size_t begin = Lookup(key);
		// `end` is exclusive.
    const size_t end = (begin + max_error_ + 1 > num_keys_) ? num_keys_ : (begin + max_error_ + 1);
    return SearchBound{begin, end};
	}
        
  // Returns the size in bytes.
  size_t GetSize() const {
    return sizeof(*this) + table_.size() * sizeof(unsigned);
  }

 private:
	static constexpr unsigned LEAF = (1u << 31);
	static constexpr unsigned MASK = LEAF - 1;
	
	// Lookup `key` in tree
  size_t Lookup(KeyType key) const {
		// Edge cases
		if (key <= min_key_) return 0;
		if (key >= max_key_) return num_keys_;
		key -= min_key_;
		
		auto width = shift_;
		auto next = 0;
		do {
			// Get the bin
			auto bin = key >> width;
			next = table_[next + bin];

			// Is it a leaf?
			if (next & LEAF)
				return next & MASK;
			
			// Prepare for the next level
			key -= bin << width;
			width -= log_num_bins_;
		} while (true);
  }
	 
	const KeyType min_key_;
  const KeyType max_key_;
  const size_t num_keys_;
  const size_t num_bins_;
	const size_t log_num_bins_;
	const size_t max_error_;
	const size_t shift_;
		
  std::vector<unsigned> table_;
};

} // namespace cht
