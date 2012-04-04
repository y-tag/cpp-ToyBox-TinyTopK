#ifndef TOYBOX_TINYTOPK_TINYWANDSCORER_H
#define TOYBOX_TINYTOPK_TINYWANDSCORER_H

#include <map>

#include "posting_list.h"
#include "tiny_scorer.h"

namespace toybox {

class TinyWANDScorer : public TinyScorer {
  public:
    TinyWANDScorer()  {};
    virtual ~TinyWANDScorer() {};
    virtual int Retrieve(PostingListContainer *pl_container,
                         const termvec_t &term_vec,
                         int k, double min_thresh, docvec_t *docvec);
  private:
    TinyWANDScorer(const TinyWANDScorer&);
    void operator=(const TinyWANDScorer&);
};

} // namespace toybox

#endif // TOYBOX_TINYTOPK_TINYWANDSCORER_H
