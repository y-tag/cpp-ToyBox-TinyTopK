#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

#include <fstream>
#include <string>
#include <vector>

#include "tiny_taat_scorer.h"
#include "tiny_daat_scorer.h"
#include "tiny_wand_scorer.h"
#include "posting_list.h"

namespace {

inline double gettimeofday_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

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

int read_data_file(const std::string &file_name,
                   std::vector<toybox::termvec_t> *data_vec) {
  if (data_vec == NULL) { return -1; }

  std::ifstream ifs;
  std::string buff;

  ifs.open(file_name.c_str());
  if (! ifs.is_open()) { return -1; }

  while (getline(ifs, buff)) {
    toybox::docid_t dummy;
    toybox::termvec_t term_vec;

    parse_term_vec(buff, &dummy, &term_vec);
    data_vec->push_back(term_vec);
  }

  ifs.close();

  return 1;
}

} // namespace


int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s index_file query_file\n", argv[0]);
    return 1;
  }
  const char *index_file = argv[1];
  const char *query_file = argv[2];

  toybox::PostingListContainer *pl_container = new toybox::PostingListContainer();

  std::vector<toybox::termvec_t> index_vec;
  std::vector<toybox::termvec_t> query_vec;

  read_data_file(index_file, &index_vec);
  read_data_file(query_file, &query_vec);

  for (size_t i = 0; i < index_vec.size(); ++i) {
    pl_container->Insert(i, index_vec[i]);
  }

  fprintf(stdout, "document num: %u, query num: %u\n", index_vec.size(), query_vec.size());

  std::vector<toybox::TinyScorer*> scorer_vec;
  std::vector<std::string> scorer_name_vec;

  scorer_vec.push_back(new toybox::TinyTAATScorer()); scorer_name_vec.push_back("TAAT");
  scorer_vec.push_back(new toybox::TinyDAATScorer()); scorer_name_vec.push_back("DAAT");
  scorer_vec.push_back(new toybox::TinyWANDScorer()); scorer_name_vec.push_back("WAND");

  int topk = 100;
  double min_thresh = 0.0;
  toybox::docvec_t doc_vec;
  std::vector<double> elapsed_vec;

  for (size_t sc = 0; sc < scorer_vec.size(); ++sc) {
    elapsed_vec.push_back(0.0);
    double start_time = 0.0;

    for (size_t qr = 0; qr < query_vec.size(); ++qr) {
      doc_vec.clear();
      start_time = gettimeofday_sec();
      scorer_vec[sc]->Retrieve(pl_container, query_vec[qr], topk, min_thresh, &doc_vec);
      elapsed_vec[sc] += gettimeofday_sec() - start_time;
    }
    fprintf(stdout, "%s\t%f sec\t%f msec/req\n",
            scorer_name_vec[sc].c_str(), elapsed_vec[sc],
            1000 * elapsed_vec[sc] / query_vec.size());
  }

  for (size_t i = 0; i < scorer_vec.size(); ++i) {
    delete scorer_vec[i];
    scorer_vec[i] = NULL;
  }
  scorer_vec.clear();
  delete pl_container; pl_container = NULL;
  
  return 0;
}

