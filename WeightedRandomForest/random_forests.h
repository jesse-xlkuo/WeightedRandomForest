/*
 * random_forests.h
 *
 *  Created on: 31 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#ifndef RANDOM_FORESTS_H_
#define RANDOM_FORESTS_H_
#include<vector>
#include"decision_tree.h"
#include"training_set.h"
#include"node.h"
#include<ctime>
#include<cstdlib>
#include<cstdio>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<iomanip>
/*
 * following included file is from boost library and is for serialization
 */
#include <boost/serialization/utility.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

class RandomForests{
private:
	// for serialization using boost
	friend class boost::serialization::access;

	static int seed_num_;
	TrainingSet* training_set_;
	int trees_num_;

	vector<DecisionTree*> random_forests_;
	vector<int> node_num_;  // the node num of the tree

	vector<vector<int> > bagging_set_;
	vector<vector<bool> > selected_status_;

	double OOB_error_rate_;
	double strength_;
	double correlation_;
	double c_s2_;
	/*
	 * following code is for serialization
	 */
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & random_forests_;
		ar & trees_num_;
		ar & OOB_error_rate_ & strength_ & correlation_ & c_s2_;
	}



public:
	void save_random_forests(const RandomForests &random_forests,
			const char* filename) {
		std::ofstream ofs(filename);
		boost::archive::text_oarchive oa(ofs);
		oa << random_forests;
	}

	void load_random_forests(RandomForests &random_forests,
			const char* filename) {
		ifstream ifs(filename);
		boost::archive::text_iarchive ia(ifs);
		ia >> random_forests;
	}

	static int get_seed_() {
		return RandomForests::seed_num_;
	}
	static void inc_seed_() {
		RandomForests::seed_num_++;
	}
	RandomForests();
	RandomForests(TrainingSet* training_set,int tree_num);
	vector<DecisionTree*> get_trees();
	void GenerateRF();
	int PredictClass(TrainingSet* training_set,int tuple);
	double GetErrorRate(TrainingSet* training_set,vector<int> training_set_index);
	vector<vector<int> > GetRandomTrainingSet();
	void GetEachTreeErrorRate(TrainingSet* training_set,vector<int> training_set_index);
	void PrintTrees();
	vector<vector<int> > get_bagging_set();

	int get_trees_num();
	vector<vector<bool> > get_selected_status_();
	vector<int> GetRandomSubSpace(vector<int> attribute_list);

	/*
	 * set and get oob estimated information,it is temporary
	 */
	void set_OOB_error_rate_(double rate);
	double get_OOB_error_rate_();
	void set_strength_(double strength);
	double get_strength();
	void set_correlation_(double correlation);
	double get_correlation_();
	void set_c_s2_(double c_s2);
	double get_c_s2_();
	void PredictClassToFile(TrainingSet* training_set, AttributeValueMapper* attribute_value_mapper, string file_name);





};

#endif
