/*
 * c4_5_attribute_selection_method.cpp
 *
 *  Created on: 10 Feb, 2012
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */
#include "c4_5_attribute_selection_method.h"

int C4_5AttributeSelectionMethod::current_attribute_ = -1;

C4_5AttributeSelectionMethod::C4_5AttributeSelectionMethod(
		TrainingSet* training_set, vector<int> training_set_index,
		vector<int> attribute_list) :
		AttributeSelectionMethod(training_set, training_set_index,
				attribute_list) {

}

int C4_5AttributeSelectionMethod::seed_num_ = 1000;

void C4_5AttributeSelectionMethod::set_status_(bool status) {
	this->status_ = status;
}

bool C4_5AttributeSelectionMethod::get_status_() {
	return this->status_;
}

void C4_5AttributeSelectionMethod::set_info_gain_(int attribute,
		double info_gain) {
	this->info_gain_.insert(map<int, double>::value_type(attribute, info_gain));
}

double C4_5AttributeSelectionMethod::get_info_gain_(int attribute) {
	return this->info_gain_[attribute];
}

void C4_5AttributeSelectionMethod::set_split_info_(int attribute,
		double split_info) {
	this->split_info_.insert(
			map<int, double>::value_type(attribute, split_info));
}

double C4_5AttributeSelectionMethod::get_split_info_(int attribute) {
	return this->split_info_[attribute];
}

void C4_5AttributeSelectionMethod::set_splited_training_set(int attribute,
		map<int, vector<int> > splited_training_set_index) {
	this->splited_training_set_.insert(
			map<int, map<int, vector<int> > >::value_type(attribute,
					splited_training_set_index));
}

map<int, vector<int> > C4_5AttributeSelectionMethod::get_splited_training_set_(
		int attribute) {
	return this->splited_training_set_[attribute];
}

void C4_5AttributeSelectionMethod::set_split_value_(int attribute,
		double split_value) {
	this->split_value_.insert(
			map<int, double>::value_type(attribute, split_value));
}

double C4_5AttributeSelectionMethod::get_split_value(int attribute) {
	return this->split_value_[attribute];
}

double C4_5AttributeSelectionMethod::CalculateInfo(
		vector<int> training_set_index) {
	double info = 0;
	double base = 2;
	vector<int> class_distribution = this->get_training_set_()->GetClassesNum(
			training_set_index);
	return this->CalculateInfoByClassNum(class_distribution,
			training_set_index.size());
}

double C4_5AttributeSelectionMethod::CalculateDiscreteAttributeInfoGain(
		int attribute) {
	double info1 = CalculateInfo(this->get_training_set_index_());
	double info2 = 0;
	int training_set_num = this->get_training_set_index_().size();
	map<int, vector<int> > mapper =
			this->get_training_set_()->SplitByDiscreteAttribute(
					this->get_training_set_index_(), attribute);
	this->splited_training_set_.insert(
			map<int, map<int, vector<int> > >::value_type(attribute, mapper));
	double temp_ratio;
	double split_info = 0;
	for (map<int, vector<int> >::iterator iter = mapper.begin();
			iter != mapper.end(); ++iter) {
		if (iter->second.size() != 0) {
			temp_ratio = (double) iter->second.size()
					/ (double) training_set_num;
			split_info += (-(temp_ratio) * (log(temp_ratio) / log(2.0)));
			info2 += (temp_ratio) * (this->CalculateInfo(iter->second));
		}
	}
	this->set_split_info_(attribute, split_info);
	return info1 - info2;
}

bool C4_5AttributeSelectionMethod::HandleDiscreteAttribute(int attribute) {
	map<int, vector<int> > mapper =
			this->get_training_set_()->SplitByDiscreteAttribute(
					this->get_training_set_index_(), attribute);

	int count = 0;
	map<int, vector<int> >::iterator iter;
	for (iter = mapper.begin(); iter != mapper.end(); ++iter) {
		if (iter->second.size()
				>= C4_5AttributeSelectionMethod::get_training_set_minimum_()) {
			count++;
		}
	}
	if (count < 2) {
		return false;
	}

	double info1 = CalculateInfo(this->get_training_set_index_());

	double info2 = 0;
	int training_set_num = this->get_training_set_index_().size();
	this->splited_training_set_.insert(
			map<int, map<int, vector<int> > >::value_type(attribute, mapper));
	double temp_ratio;
	double split_info = 0;
	for (map<int, vector<int> >::iterator iter = mapper.begin();
			iter != mapper.end(); ++iter) {
		int vector_size = iter->second.size();
		if (vector_size != 0) {
			temp_ratio = (double) vector_size / (double) training_set_num;
			split_info += (-(temp_ratio) * (log(temp_ratio) / log(2.0)));
			info2 += (temp_ratio) * (this->CalculateInfo(iter->second));
		}
	}
	double info_gain = info1 - info2;
	this->set_info_gain_(attribute, info_gain);
	this->set_split_info_(attribute, split_info);
	return true;
}

