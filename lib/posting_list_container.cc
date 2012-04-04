#include "posting_list_container.h"

#include <cstdio>

#include <algorithm>

#include "posting_list.h"

namespace toybox {

PostingListContainer::PostingListContainer() : pl_map_(), max_docid_() {
}

PostingListContainer::~PostingListContainer() {
  for (std::map<termid_t, PostingList*>::iterator itr = pl_map_.begin();
       itr != pl_map_.end(); ++itr) {
    delete itr->second; itr->second = NULL;
  }
  pl_map_.clear();
}

int PostingListContainer::Insert(docid_t doc, const termvec_t &term_vec) {
  std::map<termid_t, PostingList*>::iterator itr;
  for (termvec_t::const_iterator tvitr = term_vec.begin();
       tvitr != term_vec.end(); ++tvitr) {
    itr = pl_map_.find(tvitr->first);
    if (itr == pl_map_.end()) {
      pl_map_[tvitr->first] = new PostingList();
      itr = pl_map_.find(tvitr->first);
    }
    itr->second->Insert(doc, tvitr->second);
  }

  if (doc > max_docid_) { max_docid_ = doc; }

  return 1;
}

int PostingListContainer::Delete(docid_t doc) {
  max_docid_ = docid_t();
  for (std::map<termid_t, PostingList*>::iterator itr = pl_map_.begin();
       itr != pl_map_.end(); ++itr) {
    itr->second->Delete(doc);
    if (itr->second->max_docid() > max_docid_) { max_docid_ = itr->second->max_docid(); }
  }

  return 1;
}

PostingListIterator* PostingListContainer::CreateIterator(termid_t termid) {
  std::map<termid_t, PostingList*>::iterator itr = pl_map_.find(termid);
  if (itr == pl_map_.end()) { return NULL; }
  return itr->second->CreateIterator();
}

} // namespace toybox

