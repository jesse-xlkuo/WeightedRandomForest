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
#include"internal_node.h"
#include"leaf_node.h"
#include<ctime>
#include<cstdlib>
#include<cstdio>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<iomanip>
using namespace std;

class RandomForests{
private:
	static int seed_num_;
	TrainingSet* training_set_;
	int trees_num_;
	vector<DecisionTree*> random_forests_;
	vector<int> node_num_;  // the node num of the tree
	vector<vector<int> > bagging_set_;
	vector<vector<bool> > selected_status_;
public:
	static int get_seed_() {
		return RandomForests::seed_num_;
	}
	static void inc_seed_() {
		RandomForests::seed_num_++;
	}
	RandomForests(TrainingSet* training_set,int tree_num);
	vector<DecisionTree*> get_trees();
	void GenerateRF();
	int PredictClass(TrainingSet* training_set,int tuple);
	double GetErrorRate(TrainingSet* training_set,vector<int> training_set_index);
	vector<vector<int> > GetRandomTrainingSet();
	void GetEachTreeErrorRate(TrainingSet* training_set,vector<int> training_set_index);
	void PrintTrees();
	vector<vector<int> > get_bagging_set();

	/********��Ԫ���Ժ���**************/
	void TestGetRandomTrainingSet();
	int get_trees_num();
	vector<vector<bool> > get_selected_status_();
	vector<int> GetRandomSubSpace(vector<int> attribute_list);

};

#endif