//bool C4_5AttributeSelectionMethod::HandleContinuousAttribute(int attribute) {
//
//	//vector<int> ordered_training_set_index = this->get_training_set_index_();
//	vector<int> ordered_training_set_index = this->training_set_index_;
//	int training_set_num = ordered_training_set_index.size();
//	TrainingSet* training_set = this->get_training_set_();
//	if (this->case_num_ < 4) {
//		return false;
//	}
//
//
//	vector<int>::iterator it;
//	IndexValue temp;
//	int class_attribute = training_set->GetClassifyAttribute();
//	AttributeValue** value_matrix = training_set->GetValueMatrixP();
//
//	vector<IndexValue> index_value_sort;
//	for (it = ordered_training_set_index.begin(); it != ordered_training_set_index.end(); ++it) {
//		temp.index_ = *it;
//		temp.value_ = value_matrix[attribute][*it].continuous_value_;
//		index_value_sort.push_back(temp);
//	}
//
//	this->SortTrainingSetByContinuousAttribute(index_value_sort);
//
//	ordered_training_set_index.clear();
//
//	vector<IndexValue>::iterator index_value_iter;
//	vector<IndexClass> index_class;
//	IndexClass temp_index_class;
//	for (index_value_iter = index_value_sort.begin(); index_value_iter != index_value_sort.end(); ++index_value_iter) {
//		ordered_training_set_index.push_back((*index_value_iter).index_);
//		temp_index_class.index_ = (*index_value_iter).index_;
//		temp_index_class.class_ = value_matrix[class_attribute][(*index_value_iter).index_].discrete_value_;
//		index_class.push_back(temp_index_class);
//	}
//
//	vector<int> mapper_class_num_right = training_set->GetClassesNum(ordered_training_set_index);
//	double info1 = this->CalculateInfo(ordered_training_set_index);
//	double info_test = this->CalculateInfoByClassNum(mapper_class_num_right,
//			this->case_num_);
//	vector<int> mapper_class_num_left;
//	int i;
//	for (i = 0; i < training_set->GetAttributeValueNum(class_attribute); ++i) {
//		mapper_class_num_left.push_back(0);
//	}
//	int training_set_minimum =
//			C4_5AttributeSelectionMethod::get_training_set_minimum_();
//	double current_value;
//	double next_value;
//	double info2;
//	bool info2_is_set = false;
//	double split_info;
//	int pos;
//	int class_num = training_set->GetAttributeValueNum(class_attribute);
//
//	int min_split = (training_set_num * 0.1) / class_num;
//	if (min_split > 25) {
//		min_split = 25;
//	} else if (min_split
//			< C4_5AttributeSelectionMethod::get_training_set_minimum_()) {
//		min_split = C4_5AttributeSelectionMethod::get_training_set_minimum_();
//
//	}
//	for (i = 0; i < training_set_num - min_split + 1; ++i) {
//		int class_index = index_class.at(i).class_;
//		if (i < min_split) {
//			;
//		} else {
//			next_value = index_value_sort.at(i).value_;
//			if (current_value != next_value) {
//				double new_info2 = ((double) i / (double) training_set_num) * this->CalculateInfoByClassNum(mapper_class_num_left,i) + ((double) (training_set_num - i) / (double) training_set_num) * this->CalculateInfoByClassNum(mapper_class_num_right,training_set_num - i);
//				if (info2_is_set) {
//					if (new_info2 < info2) {
//						info2 = new_info2;
//						pos = i - 1;
//					}
//				} else {
//					info2 = new_info2;
//					info2_is_set = true;
//					pos = i - 1;
//				}
//
//			}
//		}
//		current_value = index_value_sort.at(i).value_;
//		mapper_class_num_left.at(class_index) ++;
//		mapper_class_num_right.at(class_index) --;
//
//		}
//
//if(	info2_is_set) {
//		double info_gain = info1 - info2;
//		this->set_info_gain_(attribute, info_gain);
//		double split_value = (index_value_sort.at(pos).value_
//		+ index_value_sort.at(pos + 1).value_) / 2;
//		this->set_split_value_(attribute, split_value);
//
//		double ratio1 = (double) (pos + 1) / (double) (training_set_num);
//		double ratio2 = (double) (training_set_num - pos - 1)
//		/ (double) (training_set_num);
//		double split_info = (-ratio1) * (log(ratio1) / log(2.0))
//		+ (-ratio2) * (log(ratio2) / log(2.0));
//
//		this->set_split_info_(attribute, split_info);
//		map<int, vector<int> > mapper = training_set->SplitByPositon(
//		ordered_training_set_index, pos);
//		this->set_splited_training_set(attribute, mapper);
//
//		return true;
//
//	} else {
//		return false;
//	}
//}

