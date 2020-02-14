#include <iostream>
#include <vector>

// DBoW2
#include "DBoW2.h" // defines OrbVocabulary and OrbDatabase

#include <DUtils/DUtils.h>
#include <DVision/DVision.h>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>


using namespace DBoW2;
using namespace DUtils;
using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void loadFeatures(const char *imageDIR, vector<vector<cv::Mat > > &features);
void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out);
void testVocCreation(const vector<vector<cv::Mat > > &features);
void testDatabase(const vector<vector<cv::Mat > > &features);
void testVocCreation(const vector<vector<cv::Mat > > &featuresSRC, const vector<vector<cv::Mat > > &featuresDEST);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// number of training images
const int NIMAGES = 10;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void wait()
{
  cout << endl << "Press enter to continue" << endl;
  getchar();
}

// ----------------------------------------------------------------------------
vector<vector<cv::Mat > > features1;
vector<vector<cv::Mat > > features2;

void aibrainTest1(void)
{
	cout << "aibrainTest1... start" << endl;
	loadFeatures("images1/", features1);	
	testVocCreation(features1);	
	wait();
	// testDatabase(features1);
	cout << "aibrainTest1... end" << endl;
}

void aibrainTest2(void)
{
	cout << "aibrainTest2... start" << endl;
	loadFeatures("images2/", features2);	
	testVocCreation(features1, features2);	
	wait();
	// testDatabase(features2);
	cout << "aibrainTest2... end" << endl;
}

// 原始的代码
void aibrainTest(void)
{
	cout << "aibrainTest... start" << endl;
	loadFeatures("images/", features1);	
	testVocCreation(features1);	
	wait();
	testDatabase(features1);
	cout << "aibrainTest1... end" << endl;
}

int main()
{
#if 0
	// 原始的代码
	aibrainTest();
#else
	// 测试代码
	aibrainTest1();
	aibrainTest2();
#endif
	return 0;
}

// ----------------------------------------------------------------------------

void loadFeatures(const char *imageDIR, vector<vector<cv::Mat > > &features)
{
  features.clear();
  features.reserve(NIMAGES);

  cv::Ptr<cv::ORB> orb = cv::ORB::create();

  cout << "Extracting ORB features..." << endl;
  for(int i = 0; i < NIMAGES; ++i)
  {
    stringstream ss;
    ss << imageDIR << i << ".png";
	cout << "ss: " << ss.str() << endl;

    cv::Mat image = cv::imread(ss.str(), 0);
    cv::Mat mask;
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    orb->detectAndCompute(image, mask, keypoints, descriptors);

    features.push_back(vector<cv::Mat >());
    changeStructure(descriptors, features.back());
  }
}

// ----------------------------------------------------------------------------

void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out)
{
  out.resize(plain.rows);

  for(int i = 0; i < plain.rows; ++i)
  {
    out[i] = plain.row(i);
  }
}

// ----------------------------------------------------------------------------
// 测试用featuresSRC的特征值去匹配图片featuresDEST
void testVocCreation(const vector<vector<cv::Mat > > &featuresSRC, const vector<vector<cv::Mat > > &featuresDEST)
{
  // branching factor and depth levels
  const int k = 10;
  const int L = 5;
  const WeightingType weight = TF_IDF;
  const ScoringType score = L1_NORM;

  OrbVocabulary voc(k, L, weight, score);

  cout << "Creating a " << k << "^" << L << " vocabulary..." << endl;
  voc.create(featuresSRC);
  cout << "... done!" << endl;

  cout << "Vocabulary information: " << endl
  << voc << endl << endl;

  // lets do something with this vocabulary
  cout << "Matching images against themselves: " << endl;
  BowVector v1, v2;
  for(int i = 0; i < NIMAGES; i++)
  {
    voc.transform(featuresSRC[i], v1);
    for(int j = 0; j < NIMAGES; j++)
    {
      voc.transform(featuresDEST[j], v2);

      double score = voc.score(v1, v2);
      cout << "Image " << i << " vs Image " << j << ": " << score << endl;
    }
  }

  // save the vocabulary to disk
  cout << endl << "Saving vocabulary..." << endl;
  voc.save("voc.yml.gz");
  voc.saveToTextFile("Myvoc.txt");
  cout << "Done" << endl;
}


void testVocCreation(const vector<vector<cv::Mat > > &features)
{
  // branching factor and depth levels
  const int k = 10;
  const int L = 5;
  const WeightingType weight = TF_IDF;
  const ScoringType score = L1_NORM;

  OrbVocabulary voc(k, L, weight, score);

  cout << "Creating a " << k << "^" << L << " vocabulary..." << endl;
  voc.create(features);
  cout << "... done!" << endl;

  cout << "Vocabulary information: " << endl
  << voc << endl << endl;

  // lets do something with this vocabulary
  cout << "Matching images against themselves: " << endl;
  BowVector v1, v2;
  for(int i = 0; i < NIMAGES; i++)
  {
    voc.transform(features[i], v1);
    for(int j = 0; j < NIMAGES; j++)
    {
      voc.transform(features[j], v2);

      double score = voc.score(v1, v2);
      cout << "Image " << i << " vs Image " << j << ": " << score << endl;
    }
  }

  // save the vocabulary to disk
  cout << endl << "Saving vocabulary..." << endl;
  voc.save("voc.yml.gz");
  voc.saveToTextFile("Myvoc.txt");
  cout << "Done" << endl;
}

// ----------------------------------------------------------------------------

void testDatabase(const vector<vector<cv::Mat > > &features)
{
  cout << "Creating a database..." << endl;

  // load the vocabulary from disk
  OrbVocabulary voc("voc.yml.gz");

  OrbDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

  // add images to the database
  for(int i = 0; i < NIMAGES; i++)
  {
    db.add(features[i]);
  }

  cout << "... done!" << endl;

  cout << "Database information: " << endl << db << endl;

  // and query the database
  cout << "Querying the database: " << endl;

  QueryResults ret;
  for(int i = 0; i < NIMAGES; i++)
  {
    db.query(features[i], ret, 4);

    // ret[0] is always the same image in this case, because we added it to the
    // database. ret[1] is the second best match.

    cout << "Searching for Image " << i << ". " << ret << endl;
  }

  cout << endl;

  // we can save the database. The created file includes the vocabulary
  // and the entries added
  cout << "Saving database..." << endl;
  db.save("db_voc.yml.gz");
  cout << "... done!" << endl;

  // once saved, we can load it again
  cout << "Retrieving database once again..." << endl;
  OrbDatabase db2("db_voc.yml.gz");
  cout << "... done! This is: " << endl << db2 << endl;
}
