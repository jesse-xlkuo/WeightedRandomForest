/*
 * leaf_node.cpp
 *
 *  Created on: 20 Feb, 2012
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#include"leaf_node.h"

LeafNode::LeafNode(){
	this->set_type_(LEAFNODE);
}

void LeafNode::set_class_(int class_){
	this->class_=class_;
}
int LeafNode::get_class_(){
	return this->class_;
}

void LeafNode::set_purity_(double purity){
	this->purity_ = purity;
}

double LeafNode::get_purity_(){
	return this->purity_;
}