bool C4_5AttributeSelectionMethod::HandleContinuousAttribute(int attribute) {

	//vector<int> ordered_training_set_index = this->get_training_set_index_();
	C4_5AttributeSelectionMethod::current_attribute_ = attribute;
	vector<int> ordered_training_set_index = this->training_set_index_;
	int training_set_num = ordered_training_set_index.size();
	TrainingSet* training_set = this->get_training_set_();
	if (this->case_num_ < 4) {
		return false;
	}

	int class_attribute = training_set->GetClassifyAttribute();
	AttributeValue** value_matrix = training_set->GetValueMatrixP();

	this->SortTrainingSet(ordered_training_set_index);

	vector<int> mapper_class_num_right = training_set->GetClassesNum(ordered_training_set_index);
	double info1 = this->CalculateInfo(ordered_training_set_index);
	vector<int> mapper_class_num_left;
	int i;
	for (i = 0; i < training_set->GetAttributeValueNum(class_attribute); ++i) {
		mapper_class_num_left.push_back(0);
	}
	double current_value;
	double next_value;
	double info2;
	bool info2_is_set = false;
	int pos;
	int class_num = training_set->GetAttributeValueNum(class_attribute);

	int min_split = (training_set_num * 0.1) / class_num;
	if (min_split > 25) {
		min_split = 25;
	} else if (min_split < C4_5AttributeSelectionMethod::get_training_set_minimum_()) {
		min_split = C4_5AttributeSelectionMethod::get_training_set_minimum_();

	}
	for (i = 0; i < training_set_num - min_split + 1; ++i) {
		int class_index = value_matrix[class_attribute][ordered_training_set_index.at(i)].discrete_value_;
		if (i < min_split) {
			;
		} else {
//			next_value = index_value_sort.at(i).value_;
			next_value = value_matrix[attribute][ordered_training_set_index.at(i)].continuous_value_;
			if (current_value != next_value) {
				double new_info2 = ((double) i / (double) training_set_num) * this->CalculateInfoByClassNum(mapper_class_num_left,i) + ((double) (training_set_num - i) / (double) training_set_num) * this->CalculateInfoByClassNum(mapper_class_num_right,training_set_num - i);
				if (info2_is_set) {
					if (new_info2 < info2) {
						info2 = new_info2;
						pos = i - 1;
					}
				} else {
					info2 = new_info2;
					info2_is_set = true;
					pos = i - 1;
				}

			}
		}
		current_value = value_matrix[attribute][ordered_training_set_index.at(i)].continuous_value_;
		mapper_class_num_left.at(class_index) ++;
		mapper_class_num_right.at(class_index) --;

		}

if(	info2_is_set) {
		double info_gain = info1 - info2;
		this->set_info_gain_(attribute, info_gain);
		double split_value = (value_matrix[attribute][ordered_training_set_index.at(pos)].continuous_value_ + value_matrix[attribute][ordered_training_set_index.at(pos + 1)].continuous_value_) / 2;

		this->set_split_value_(attribute, split_value);

		double ratio1 = (double) (pos + 1) / (double) (training_set_num);
		double ratio2 = (double) (training_set_num - pos - 1) / (double) (training_set_num);
		double split_info = (-ratio1) * (log(ratio1) / log(2.0))
		+ (-ratio2) * (log(ratio2) / log(2.0));

		this->set_split_info_(attribute, split_info);
		map<int, vector<int> > mapper = training_set->SplitByPositon(
		ordered_training_set_index, pos);
		this->set_splited_training_set(attribute, mapper);

		return true;

	} else {
		return false;
	}
}

