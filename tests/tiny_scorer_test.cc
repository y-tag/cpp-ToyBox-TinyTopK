#include <cfloat>
#include <cstdlib>

#include <vector>

#include <cppunit/extensions/HelperMacros.h>

#include "tiny_taat_scorer.h"
#include "tiny_daat_scorer.h"
#include "tiny_wand_scorer.h"
#include "posting_list.h"

namespace {

void parse_term_vec(const std::string &str, toybox::docid_t *docid, toybox::termvec_t *term_vec) {
  std::string::size_type start_pos = 0;
  std::string::size_type end_pos;
  term_vec->clear();

  end_pos = str.find(' ');
  if (end_pos == str.npos) { return; }

  *docid = static_cast<toybox::docid_t>(strtol(str.substr(start_pos, end_pos).c_str(), NULL, 10));
  start_pos = end_pos + 1;

  while (start_pos < str.size() && end_pos != str.npos) {
    end_pos = str.find(':', start_pos);
    toybox::termid_t termid = static_cast<toybox::termid_t>(strtol(str.substr(start_pos, end_pos).c_str(), NULL, 10));

    start_pos = end_pos + 1;
    end_pos = str.find(' ', start_pos);
    toybox::payload_t payload = static_cast<toybox::payload_t>(strtod(str.substr(start_pos, end_pos).c_str(), NULL));

    term_vec->push_back(std::make_pair(termid, payload));

    start_pos = end_pos + 1;
  }

  return;
}

} // namespace

namespace toybox {

class TinyScorerTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TinyScorerTest);
  CPPUNIT_TEST(testInsertAndRetrieve);
  CPPUNIT_TEST(testDeleteAndRetrieve);
  CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();
    void tearDown();
    void testInsertAndRetrieve();
    void testDeleteAndRetrieve();
  private:
    std::vector<TinyScorer*> scorer_vector_;
    PostingListContainer *pl_container_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TinyScorerTest);

void TinyScorerTest::setUp() {
  scorer_vector_.push_back(new TinyTAATScorer());
  scorer_vector_.push_back(new TinyDAATScorer());
  scorer_vector_.push_back(new TinyWANDScorer());

  pl_container_ = new PostingListContainer();

  docid_t docid;
  termvec_t term_vec;
  std::string tv_str;

  tv_str = "1 1:2.1 2:5.9 3:4.2 5:1.7";
  parse_term_vec(tv_str, &docid, &term_vec); pl_container_->Insert(docid, term_vec);
  tv_str = "2 1:2.1";
  parse_term_vec(tv_str, &docid, &term_vec); pl_container_->Insert(docid, term_vec);
  tv_str = "3 2:5.9";
  parse_term_vec(tv_str, &docid, &term_vec); pl_container_->Insert(docid, term_vec);
  tv_str = "5 3:4.2";
  parse_term_vec(tv_str, &docid, &term_vec); pl_container_->Insert(docid, term_vec);
  tv_str = "7 5:1.7";
  parse_term_vec(tv_str, &docid, &term_vec); pl_container_->Insert(docid, term_vec);
  tv_str = "8 7:3.9";
  parse_term_vec(tv_str, &docid, &term_vec); pl_container_->Insert(docid, term_vec);
}

void TinyScorerTest::tearDown() {
  for (size_t i = 0; i < scorer_vector_.size(); ++i) {
    delete scorer_vector_[i];
    scorer_vector_[i] = NULL;
  }
  scorer_vector_.clear();

  delete pl_container_; pl_container_ = NULL;
}

