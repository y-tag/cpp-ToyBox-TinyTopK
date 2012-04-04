#include "tiny_daat_scorer.h"

#include <cstdio>

#include <algorithm>

#include "posting_list.h"

namespace {

bool second_greater(const std::pair<toybox::docid_t, toybox::payload_t> &left,
                    const std::pair<toybox::docid_t, toybox::payload_t> &right) {
  return left.second > right.second;
}
bool itr_docid_greater(
    const std::pair<toybox::PostingListIterator*, toybox::payload_t> &left,
    const std::pair<toybox::PostingListIterator*, toybox::payload_t> &right
) {
  return left.first->docid() > right.first->docid();
}

inline void down_front_of_heap(
  std::vector<std::pair<toybox::PostingListIterator*, toybox::payload_t> > &heap
) {
  size_t heap_size = heap.size();

  if (heap_size < 2) { return; }
  std::swap(heap[0], heap[heap_size - 1]);

  size_t i = 0;
  size_t j = 1;
  while (j < heap_size - 1) {
    if (j + 1 < heap_size - 1 && heap[j].first->docid() > heap[j + 1].first->docid() &&
        heap[i].first->docid() > heap[j + 1].first->docid()) {
      std::swap(heap[i], heap[j + 1]); i = j + 1;
    } else if (heap[i].first->docid() > heap[j].first->docid()) {
      std::swap(heap[i], heap[j]); i = j;
    } else {
      break;
    }
    j = 2 * i + 1;
  }

  i = heap_size - 1;
  j = (i - 1) / 2;
  while (j >= 0) {
    if (heap[i].first->docid() < heap[j].first->docid()) {
      std::swap(heap[i], heap[j]); i = j;
      if (i == 0) { break; }
      j = (i - 1) / 2;
    } else {
      break;
    }
  }
}


} // namespace

namespace toybox {

int TinyDAATScorer::Retrieve(PostingListContainer *pl_container,
                             const termvec_t &term_vec,
                             int k, double min_thresh, docvec_t *docvec) {
  if (pl_container == NULL || docvec == NULL || k <= 0) { return 0; }

  std::map<termid_t, PostingList*>::iterator itr;
  std::map<docid_t, payload_t>::iterator score_map_itr;
  PostingListIterator *posting_list_itr = NULL;

  std::vector<std::pair<PostingListIterator*, payload_t> > pl_itr_heap;

  for (termvec_t::const_iterator tvitr = term_vec.begin();
       tvitr != term_vec.end(); ++tvitr) {
    posting_list_itr = pl_container->CreateIterator(tvitr->first);
    if (posting_list_itr == NULL) { continue; }

    pl_itr_heap.push_back(std::make_pair(posting_list_itr, tvitr->second));
  }

  if (pl_itr_heap.size() == 0U) { return 0; }

  std::make_heap(pl_itr_heap.begin(), pl_itr_heap.end(), itr_docid_greater);

  if (pl_itr_heap.front().first->docid() == NO_MORE_DOCS) { return 0; }

  int docvec_size = docvec->size();
  docid_t prev_docid = docid_t();
  payload_t payload_sum = payload_t();
  while (pl_itr_heap.front().first->docid() != NO_MORE_DOCS) {
    if (pl_itr_heap.front().first->docid() > prev_docid) {
      if (docvec_size < k && payload_sum > min_thresh) {
        docvec->push_back(std::make_pair(prev_docid, payload_sum)); 
        std::push_heap(docvec->begin(), docvec->end(), second_greater);
        ++docvec_size;
      } else if (docvec_size >= k && payload_sum > docvec->front().second) {
        std::pop_heap(docvec->begin(), docvec->end(), second_greater);
        docvec->pop_back();
        docvec->push_back(std::make_pair(prev_docid, payload_sum)); 
        std::push_heap(docvec->begin(), docvec->end(), second_greater);
      }
      payload_sum = payload_t();
    }

    payload_sum += pl_itr_heap.front().first->payload() * pl_itr_heap.front().second;
    prev_docid = pl_itr_heap.front().first->docid();

    pl_itr_heap.front().first->Next();
    down_front_of_heap(pl_itr_heap);
  }

  if (docvec_size < k && payload_sum > min_thresh) {
    docvec->push_back(std::make_pair(prev_docid, payload_sum)); 
    std::push_heap(docvec->begin(), docvec->end(), second_greater);
    ++docvec_size;
  } else if (docvec_size >= k && payload_sum >= docvec->front().second) {
    std::pop_heap(docvec->begin(), docvec->end(), second_greater);
    docvec->pop_back();
    docvec->push_back(std::make_pair(prev_docid, payload_sum)); 
    std::push_heap(docvec->begin(), docvec->end(), second_greater);
  }

  std::sort_heap(docvec->begin(), docvec->end(), second_greater);

  for (size_t i = 0; i < pl_itr_heap.size(); ++i) {
    delete pl_itr_heap[i].first; pl_itr_heap[i].first = NULL;
  }
  pl_itr_heap.clear();

  return docvec_size;
}

} // namespace toybox

