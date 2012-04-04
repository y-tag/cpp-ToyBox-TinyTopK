#ifndef TOYBOX_TINYTOPK_POSTING_LIST_H
#define TOYBOX_TINYTOPK_POSTING_LIST_H

#include <climits>

#include <algorithm>
#include <map>
#include <vector>

namespace toybox {

typedef unsigned int termid_t;
typedef unsigned int docid_t;
typedef double   payload_t;

typedef std::vector<std::pair<termid_t, payload_t> > termvec_t;
typedef std::vector<std::pair<docid_t, payload_t> > docvec_t;

#define NO_MORE_DOCS UINT_MAX

} // namespace


namespace {

bool first_less_than(const std::pair<toybox::docid_t, toybox::payload_t> &left,
                     const std::pair<toybox::docid_t, toybox::payload_t> &right) {
  return left.first < right.first;
}

} // namespace


namespace toybox {

class PostingList;
class PostingListIterator;

class PostingList {
  public:
    PostingList();
    ~PostingList() {};
    PostingListIterator* CreateIterator();
    int Insert(docid_t doc, payload_t payload);
    int Delete(docid_t doc);
    const std::vector<std::pair<docid_t, payload_t> > &list() const { return list_; };
    docid_t max_docid() const { return max_docid_; }
    payload_t upper_bound() const { return upper_bound_; }
    size_t size() const { return list_.size(); }
  private:
    std::vector<std::pair<docid_t, payload_t> > list_;
    docid_t max_docid_;
    payload_t upper_bound_;
    PostingList(const PostingList&);
    void operator=(const PostingList&);
};

class PostingListIterator {
  public:
    PostingListIterator(PostingList *list_ptr)
      : list_ptr_(list_ptr), list_itr_(list_ptr->list().begin()) {
    }
    ~PostingListIterator() {};
    docid_t Next() {
      if (list_itr_ == list_ptr_->list().end()) { return NO_MORE_DOCS; }
      ++list_itr_;
      if (list_itr_ == list_ptr_->list().end()) { return NO_MORE_DOCS; }
      return list_itr_->first;
    }
    docid_t AdvanceGTE(docid_t id) {
      list_itr_ = std::lower_bound(list_itr_,
                                   list_ptr_->list().end(),
                                   std::make_pair(id, payload_t()),
                                   first_less_than);
      if (list_itr_ == list_ptr_->list().end()) { return NO_MORE_DOCS; }
      return list_itr_->first;
    }
    docid_t docid() const {
      if (list_itr_ == list_ptr_->list().end()) { return NO_MORE_DOCS; }
      return list_itr_->first;
    }
    payload_t payload() const {
      if (list_itr_ == list_ptr_->list().end()) { return payload_t(); }
      return list_itr_->second;
    }
    payload_t upper_bound() const {
      return list_ptr_->upper_bound();
    }
    docid_t max_docid() const {
      return list_ptr_->max_docid();
    }
    size_t size() const {
      return list_ptr_->size();
    }
  private:
    PostingList *list_ptr_;
    std::vector<std::pair<docid_t, payload_t> > ::const_iterator list_itr_;
    PostingListIterator();
    PostingListIterator(const PostingListIterator&);
    void operator=(const PostingListIterator&);
};

} // namespace toybox

#endif // TOYBOX_TINYTOPK_POSTLING_LIST_H