void TinyScorerTest::testInsertAndRetrieve() {
  for (size_t i = 0; i < scorer_vector_.size(); ++i) {
    TinyScorer *scorer = scorer_vector_[i];

    docid_t docid;
    termvec_t term_vec;
    std::string tv_str;
    docvec_t doc_vec;
    docvec_t expected_doc_vec;
    payload_t expected;
    size_t expected_doc_num;

    // query vector
    tv_str = "0 1:1.0 2:2.0 3:3.0 5:4.0";
    parse_term_vec(tv_str, &docid, &term_vec);

    expected = 1.0 * 2.1 + 2.0 * 5.9 + 3.0 * 4.2 + 4.0 * 1.7;
    expected_doc_vec.push_back(std::make_pair(docid_t(1), payload_t(expected)));
    expected = 1.0 * 0.0 + 2.0 * 0.0 + 3.0 * 4.2 + 4.0 * 0.0;
    expected_doc_vec.push_back(std::make_pair(docid_t(5), payload_t(expected)));
    expected = 1.0 * 0.0 + 2.0 * 5.9 + 3.0 * 0.0 + 4.0 * 0.0;
    expected_doc_vec.push_back(std::make_pair(docid_t(3), payload_t(expected)));
    expected = 1.0 * 0.0 + 2.0 * 0.0 + 3.0 * 0.0 + 4.0 * 1.7;
    expected_doc_vec.push_back(std::make_pair(docid_t(7), payload_t(expected)));
    expected = 1.0 * 2.1 + 2.0 * 0.0 + 3.0 * 0.0 + 4.0 * 0.0;
    expected_doc_vec.push_back(std::make_pair(docid_t(2), payload_t(expected)));

    doc_vec.clear();
    scorer->Retrieve(pl_container_, term_vec, INT_MAX, 0.0, &doc_vec);
    expected_doc_num = 5U;
    CPPUNIT_ASSERT_EQUAL(expected_doc_num, doc_vec.size());
    for (size_t i = 0; i < expected_doc_num; ++i) {
      CPPUNIT_ASSERT_EQUAL(expected_doc_vec[i].first, doc_vec[i].first);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_doc_vec[i].second, doc_vec[i].second, FLT_EPSILON);
    }

    doc_vec.clear();
    scorer->Retrieve(pl_container_, term_vec, 3, 0.0, &doc_vec);
    expected_doc_num = 3U;
    CPPUNIT_ASSERT_EQUAL(expected_doc_num, doc_vec.size());
    for (size_t i = 0; i < expected_doc_num; ++i) {
      CPPUNIT_ASSERT_EQUAL(expected_doc_vec[i].first, doc_vec[i].first);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_doc_vec[i].second, doc_vec[i].second, FLT_EPSILON);
    }

    doc_vec.clear();
    scorer->Retrieve(pl_container_, term_vec, INT_MAX, 5.0, &doc_vec);
    expected_doc_num = 4U;
    CPPUNIT_ASSERT_EQUAL(expected_doc_num, doc_vec.size());
    for (size_t i = 0; i < expected_doc_num; ++i) {
      CPPUNIT_ASSERT_EQUAL(expected_doc_vec[i].first, doc_vec[i].first);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_doc_vec[i].second, doc_vec[i].second, FLT_EPSILON);
    }

    doc_vec.clear();
    scorer->Retrieve(pl_container_, term_vec, 3, 12.6, &doc_vec);
    expected_doc_num = 2U;
    CPPUNIT_ASSERT_EQUAL(expected_doc_num, doc_vec.size());
    for (size_t i = 0; i < expected_doc_num; ++i) {
      CPPUNIT_ASSERT_EQUAL(expected_doc_vec[i].first, doc_vec[i].first);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_doc_vec[i].second, doc_vec[i].second, FLT_EPSILON);
    }

    doc_vec.clear();
    scorer->Retrieve(pl_container_, term_vec, 3, 5.0, &doc_vec);
    expected_doc_num = 3U;
    CPPUNIT_ASSERT_EQUAL(expected_doc_num, doc_vec.size());
    for (size_t i = 0; i < expected_doc_num; ++i) {
      CPPUNIT_ASSERT_EQUAL(expected_doc_vec[i].first, doc_vec[i].first);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_doc_vec[i].second, doc_vec[i].second, FLT_EPSILON);
    }
  }

}

