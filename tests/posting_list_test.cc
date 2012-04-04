#include <cfloat>

#include <vector>

#include <cppunit/extensions/HelperMacros.h>

#include "posting_list.h"

namespace toybox {

class PostingListTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(PostingListTest);
  CPPUNIT_TEST(testInsert);
  CPPUNIT_TEST(testDelete);
  CPPUNIT_TEST(testNext);
  CPPUNIT_TEST(testAdvanceGTE);
  CPPUNIT_TEST(testGetUpperBound);
  CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();
    void tearDown();
    void testInsert();
    void testDelete();
    void testNext();
    void testAdvanceGTE();
    void testGetUpperBound();
  private:
    std::vector<PostingList*> posting_lists_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PostingListTest);

void PostingListTest::setUp() {
  posting_lists_.push_back(new PostingList());
}

void PostingListTest::tearDown() {
  for (size_t i = 0; i < posting_lists_.size(); ++i) {
    delete posting_lists_[i];
    posting_lists_[i] = NULL;
  }
  posting_lists_.clear();
}

void PostingListTest::testInsert() {
  for (size_t i = 0; i < posting_lists_.size(); ++i) {
    PostingList *pl = posting_lists_[i];

    pl->Insert(1, 2.1);
    pl->Insert(3, 4.2);
    pl->Insert(2, 5.9);
    pl->Insert(5, 1.7);

    PostingListIterator *itr = pl->CreateIterator();
    CPPUNIT_ASSERT_EQUAL(docid_t(5), itr->max_docid());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(payload_t(5.9), itr->upper_bound(), DBL_EPSILON);

    CPPUNIT_ASSERT_EQUAL(docid_t(1),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(2.1), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(2),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(5.9), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(3),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(4.2), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(1.7), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS == itr->Next());

    // update the value
    pl->Insert(1, 1.2);
    pl->Insert(3, 2.4);

    delete itr; itr = pl->CreateIterator();

    CPPUNIT_ASSERT_EQUAL(docid_t(1),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(1.2), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(2),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(5.9), itr->payload());

    // update the value
    pl->Insert(2, 9.5);
    pl->Insert(5, 7.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(payload_t(9.5), itr->upper_bound(), DBL_EPSILON);

    CPPUNIT_ASSERT_EQUAL(docid_t(2),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(9.5), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(3),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(2.4), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(7.1), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS == itr->Next());

    delete itr; itr = NULL;
  }

}

void PostingListTest::testDelete() {
  for (size_t i = 0; i < posting_lists_.size(); ++i) {
    PostingList *pl = posting_lists_[i];

    pl->Insert(1, 2.1);
    pl->Insert(3, 4.2);
    pl->Insert(2, 2.9);
    pl->Insert(5, 1.7);

    PostingListIterator *itr = pl->CreateIterator();
    CPPUNIT_ASSERT_EQUAL(docid_t(5), itr->max_docid());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(payload_t(4.2), itr->upper_bound(), DBL_EPSILON);

    CPPUNIT_ASSERT_EQUAL(docid_t(1),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(2.1), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(2),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(2.9), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(3),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(4.2), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(1.7), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS == itr->Next());

    // delete the value
    pl->Delete(3);
    CPPUNIT_ASSERT_EQUAL(docid_t(5), itr->max_docid());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(payload_t(2.9), itr->upper_bound(), DBL_EPSILON);
    pl->Delete(1);
    CPPUNIT_ASSERT_EQUAL(docid_t(5), itr->max_docid());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(payload_t(2.9), itr->upper_bound(), DBL_EPSILON);

    delete itr; itr = pl->CreateIterator();

    CPPUNIT_ASSERT_EQUAL(docid_t(2),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(2.9), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(1.7), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS == itr->Next());

    delete itr; itr = NULL;
  }
}

void PostingListTest::testNext() {
  for (size_t i = 0; i < posting_lists_.size(); ++i) {
    PostingList *pl = posting_lists_[i];

    pl->Insert(1, 2.1);
    pl->Insert(3, 4.2);
    pl->Insert(2, 5.9);
    pl->Insert(7, 4.3);
    pl->Insert(8, 2.0);
    pl->Insert(5, 1.7);

    PostingListIterator *itr = pl->CreateIterator();

    CPPUNIT_ASSERT_EQUAL(docid_t(1),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(2.1), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(2),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(5.9), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(3),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(4.2), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(1.7), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(7),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(4.3), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS != itr->Next());

    CPPUNIT_ASSERT_EQUAL(docid_t(8),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(2.0), itr->payload());
    CPPUNIT_ASSERT(NO_MORE_DOCS == itr->Next());

    delete itr; itr = NULL;
  }
}

void PostingListTest::testAdvanceGTE() {
  for (size_t i = 0; i < posting_lists_.size(); ++i) {
    PostingList *pl = posting_lists_[i];

    pl->Insert(1, 2.1);
    pl->Insert(3, 4.2);
    pl->Insert(2, 5.9);
    pl->Insert(7, 4.3);
    pl->Insert(8, 2.0);
    pl->Insert(5, 1.7);

    PostingListIterator *itr = pl->CreateIterator();

    CPPUNIT_ASSERT_EQUAL(docid_t(1),     itr->AdvanceGTE(1));
    CPPUNIT_ASSERT_EQUAL(docid_t(1),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(2.1), itr->payload());

    CPPUNIT_ASSERT_EQUAL(docid_t(3),     itr->AdvanceGTE(3));
    CPPUNIT_ASSERT_EQUAL(docid_t(3),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(4.2), itr->payload());

    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->AdvanceGTE(4));
    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(1.7), itr->payload());

    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->AdvanceGTE(5));
    CPPUNIT_ASSERT_EQUAL(docid_t(5),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(1.7), itr->payload());

    CPPUNIT_ASSERT_EQUAL(docid_t(7),     itr->AdvanceGTE(6));
    CPPUNIT_ASSERT_EQUAL(docid_t(7),     itr->docid());
    CPPUNIT_ASSERT_EQUAL(payload_t(4.3), itr->payload());

    CPPUNIT_ASSERT_EQUAL(NO_MORE_DOCS,   itr->AdvanceGTE(9));
    CPPUNIT_ASSERT_EQUAL(NO_MORE_DOCS,   itr->docid());

    delete itr; itr = NULL;
  }
}

void PostingListTest::testGetUpperBound() {
  for (size_t i = 0; i < posting_lists_.size(); ++i) {
    PostingList *pl = posting_lists_[i];

    PostingListIterator *itr = pl->CreateIterator();
    CPPUNIT_ASSERT_EQUAL(payload_t(), itr->upper_bound());

    pl->Insert(1, 2.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(payload_t(2.1), itr->upper_bound(), DBL_EPSILON);

    pl->Insert(3, 4.2);
    pl->Insert(2, 5.9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(payload_t(5.9), itr->upper_bound(), DBL_EPSILON);

    pl->Insert(7, 4.3);
    pl->Insert(8, 2.0);
    pl->Insert(5, 1.7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(payload_t(5.9), itr->upper_bound(), DBL_EPSILON);

    delete itr; itr = NULL;
  }
}


} // namespace