bool C4_5AttributeSelectionMethod::CompStatic(int a, int b){
	AttributeValue** value_matrix = AttributeSelectionMethod::training_set_->GetValueMatrixP();
	if(value_matrix[C4_5AttributeSelectionMethod::current_attribute_][a].continuous_value_ < value_matrix[C4_5AttributeSelectionMethod::current_attribute_][b].continuous_value_ ){
		return true;
	}else{
		return false;
	}
}

void C4_5AttributeSelectionMethod::SortTrainingSet(vector<int>& training_set_index){
	sort(training_set_index.begin(), training_set_index.end(), C4_5AttributeSelectionMethod::CompStatic);
}

bool comp(IndexValue a, IndexValue b) {
	if (a.value_ < b.value_) {
		return true;
	} else {
		return false;
	}
}

void C4_5AttributeSelectionMethod::SortTrainingSetByContinuousAttribute(
		vector<IndexValue> &index_value_sort) {
	sort(index_value_sort.begin(), index_value_sort.end(), comp);

}

double C4_5AttributeSelectionMethod::CalculateInfoByClassNum(
		vector<int>& class_num, int all_num) {
	double info = 0;
	vector<int>::iterator iter;
	for (iter = class_num.begin(); iter != class_num.end(); ++iter) {
		if ((*iter) != 0) {
			double ratio = (double) (*iter) / (double) all_num;
			info += (-ratio) * (log(ratio) / log(2.0));
		}
	}
	return info;
}



vector<int> C4_5AttributeSelectionMethod::GetRandomSubSpace(
		vector<int> attribute_list) {
	// the result attribute list can't be repeatable
	int seed_num = C4_5AttributeSelectionMethod::get_seed_();
	vector<int> result;
	int attribute_num = attribute_list.size();
	int result_size = log((double) attribute_num) / log(2.0) + 1;
	int j = 0;
	int random_num;
	vector<int>::iterator it = attribute_list.begin();

	for (j = 0; j < result_size; ++j) {
		srand(unsigned(time(NULL)) + seed_num);
		random_num = rand() % attribute_num;
		result.push_back(attribute_list.at(random_num));
		attribute_list.erase(it + random_num);
		it = attribute_list.begin();
		attribute_num = attribute_list.size();
	}
	C4_5AttributeSelectionMethod::inc_seed_();
	return result;

}

void C4_5AttributeSelectionMethod::TestGetRandomSubSpace() {
	vector<int> attribute_list;

	vector<int> result = this->GetRandomSubSpace(attribute_list);
	vector<int>::iterator it;
	for (it = result.begin(); it != result.end(); ++it) {
		cout << *it << endl;
	}
}

void C4_5AttributeSelectionMethod::ExecuteSelection() {
	/*
	 * use the random subspace of all features
	 */
	//vector<int> attribute_list = this->GetRandomSubSpace(this->get_attribute_list_());

	/*
	 * use the all features
	 */
	vector<int> attribute_list = this->get_attribute_list_();

	TrainingSet* training_set = this->get_training_set_();
	vector<int>::iterator iter;
	this->set_status_(false);
	for (iter = attribute_list.begin(); iter != attribute_list.end(); ++iter) {
		if (training_set->GetAttributeType(*iter) == DISCRETE) {
			if (this->HandleDiscreteAttribute(*iter)) {
				this->set_status_(true);
			}
		} else {
			if (this->HandleContinuousAttribute(*iter)) {
				this->set_status_(true);
			}
		}
	}
	if (!this->get_status_()) {
		return;
	}
	int mapper_info_gain_size = this->info_gain_.size();
	map<int, double>::iterator iter_info_gain;
	iter_info_gain = this->info_gain_.begin();
	double total_info_gain = 0;
	for (iter_info_gain = this->info_gain_.begin();iter_info_gain != this->info_gain_.end(); ++iter_info_gain) {
		total_info_gain += iter_info_gain->second;
	}
	double average_info_gain = total_info_gain / (double) (mapper_info_gain_size);
	double gain_ratio;
	double temp_gain_ratio;
	bool is_set_gain_ratio = false;
	int attribute;
	bool is_set_attribute = false;
	double split_info; //just for test
	for (iter_info_gain = this->info_gain_.begin(); iter_info_gain != this->info_gain_.end(); ++iter_info_gain) {
		/*the average_info_gain minus 0.001 to avoid the situation 
		 where all the info gain is the same */
		if (iter_info_gain->second >= average_info_gain - 0.001) {
			if (this->get_split_info_(iter_info_gain->first) > 0) {
				split_info = get_split_info_(iter_info_gain->first); //just for test
				temp_gain_ratio = this->get_info_gain_(iter_info_gain->first) / this->get_split_info_(iter_info_gain->first);
				if (is_set_gain_ratio) {
					if (temp_gain_ratio > gain_ratio) {
						gain_ratio = temp_gain_ratio;
						attribute = iter_info_gain->first;
						is_set_attribute = true;
					}
				} else {
					gain_ratio = temp_gain_ratio;
					attribute = iter_info_gain->first;
					is_set_gain_ratio = true;
					is_set_attribute = true;
				}
			}
		}
	}
	if (!is_set_attribute) {
		attribute = this->info_gain_.begin()->first;
	}
	this->set_attribute_(attribute);
	if (this->get_training_set_()->GetAttributeType(attribute) == CONTINUOUS) {
		this->set_split_value_result_(this->get_split_value(attribute));
	}
	this->set_attribute_info_gain_(this->info_gain_[attribute]);
	this->set_splited_training_set_result_(this->get_splited_training_set_(attribute));
}



