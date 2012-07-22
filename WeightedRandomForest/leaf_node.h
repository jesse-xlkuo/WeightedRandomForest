/*
 * leaf_node.h
 *
 *  Created on: 20 Feb, 2012
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#ifndef LEAF_NODE_H_
#define LEAF_NODE_H_

#include"utility.h"
#include"node.h"
using namespace std;

class LeafNode:public Node{
private:
	int class_;
	double purity_;
public:
	LeafNode();
	void set_class_(int class_);
	int get_class_();
	void set_purity_(double purity);
	double get_purity_();
};

#endif 
