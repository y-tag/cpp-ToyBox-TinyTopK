#ifndef TOYBOX_TINYTOPK_TINYTAATSCORER_H
#define TOYBOX_TINYTOPK_TINYTAATSCORER_H

#include <map>

#include "posting_list.h"
#include "tiny_scorer.h"

namespace toybox {

class TinyTAATScorer : public TinyScorer {
  public:
    TinyTAATScorer() {};
    virtual ~TinyTAATScorer() {};
    virtual int Retrieve(PostingListContainer *pl_container,
                         const termvec_t &term_vec,
                         int k, double min_thresh, docvec_t *docvec);
  private:
    TinyTAATScorer(const TinyTAATScorer&);
    void operator=(const TinyTAATScorer&);
};

} // namespace toybox

#endif // TOYBOX_TINYTOPK_TINYTAATSCORER_H