/*
 * Problem:when we use c4.5 to construct decision tree, the selected attribute information gain
 * is always greater than the average information gain over all attributes
 * How IGR deal with it ?
 */


void C4_5AttributeSelectionMethod::ExecuteSelectionByIGR() {
	/*
	 * use the random subspace of all features
	 */

	//vector<int> attribute_list = this->GetRandomSubSpace(this->get_attribute_list_());

	/*
	 * use the all features
	 */
	vector<int> attribute_list = this->get_attribute_list_();

	TrainingSet* training_set = this->get_training_set_();
	vector<int>::iterator iter;
	this->set_status_(false);
	for (iter = attribute_list.begin(); iter != attribute_list.end(); ++iter) {
		if (training_set->GetAttributeType(*iter) == DISCRETE) {
			if (this->HandleDiscreteAttribute(*iter)) {
				this->set_status_(true);
			}
		} else {
			if (this->HandleContinuousAttribute(*iter)) {
				this->set_status_(true);
			}
		}
	}

	if (!this->get_status_()) {
		return;
	}

	int mapper_info_gain_size = this->info_gain_.size();
	map<int, double>::iterator iter_info_gain;
	iter_info_gain = this->info_gain_.begin();
	double total_info_gain = 0;
	for (iter_info_gain = this->info_gain_.begin();iter_info_gain != this->info_gain_.end(); ++iter_info_gain) {
		total_info_gain += iter_info_gain->second;
	}
	double average_info_gain = total_info_gain / (double) (mapper_info_gain_size);
	double gain_ratio;
	double temp_gain_ratio;
	bool is_set_gain_ratio = false;
	int attribute;
	bool is_set_attribute = false;
	double split_info; //just for test
	for (iter_info_gain = this->info_gain_.begin(); iter_info_gain != this->info_gain_.end(); ++iter_info_gain) {
		/*the average_info_gain minus 0.001 to avoid the situation
		 where all the info gain is the same */
		if (iter_info_gain->second >= average_info_gain - 0.001) {
			if (this->get_split_info_(iter_info_gain->first) > 0) {
				split_info = get_split_info_(iter_info_gain->first); //just for test
				temp_gain_ratio = this->get_info_gain_(iter_info_gain->first)
						/ this->get_split_info_(iter_info_gain->first);
				if (is_set_gain_ratio) {
					if (temp_gain_ratio > gain_ratio) {
						gain_ratio = temp_gain_ratio;
						attribute = iter_info_gain->first;
						is_set_attribute = true;
					}
				} else {
					gain_ratio = temp_gain_ratio;
					attribute = iter_info_gain->first;
					is_set_gain_ratio = true;
					is_set_attribute = true;
				}
			}
		}
	}
	if (!is_set_attribute) {
		attribute = this->info_gain_.begin()->first;
	}
	this->set_attribute_(attribute);
	if (this->get_training_set_()->GetAttributeType(attribute) == CONTINUOUS) {
		this->set_split_value_result_(this->get_split_value(attribute));
	}
	this->set_attribute_info_gain_(this->info_gain_[attribute]);
	this->set_splited_training_set_result_(this->get_splited_training_set_(attribute));
}

