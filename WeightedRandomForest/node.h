/*
 * node.h
 *
 *  Created on: 31 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#ifndef NODE_H_
#define NODE_H_
#include"utility.h"

/*
 * following included file is from boost library and is for serialization
 */
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

class Node{
private:
	// for serialization using boost
	friend class boost::serialization::access;

	NodeType type_;
	int case_num_;   //added for result file
	int id_;   //added for result file
	int level_; //the level which the node in

	/*
	 * following attributes is for internal node
	 */
	int attribute_;
	double split_value_;
	vector<Node*> child_node_vec_;
	double info_gain_; //added for result file

	/*
	 * following attributes is for leaf node
	 */
	int class_;
	double purity_;

//	/*
//	 *the following is added for BFS
//	 */
//	Node* parent_;
//	Node* right_sibling_;
//	vector<int> attribute_list_;
//	vector<int> training_set_index_;
	/*
	 * serialization
	 */
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & type_  & attribute_ & split_value_;
		ar & child_node_vec_;
		ar & class_;
	}


public:
	static int node_num_;
	Node();
	void set_type_(NodeType);
	NodeType GetNodeType();
	void set_case_num_(int case_num);
	int get_case_num_();
	int get_id_();

	void set_attribute_(int attribute);
	int get_attribute_();
	void set_split_value(double split_value);
	double get_split_value();
	void set_child_node_(int, Node*);
	vector<Node*> get_all_child_nodes_();
	Node* get_child_node_(int value);

	/*
	 * the following function is for result data file
	 */
	void set_info_gain_(double info_gain);
	double get_info_gain();

	void set_class_(int class_);
	int get_class_();
	void set_purity_(double purity);
	double get_purity_();



	/*
	 * the following is added for BFS
	 */
//	void set_level_(int level);
//	int get_level_();
//	void set_parent_(Node* parent);
//	Node* get_parent_();
//	void set_right_sibling_(Node* right_sibling);
//	Node* get_right_sibling_();
//	void set_attribute_list_(vector<int> attribute_list);
//	vector<int> get_attribute_list_();
//	void set_training_set_index_(vector<int> training_set_index);
//	vector<int> get_training_set_index();

};

#endif
