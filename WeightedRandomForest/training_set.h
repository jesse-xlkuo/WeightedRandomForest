/*
 * training_set.h
 *
 *  Created on: 29 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#ifndef TRAINING_SET_H_
#define TRAINING_SET_H_
#include"utility.h"
#include<map>
#include"attribute_value_mapper.h"
#include<sstream>
#include<iostream>
using namespace std;

class TrainingSet{
private:
	string source_name_;
	AttributeValue** value_matrix_;
	AttributeValueMapper attribute_value_mapper_;
	int training_set_num_;
public:

	void set_attribute_value_mapper(AttributeValueMapper);
	int get_training_set_num_();
	void TestMatrix();
	int GetClassifyAttribute();
	AttributeType GetAttributeType(int);
	int GetAttributeNum();
	vector<int> GetNormalAttributes();
	int GetAttributeValueNum(int);
	map<int,double> GetClassDistribution(vector<int>);
	int GetTheMostClass(vector<int>);
	bool IsTrainingSetSameClass(vector<int>);
	map<int,vector<int> > SplitByDiscreteAttribute(vector<int>,int);
	map<int,vector<int> > SplitByPositon(vector<int>,int);
	void DeleteOneAttribute(vector<int>&,int);
	string GetAttributeName(int attribute);
	string GetAttributeValueName(int attribute,int index);
	vector<int> GetClassesNum(vector<int> training_set_index);
	AttributeValue** GetValueMatrixP();

	/*
	 * added in 4/6/2012
	 * the DataFileReader is duplicate,we can generate the training set when the
	 * file data is reading.
	 */
	StatusCode ProduceTrainingSetMatrix(string file_path);
	StatusCode split(const string&,char,vector<string>&);


	/*
	 * added for decision tree result file
	 */
	string GetAllDAttributeValue(int attribute);
	void set_source_name_(string source_name);
	string get_source_name_();
	string GetAllClassName();
	
	/*
	 * V2.0
	 */
	//TrainingSet(string DF);

};
#endif
