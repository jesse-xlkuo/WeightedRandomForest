/*
 * decision_tree.h
 *
 *  Created on: 31 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */
#ifndef DECISION_TREE_H_
#define DECISION_TREE_H_
#include"utility.h"
#include"node.h"
#include"training_set.h"
#include"attribute_selection_method.h"
#include"c4_5_attribute_selection_method.h"
#include"leaf_node.h"
#include"internal_node.h"
#include<vector>
#include<iostream>
#include<fstream>
using namespace std;

struct ChildInfo {
	InternalNode* parent_;
	int split_;
	vector<int> attributes_;
	vector<int> training_set_;
};

class DecisionTree{
private:
	static int seed_num_;
	Node* root_;
public:
	static int get_seed_() {
		return DecisionTree::seed_num_;
	}
	static void inc_seed_() {
		DecisionTree::seed_num_++;
	}
	void set_root_(Node* root);
	Node* get_root_();
	Node* GenerateDecisionTreeByC4_5(TrainingSet* training_set,vector<int> training_set_index,vector<int> attribute_list);
	//Node* GenerateDecisionTreeByC4_5(TrainingSet* training_set,vector<int> training_set_index,vector<int> attribute_list,int current_height);
	Node* GenerateDecisionTreeByC4_5(TrainingSet* training_set,vector<ChildInfo> child_info_vec);
	void TraverseTree(ofstream& f,TrainingSet* training_set,Node* decision_tree);
	Node* PredictClass(TrainingSet* training_set,int index,Node* root);
	double GetErrorRate(TrainingSet* training_set,vector<int> training_set_index);
	const static int max_depth_ = 1000;

	// the following function is for print the decison tree in a data file
//	void PrintLeafNode(ofstream& f, TrainingSet* training_set, Node* node);
//	void PrintInternalNode();
//	void PrintEdge();
//	void PrintDecisionTreeInfo();
	vector<int> GetRandomSubSpace(vector<int> attribute_list);
	int GetParentTheMostClass(TrainingSet* training_set, Node* parent, vector<ChildInfo> child_info_vec);
};

#endif
