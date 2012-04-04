#ifndef TOYBOX_TINYTOPK_TINYDAATSCORER_H
#define TOYBOX_TINYTOPK_TINYDAATSCORER_H

#include <map>

#include "posting_list.h"
#include "tiny_scorer.h"

namespace toybox {

class TinyDAATScorer : public TinyScorer {
  public:
    TinyDAATScorer() {};
    virtual ~TinyDAATScorer() {};
    virtual int Retrieve(PostingListContainer *pl_container,
                         const termvec_t &term_vec,
                         int k, double min_thresh, docvec_t *docvec);
  private:
    TinyDAATScorer(const TinyDAATScorer&);
    void operator=(const TinyDAATScorer&);
};

} // namespace toybox

#endif // TOYBOX_TINYTOPK_TINYDAATSCORER_H
