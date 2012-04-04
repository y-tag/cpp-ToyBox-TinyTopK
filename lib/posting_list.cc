#include "posting_list.h"

#include <cstdio>

#include <algorithm>

namespace toybox {

PostingList::PostingList() : list_(), max_docid_(), upper_bound_() {
}

PostingListIterator* PostingList::CreateIterator() {
  return new PostingListIterator(this);
}

int PostingList::Insert(docid_t doc, payload_t payload) {
  int need_update = 0;
  const std::pair<docid_t, payload_t> &target_pair = std::make_pair(doc, payload);

  if (payload > upper_bound_) { upper_bound_ = payload; }
  if (doc > max_docid_) {
    max_docid_   = doc;    
    list_.push_back(target_pair);
    return 1;
  }

  std::vector<std::pair<docid_t, payload_t> >::iterator itr
    = std::lower_bound(list_.begin(), list_.end(), target_pair, first_less_than);

  if (itr == list_.end()) {
    list_.push_back(target_pair);
  } else if (itr->first == doc) {
    if (itr->second == upper_bound_  && upper_bound_ != payload) {
      need_update = 1;
    }
    itr->second = payload;
  } else {
    list_.insert(itr, target_pair);
  }

  if (need_update == 1) {
    itr = list_.begin();
    upper_bound_ = itr->second;
    for (; itr != list_.end(); ++itr) {
      if (itr->second > upper_bound_) { upper_bound_ = itr->second; }
    }
  }
  
  return 1;
}

int PostingList::Delete(docid_t doc) {
  int need_update = 0;
  std::vector<std::pair<docid_t, payload_t> >::iterator itr
    = std::lower_bound(list_.begin(), list_.end(), std::make_pair(doc, payload_t()), first_less_than);
  if (doc == max_docid_) { need_update = 1; }

  if (itr == list_.end()) {
    return 0;
  } else if (itr->first == doc) {
    if (itr->second == upper_bound_) { need_update = 1; }
    list_.erase(itr);
  }

  if (need_update == 1) {
    itr = list_.begin();
    max_docid_ = itr->first;
    upper_bound_ = itr->second;
    for (; itr != list_.end(); ++itr) {
      if (itr->first  > max_docid_)   { max_docid_   = itr->first;  }
      if (itr->second > upper_bound_) { upper_bound_ = itr->second; }
    }
  }
  
  return 1;
}


} // namespace toybox