void TinyScorerTest::testDeleteAndRetrieve() {
  for (size_t i = 0; i < scorer_vector_.size(); ++i) {
    TinyScorer *scorer = scorer_vector_[i];

    docid_t docid;
    termvec_t term_vec;
    std::string tv_str;
    docvec_t doc_vec;
    docvec_t expected_doc_vec;
    payload_t expected;
    int i = 0;

    // query vector
    tv_str = "0 1:1.0 2:2.0 3:3.0 5:4.0";
    parse_term_vec(tv_str, &docid, &term_vec);

    doc_vec.clear();
    scorer->Retrieve(pl_container_, term_vec, INT_MAX, 0.0, &doc_vec);
    CPPUNIT_ASSERT_EQUAL(5U, doc_vec.size());

    i = 0;
    expected = 1.0 * 2.1 + 2.0 * 5.9 + 3.0 * 4.2 + 4.0 * 1.7;
    CPPUNIT_ASSERT_EQUAL(docid_t(1), doc_vec[i].first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, doc_vec[i].second, FLT_EPSILON);
    ++i;
    expected = 1.0 * 0.0 + 2.0 * 0.0 + 3.0 * 4.2 + 4.0 * 0.0;
    CPPUNIT_ASSERT_EQUAL(docid_t(5), doc_vec[i].first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, doc_vec[i].second, FLT_EPSILON);
    ++i;
    expected = 1.0 * 0.0 + 2.0 * 5.9 + 3.0 * 0.0 + 4.0 * 0.0;
    CPPUNIT_ASSERT_EQUAL(docid_t(3), doc_vec[i].first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, doc_vec[i].second, FLT_EPSILON);
    ++i;
    expected = 1.0 * 0.0 + 2.0 * 0.0 + 3.0 * 0.0 + 4.0 * 1.7;
    CPPUNIT_ASSERT_EQUAL(docid_t(7), doc_vec[i].first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, doc_vec[i].second, FLT_EPSILON);
    ++i;
    expected = 1.0 * 2.1 + 2.0 * 0.0 + 3.0 * 0.0 + 4.0 * 0.0;
    CPPUNIT_ASSERT_EQUAL(docid_t(2), doc_vec[i].first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, doc_vec[i].second, FLT_EPSILON);
    ++i;

    // Delete documents
    pl_container_->Delete(docid_t(1));
    pl_container_->Delete(docid_t(7));

    doc_vec.clear();
    scorer->Retrieve(pl_container_, term_vec, INT_MAX, 0.0, &doc_vec);
    CPPUNIT_ASSERT_EQUAL(3U, doc_vec.size());

    i = 0;
    expected = 1.0 * 0.0 + 2.0 * 0.0 + 3.0 * 4.2 + 4.0 * 0.0;
    CPPUNIT_ASSERT_EQUAL(docid_t(5), doc_vec[i].first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, doc_vec[i].second, FLT_EPSILON);
    ++i;
    expected = 1.0 * 0.0 + 2.0 * 5.9 + 3.0 * 0.0 + 4.0 * 0.0;
    CPPUNIT_ASSERT_EQUAL(docid_t(3), doc_vec[i].first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, doc_vec[i].second, FLT_EPSILON);
    ++i;
    expected = 1.0 * 2.1 + 2.0 * 0.0 + 3.0 * 0.0 + 4.0 * 0.0;
    CPPUNIT_ASSERT_EQUAL(docid_t(2), doc_vec[i].first);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, doc_vec[i].second, FLT_EPSILON);

    // Add documents one more
    tv_str = "1 1:2.1 2:5.9 3:4.2 5:1.7";
    parse_term_vec(tv_str, &docid, &term_vec); pl_container_->Insert(docid, term_vec);
    tv_str = "7 5:1.7";
    parse_term_vec(tv_str, &docid, &term_vec); pl_container_->Insert(docid, term_vec);
  }

}


} // namespace
