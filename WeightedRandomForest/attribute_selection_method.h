/*
 * attribute_selection_method.h
 *
 *  Created on: 2 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#ifndef  ATTRIBUTE_SELECTION_METHOD_H_
#define ATTRIBUTE_SELECTION_METHOD_H_

#include"utility.h"
#include"training_set.h"
#include<iostream>
#include<vector>
using namespace std;

class AttributeSelectionMethod {
protected:
	static TrainingSet* training_set_;
	vector<int> training_set_index_;
	vector<int> attribute_list_;
	int attribute_;
	double split_value_result_;
	map<int,vector<int> > splited_training_set_result_;
	int case_num_;
	double attribute_info_gain_;  // added for result data file
public:
	AttributeSelectionMethod(TrainingSet*,vector<int>,vector<int>);
	static void set_training_set_(TrainingSet* training_set);
	static TrainingSet* get_training_set_();
	vector<int> get_training_set_index_();
	vector<int> get_attribute_list_();
	void set_attribute_(int);
	int get_attribute_();
	void set_split_value_result_(double);
	double get_split_value_result_();
	void set_splited_training_set_result_(map<int,vector<int> >);
	map<int,vector<int> > get_splited_training_set_result_();
	AttributeSelectionResult GetSelectionResult();
	void set_case_num_(int case_num);
	int get_case_num_();

	void set_attribute_info_gain_(double info_gain);
	double get_attribute_info_gain_();
};

#endif


