#ifndef TOYBOX_TINYTOPK_POSTINGLIST_CONTAINER_H
#define TOYBOX_TINYTOPK_POSTINGLIST_CONTAINER_H

#include <map>

#include "posting_list.h"

namespace toybox {

class PostingListContainer {
  public:
    PostingListContainer();
    ~PostingListContainer();
    int Insert(docid_t doc, const termvec_t &term_vec);
    int Delete(docid_t doc);
    docid_t max_docid() const { return max_docid_; };
    PostingListIterator* CreateIterator(termid_t termid);
  private:
    std::map<termid_t, PostingList*> pl_map_;
    docid_t max_docid_;
    PostingListContainer(const PostingListContainer&);
    void operator=(const PostingListContainer&);
};

} // namespace toybox

#endif // TOYBOX_TINYTOPK_POSTINGLIST_CONTAINER_H
