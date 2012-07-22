/*
 * attribute_value_mapper.h
 *
 *  Created on: 8 Feb, 2012
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#ifndef ATTRIBUTE_VALUE_MAPPER_H_
#define ATTRIBUTE_VALUE_MAPPER_H_

#include"utility.h"
#include<cstdio>
#include<cstdlib>
#include<map>
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

class AttributeValueMapper{
private:
	string file_path;
	int attribute_num_;
	vector<AttributeType> attribute_type_;
	map<string,int> attribute_name_mapper_;
	map<int,map<string,int> > attribute_value_mapper_;
	int classify_attribute_;
public:
	AttributeValueMapper();
	AttributeValueMapper(string file_path);
	StatusCode split(const string &str,char delim,vector<string> &strVec);
	bool HaveASpace(const string& str);
	int GetAttributeNum();
	AttributeType GetAttributeType(int attribute);
	bool GetMapperValue(int attribute, string value,map<string,int>::iterator &it);
	int GetClassifyAttribute();
	int GetAttributeValueNum(int attribute);
	string GetAttributeName(int attribute);
	string GetAttributeValueName(int attribute,int index);
	StatusCode MapAttributeValue();
    void DeleteComment(string& str);
	void DeleteSymbol(string& str);
	void test();
};
#endif
