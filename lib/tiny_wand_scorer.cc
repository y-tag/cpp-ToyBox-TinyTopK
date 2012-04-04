#include "tiny_wand_scorer.h"

#include <cstdio>

#include <algorithm>

#include "posting_list.h"

namespace {

bool second_greater(const std::pair<toybox::docid_t, toybox::payload_t> &left,
                    const std::pair<toybox::docid_t, toybox::payload_t> &right) {
  return left.second > right.second;
}

class DocIDLess {
  public:
    DocIDLess(toybox::docid_t *docid_array) : docid_array_(docid_array) {}
    inline bool operator() (int left, int right) {
      return docid_array_[left] < docid_array_[right];
    };
  private:
    toybox::docid_t *docid_array_;
};

} // namespace

namespace toybox {

int TinyWANDScorer::Retrieve(PostingListContainer *pl_container,
                             const termvec_t &term_vec,
                             int k, double min_thresh, docvec_t *docvec) {
  if (pl_container == NULL || docvec == NULL || k <= 0) { return 0; }

  PostingListIterator *posting_list_itr = NULL;

  std::vector<PostingListIterator*> pl_itr_vec;
  std::vector<payload_t> payload_vec;

  for (termvec_t::const_iterator tvitr = term_vec.begin();
       tvitr != term_vec.end(); ++tvitr) {
    posting_list_itr = pl_container->CreateIterator(tvitr->first);
    if (posting_list_itr == NULL) { continue; }

    pl_itr_vec.push_back(posting_list_itr);
    payload_vec.push_back(tvitr->second);
  }

  size_t pl_itr_vec_size = pl_itr_vec.size();
  if (pl_itr_vec_size == 0U) { return 0; }

  docid_t *docid_array = new docid_t[pl_itr_vec_size];
  int *termid_array = new int[pl_itr_vec_size];

  for (size_t i = 0; i < pl_itr_vec_size; ++i) {
    docid_array[i]  = pl_itr_vec[i]->docid();
    termid_array[i] = i;
  }

  DocIDLess docid_less(docid_array);
  std::sort(termid_array, termid_array + pl_itr_vec_size, docid_less);

  int docvec_size = docvec->size();
  while (docid_array[termid_array[0]] != NO_MORE_DOCS) {
    double min_score = 0.0;
    if (docvec_size >= k)            { min_score = docvec->front().second; }
    else if (min_thresh > min_score) { min_score = min_thresh; }

    docid_t front_docid = docid_array[termid_array[0]];
    payload_t payload_sum = payload_t();

    size_t i = 0;
    double ub_sum = 0.0;
    do {
      payload_sum += pl_itr_vec[termid_array[i]]->payload() * payload_vec[termid_array[i]];
      ub_sum += pl_itr_vec[termid_array[i]]->upper_bound() * payload_vec[termid_array[i]];
      ++i;
    } while (i < pl_itr_vec_size && ub_sum < min_score && docid_array[termid_array[i]] != NO_MORE_DOCS);

    docid_t pv_docid = docid_array[termid_array[i-1]];

    if (pv_docid == front_docid) {
      while (i < pl_itr_vec_size && docid_array[termid_array[i]] == front_docid) {
        payload_sum += pl_itr_vec[termid_array[i]]->payload() * payload_vec[termid_array[i]];
        ++i;
      }
      pv_docid = front_docid + 1;

      if (docvec_size < k && payload_sum > min_score) {
        docvec->push_back(std::make_pair(front_docid, payload_sum)); 
        std::push_heap(docvec->begin(), docvec->end(), second_greater);
        ++docvec_size;
      } else if (docvec_size >= k && payload_sum >= min_score) {
        std::pop_heap(docvec->begin(), docvec->end(), second_greater);
        docvec->pop_back();
        docvec->push_back(std::make_pair(front_docid, payload_sum)); 
        std::push_heap(docvec->begin(), docvec->end(), second_greater);
      }
    }

    for (size_t j = 0; j < i; ++j) {
      docid_array[termid_array[j]] = pl_itr_vec[termid_array[j]]->AdvanceGTE(pv_docid);
    }

    //std::sort(termid_array, termid_array + pl_itr_vec_size, docid_less);
    // For data in experiment, insertion sort was much faster
    for (size_t i = 1; i < pl_itr_vec_size; ++i) {
      termid_t termid_tmp = termid_array[i];
      docid_t docid_tmp = docid_array[termid_tmp];
      if (docid_array[termid_array[i - 1]] > docid_tmp) {
        size_t j = i;
        do {
          termid_array[j] = termid_array[j - 1];
        } while (--j > 0 && docid_array[termid_array[j - 1]] > docid_tmp);
        termid_array[j] = termid_tmp;
      }
    }

  }

  std::sort_heap(docvec->begin(), docvec->end(), second_greater);
  if (docvec->size() > static_cast<size_t>(k)) { docvec->resize(k); } // drop elements

  for (size_t i = 0; i < pl_itr_vec_size; ++i) {
    delete pl_itr_vec[i]; pl_itr_vec[i] = NULL;
  }
  pl_itr_vec.clear();

  delete [] docid_array;  docid_array  = NULL;
  delete [] termid_array; termid_array = NULL;

  return static_cast<int>(docvec->size());
}

} // namespace toybox

