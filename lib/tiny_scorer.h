#ifndef TOYBOX_TINYTOPK_TINYSCORER_H
#define TOYBOX_TINYTOPK_TINYSCORER_H

#include "posting_list.h"
#include "posting_list_container.h"

namespace toybox {

class TinyScorer {
  public:
    virtual ~TinyScorer() {};
    virtual int Retrieve(PostingListContainer *pl_container,
                         const termvec_t &term_vec,
                         int k, double min_thresh, docvec_t *docvec) = 0;
  protected:
    TinyScorer() {};
  private:
    TinyScorer(const TinyScorer&);
    void operator=(const TinyScorer&);
};

} // namespace toybox

#endif // TOYBOX_TINYTOPK_TINYSCORER_H
