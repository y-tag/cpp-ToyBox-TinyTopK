#include "tiny_taat_scorer.h"

#include <cstdio>

#include <algorithm>
#include <vector>
#include <map>

#include "posting_list.h"

namespace {

bool second_greater(const std::pair<toybox::docid_t, toybox::payload_t> &left,
                    const std::pair<toybox::docid_t, toybox::payload_t> &right) {
  return left.second > right.second;
}

} // namespace

namespace toybox {

int TinyTAATScorer::Retrieve(PostingListContainer *pl_container,
                             const termvec_t &term_vec,
                             int k, double min_thresh, docvec_t *docvec) {
  if (pl_container == NULL || docvec == NULL || k <= 0) { return 0; }

  std::vector<payload_t> score_array(pl_container->max_docid() + 1);
  std::vector<payload_t>::const_iterator score_array_itr;
  PostingListIterator *posting_list_itr = NULL;

  for (termvec_t::const_iterator tvitr = term_vec.begin();
       tvitr != term_vec.end(); ++tvitr) {
    posting_list_itr = pl_container->CreateIterator(tvitr->first);
    if (posting_list_itr == NULL) { continue; }

    while (posting_list_itr->docid() != NO_MORE_DOCS) {
      score_array[posting_list_itr->docid()] += tvitr->second * posting_list_itr->payload();
      posting_list_itr->Next();
    }

    delete posting_list_itr; posting_list_itr = NULL;
  }

  int docvec_size = docvec->size();
  double min_score = min_thresh;
  for (size_t i = 0; i < score_array.size(); ++i) {
    if (score_array[i] != payload_t() && score_array[i] >= min_score) { 
      if (docvec_size < k) {
        docvec->push_back(std::make_pair(i, score_array[i]));
        std::push_heap(docvec->begin(), docvec->end(), second_greater);
        ++docvec_size;
      } else {
        std::pop_heap(docvec->begin(), docvec->end(), second_greater);
        docvec->pop_back();
        docvec->push_back(std::make_pair(i, score_array[i]));
        std::push_heap(docvec->begin(), docvec->end(), second_greater);
      }
      if (docvec_size >= k) { min_score = docvec->front().second; }
    }
  }

  std::sort_heap(docvec->begin(), docvec->end(), second_greater);
  if (docvec_size > k) {
    docvec->resize(k); // drop elements
    return k;
  } 

  return docvec_size;
}

} // namespace toybox

