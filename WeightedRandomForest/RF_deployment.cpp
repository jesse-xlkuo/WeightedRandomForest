/*
 * RFDeployment.cpp
 *
 *  Created on: 28 Mar, 2012
 *      Author: meng email:qinghan.meng@gmail.com
 *  problem:when a training example participated any tree's construction
 */

#include "RF_deployment.h"
/*
 * for VisForsts  #include"MDScaling.h"
 */
#include"MDScaling.h"

RFDeployment::RFDeployment() {

}

RFDeployment::RFDeployment(RandomForests* RF) {
	this->RF_ = RF;
	this->random_forests_ = RF->get_trees();
}

int RFDeployment::PredictClass(TrainingSet* training_set, int tuple) {
	map<int, int> mapper;
	vector<DecisionTree*>::iterator iter;
	for (iter = this->random_forests_.begin();
			iter != this->random_forests_.end(); ++iter) {
		int target_class = ((LeafNode*) ((*iter)->PredictClass(training_set,
				tuple, (*iter)->get_root_())))->get_class_();
		if (mapper.find(target_class) == mapper.end()) {
			mapper.insert(map<int, int>::value_type(target_class, 1));
		} else {
			mapper[target_class]++;
		}

	}
	int max = 0;
	int result;
	map<int, int>::iterator map_iter;
	for (map_iter = mapper.begin(); map_iter != mapper.end(); ++map_iter) {
		if (map_iter->second >= max) {
			result = map_iter->first;
			max = map_iter->second;
		}
	}
	return result;
}

double RFDeployment::GetErrorRate(TrainingSet* training_set, vector<int> training_set_index) {
	int error = 0;
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int target_attribute = training_set->GetClassifyAttribute();
	vector<int>::iterator iter;
	for (iter = training_set_index.begin(); iter != training_set_index.end();
			++iter) {
		if (this->PredictClass(training_set, *iter)
				!= value_matrix[target_attribute][*iter].discrete_value_) {
			error++;
		}
	}
	return ((double) error) / ((double) training_set_index.size());
}

void RFDeployment::GetEachTreeErrorRate(TrainingSet* training_set, vector<int> training_set_index) {
	double rate = 0;
	int tree_num = 0;
	for (vector<DecisionTree*>::iterator it = this->random_forests_.begin();
			it != this->random_forests_.end(); ++it) {
		double error_rate = (*it)->GetErrorRate(training_set,
				training_set_index);
		rate += error_rate;
		++tree_num;
		cout << "the tree error rate:" << error_rate << endl;
	}
	cout << "the average error rate:" << rate / tree_num << endl;

}

void RFDeployment::GenerateConfusionMatrix(TrainingSet* training_set, vector<int> training_set_index) {

	int class_num = training_set->GetAttributeValueNum(training_set->GetClassifyAttribute());
	int training_set_num = training_set_index.size();
	cout << "the test num = " << training_set_num << endl;
	int i;
	for (i = 0; i < class_num; ++i) {
		confusion_matrix c_matrix;
		c_matrix.false_n_ = 0;
		c_matrix.false_p_ = 0;
		c_matrix.true_n_ = training_set_num;
		c_matrix.true_p_ = 0;
		this->c_matrix_.push_back(c_matrix);
	}
	int error = 0;
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int target_attribute = training_set->GetClassifyAttribute();
	vector<int>::iterator iter;
	for (iter = training_set_index.begin(); iter != training_set_index.end(); ++iter) {
		int predict_class = this->PredictClass(training_set, *iter);
		int actual_class = value_matrix[target_attribute][*iter].discrete_value_;
		if (predict_class != actual_class) {
			error++;
			this->c_matrix_.at(actual_class).false_n_++;
			this->c_matrix_.at(predict_class).false_p_ ++;
			this->c_matrix_.at(predict_class).true_n_--;
		}
		else{
			this->c_matrix_.at(predict_class).true_p_++;
		}
		this->c_matrix_.at(actual_class).true_n_--;
	}
	vector<confusion_matrix>::iterator it;
	int correct = 0;
	for(it = this->c_matrix_.begin(); it != this->c_matrix_.end(); ++ it){
		correct += it->true_p_;
		cout << it->true_p_ << " " << it->false_n_ << " " << it->false_p_ << " " << it->true_n_ << endl;
	}
	cout << "error num = " << error << endl;
	cout << "correct num = " << correct << endl;
	cout << "the Test Accuracy is = "<< 1 - (double)error / (double)(error + correct) << endl;
}

bool IsInBaggingSet(vector<int> bagging_set,int tuple){
	vector<int>::iterator it = bagging_set.begin();
	vector<int>::iterator end = bagging_set.end();
	while(it != end ){
		if(*it == tuple){
			return true;
		}
		++ it;
	}
	return false;
}

void RFDeployment::CalculateOOBRFCS2(TrainingSet* training_set){
	int training_set_num = training_set->get_training_set_num_();
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int trees_num = this->RF_->get_trees_num();

	vector<double> p_vec;				   //Q(x,y) - Q(x,j) for each training set example
	/*
	 * OOB_num and correct_num is to calcualte individual tree strength
	 */
	vector<int> OOB_num(trees_num,0);      //the number of out of bagging tuples of each tree
	vector<int> correct_num(trees_num,0);  // the number of correct prediction of each tree

	vector<int> j_vec(training_set_num,-1);// each training example max j

	vector<vector<int> > distribution(trees_num,vector<int>(training_set_num,-1));
	vector<vector<bool> > selected_status = this->RF_->get_selected_status_();
	int i, j;
	for(i = 0; i < training_set_num; ++ i){
		int actual_class = value_matrix[target_attribute][i].discrete_value_;
		int OOB_classifier = 0;
		vector<int> vec(class_num,0);  //store the predicted result classes num
		for(j = 0; j < trees_num; j ++){
			if(!selected_status.at(j).at(i)){
				OOB_num.at(j) ++;
				int predict_class = ((LeafNode*)this->random_forests_.at(j)->PredictClass(training_set,i,
									this->random_forests_.at(j)->get_root_()))->get_class_();
				distribution.at(j).at(i) = predict_class;
				if(actual_class == predict_class){
					correct_num.at(j) ++;
				}
				OOB_classifier ++;
				vec.at(predict_class) ++;

			}
		}
		//cout << "old:" << OOB_classifier << " ";
		if(OOB_classifier != 0){
			int max_j;
			int max_error = 0;
			int k = 0;
			while (k < class_num) {
				if (k != actual_class) {
					if (vec.at(k) >= max_error) {
						max_error = vec.at(k);
						max_j = k;
					}
				}
				k++;
			}
			/*
			 * debug:j_vec.at(i) = max_j; 7/17 2012
			 */
			//j_vec.at(i) = k;
			//cout << "old max_j " << max_j << " ";
			j_vec.at(i) = max_j;
			//cout << "old max_j " << max_j << " ";
			p_vec.push_back((double)(vec.at(actual_class) - max_error) / (double) (OOB_classifier));
		}
	}
	this->OOB_distribution_ = distribution;
	vector<double>::iterator it;
	double sum = 0.0;
	double sum_expection = 0.0;
	//cout << "debug old:";
	for(it = p_vec.begin(); it != p_vec.end(); ++ it){
		//cout << *it << " ";
		sum_expection += (*it)*(*it);
		sum += *it;
	}
	//cout << "debug end" << endl;
	double RF_strength_ = sum / (double)(p_vec.size());
	//this->RF_strength_ = sum / (double)(training_set_num);
	cout << " the RF strength is " << RF_strength_ << endl;
	/*
	 *
	 * the var is parepare to calcualte the correlation
	 */
	//double var = ((sum_expection) / (double)p_vec.size()) - this->RF_strength_ * this->RF_strength_;
	double var = ((sum_expection) / (double)training_set_num) - RF_strength_ * RF_strength_;

	//calculate the individual tree predicted class distribution by OOB estimate
	vector<vector<int> > tree_classes_dis(trees_num,vector<int>(class_num,0));
	int a;
	for(a = 0; a < trees_num; a ++){
		int b;
		for(b = 0; b < training_set_num; b ++){
			int class_no = distribution.at(a).at(b);
			if(class_no != -1){
				tree_classes_dis.at(a).at(distribution.at(a).at(b)) ++;
			}
		}
	}

	int index;
	vector<double> sd_vec(trees_num,0.0);
	for(index = 0; index < trees_num; ++ index){
		double p1 = (double)correct_num.at(index) / (double)OOB_num.at(index);
		int count = 0;
		int k;
		for(k = 0; k < training_set_num; k ++){
			if(distribution.at(index).at(k) != -1){
				if(distribution.at(index).at(k) == j_vec.at(k)){
					count ++;
				}

			}

		}
		double p2 = (double)count / (double)OOB_num.at(index);
		double base = (p1 + p2 + (p1 - p2)*(p1 - p2));
		double sd = pow(base,0.5);
		sd_vec.at(index) = sd;
	}
	/*
	 * calculate the expectation of standard deviation
	 */
	vector<double>::iterator sd_iter;
	double sum_sd = 0.0;
	for(sd_iter = sd_vec.begin(); sd_iter != sd_vec.end(); ++ sd_iter){
		sum_sd += *sd_iter;
	}
	double e_sd = sum_sd / (double)trees_num;

	/*
	 * calculate the c/s2
	 */
	double correlation = var / (e_sd*e_sd);
	cout << "the correlation is " << correlation << endl;
	double c_s2 = correlation / (RF_strength_ * RF_strength_);
	cout << "c/s2 = " << c_s2 << endl;

	/*
	 * calculate individual tree strength
	 *
	 */
    vector<double> old_trees_strength;
	int k;
	for(k = 0; k < OOB_num.size(); ++ k){
		if(OOB_num.at(k) != 0){
			//this->trees_strength_.push_back((double)correct_num.at(k)/(double)OOB_num.at(k));
			//cout << (double)(2*correct_num.at(k)-OOB_num.at(k)) / (double)training_set_num << " ";
			//old_trees_strength.push_back((double)(2*correct_num.at(k)-OOB_num.at(k)) / (double)training_set_num);
			old_trees_strength.push_back((double)(2*correct_num.at(k)-OOB_num.at(k)) / (double)OOB_num.at(k));
		}else{
			old_trees_strength.push_back(0.0);
		}
	}
}

/*double RFDeployment::CalculateCorBetweenTwoTrees(int a, int b, TrainingSet* training_set){
	int training_set_num = training_set->get_training_set_num_();
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int i;
	vector<vector<int> > tree_class(2,vector<int>(class_num,0));
	vector<int> max_j(training_set_num,-1);
	vector<double> p_vec;
	vector<int> OOB_num(2,0);
	vector<int> OOB_correct(2,0);
	for(i = 0; i < training_set_num; i ++){
		int OOB_classifier = 0;
		int actual_class = value_matrix[target_attribute][i].discrete_value_;
		vector<int> vec(class_num,0);
		if(this->OOB_distribution_.at(a).at(i) != -1){
			tree_class.at(0).at(this->OOB_distribution_.at(a).at(i)) ++;
			vec.at(this->OOB_distribution_.at(a).at(i)) ++;
			OOB_num.at(0) ++;
			if(actual_class == this->OOB_distribution_.at(a).at(i)){
				OOB_correct.at(0) ++;
			}
			OOB_classifier ++;
		}
		if(this->OOB_distribution_.at(b).at(i) != -1){
			tree_class.at(1).at(this->OOB_distribution_.at(b).at(i)) ++;
			vec.at(this->OOB_distribution_.at(b).at(i)) ++;
			OOB_num.at(1) ++;
			if(actual_class == this->OOB_distribution_.at(b).at(i)){
				OOB_correct.at(1) ++;
			}
			OOB_classifier ++;
		}
		if(OOB_classifier != 0){
			int max_error;
			bool is_j_set = false;
			int j;
			int k = 0;
			while(k < class_num){
				if(k != actual_class){
					if (!is_j_set) {
						j = k;
						max_error = vec.at(k);
						is_j_set = true;
					} else {
						if (max_error < vec.at(k)) {
							max_error = vec.at(k);
							j = k;
						}
					}
				}
				k ++;
			}
			max_j.at(i) = j;
			p_vec.push_back((double)(vec.at(actual_class) - max_error) / (double)OOB_classifier);
			if(i < 10){
				int x;
				for(x = 0; x < class_num; ++ x){
					cout << x << ":" << vec.at(x) << " ";
				}
				cout << "\n";
				cout << actual_class << " " << vec.at(actual_class) << " " << max_error << " " << j << endl;
			}
		}
	}


	 * calculate the strength

	double p_sum = 0.0;
	double e_sum = 0.0;
	vector<double>::iterator it;
	for(it = p_vec.begin(); it != p_vec.end(); ++ it){
		p_sum += *it;
		e_sum += (*it)*(*it);
	}
	double strength = p_sum / (double)(p_vec.size());
	double var = e_sum / (double)(p_vec.size()) - strength * strength;
	int tree_index;
	vector<double> sd(2,0.0);
	for(tree_index = 0; tree_index < 2; tree_index ++){
		double p1 = (double)OOB_correct.at(tree_index) / (double)OOB_num.at(tree_index);
		int count = 0;
		int t_index;
		if(tree_index == 0){
			for(t_index = 0; t_index < training_set_num; t_index ++){
				if (max_j.at(t_index) != -1) {
					if (this->OOB_distribution_.at(a).at(t_index) == max_j.at(t_index)) {
						count++;
					}
				}

			}
		}else{
			for (t_index = 0; t_index < training_set_num; t_index++) {
				if(max_j.at(t_index) != -1){
					if (this->OOB_distribution_.at(b).at(t_index) == max_j.at(t_index)) {
						count++;
					}
				}
			}
		}
	    double p2 = (double)count / (double)OOB_num.at(tree_index);
	    double base = p1 + p2 + (p1 - p2)*(p1 - p2);
	    double sd_temp = pow(base,0.5);
	    sd.at(tree_index) = sd_temp;
	}
	double e_sd = (sd.at(0) + sd.at(1)) / 2;
	double correlation = var /(e_sd * e_sd);
	return correlation;



}*/



void RFDeployment::CalculateCorBetweenEachTwoTree(TrainingSet* training_set){
	int trees_num = this->RF_->get_trees_num();
	vector<vector<double> > cor_vec(trees_num,vector<double>(trees_num,0.0));
	int a, b;
	for(a = 0; a < trees_num; a ++){
		for(b = 0; b < trees_num; b ++){
			//cor_vec.at(a).at(b) = this->CalculateCorBetweenTwoTrees(a,b,training_set);
			cor_vec.at(a).at(b) = this->CalculateTwoTreesCorrelation(a,b,training_set);
		}
		cout << endl;
	}
	this->cor_vec_ = cor_vec;
}

vector<vector<double> > RFDeployment::get_cor_vec_(){
	return this->cor_vec_;
}

vector<point> RFDeployment::CalculateRelativePos(){
	int trees_num = this->RF_->get_trees_num();
	double* cor = (double*)malloc( trees_num * trees_num * sizeof(double));
	vector<point> p_vec;
	int i = 0;
	if(this->cor_vec_.size() == 0){
		//cout << "error:you may not calculate the correlation between each two trees" << endl;
		return p_vec;
	}
	vector<vector<double> >::iterator iter1;
	for(iter1 = this->cor_vec_.begin(); iter1 != this->cor_vec_.end(); ++ iter1){
		vector<double>::iterator iter2;
		for(iter2 = iter1->begin(); iter2 != iter1->end(); ++ iter2){
			*(cor+i) = *(iter2);
			i ++;
		}
	}
	double* pos;
	pos = getOutMatrix(cor, trees_num, 0.00000001, 100);
	int x;
	for(x = 0; x < trees_num; x++){
		point p;
		p.x_ = pos[x * 2];
		p.y_ = pos[x * 2 + 1];
		//cout << p.x_ << " " << p.y_ << endl;
		p_vec.push_back(p);
	}

	free(pos);
	return p_vec;
}

vector<double> RFDeployment::GetEachTreeStrength(){
	return this->trees_strength_;
}

/*
 * following code CalculateOOBPredictorResultis added from 7/20/2012
 */

void RFDeployment::CalculateOOBPredictor(TrainingSet* training_set){
	int training_set_num = training_set->get_training_set_num_();
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int trees_num = this->RF_->get_trees_num();
	vector<vector<bool> > selected_status = this->RF_->get_selected_status_();

	int i;
	for (i = 0; i < training_set_num; ++i) {
		vector<int> temp_vec(trees_num,-1);
		int j;
		for (j = 0; j < trees_num; ++j) {
			if (selected_status.at(j).at(i) == false) {
			    int predicted_class =((LeafNode*) this->random_forests_.at(j)->PredictClass(
								    training_set, i, this->random_forests_.at(j)->get_root_()))->get_class_();
				temp_vec.at(j) = predicted_class;
			}

		}
		this->OOB_predictor_.push_back(temp_vec);
	}
	int n;
	cout << "\n";
	for(n = 0; n < 10; ++ n){
		int m;
		for(m = 0; m < trees_num; m ++ ){
			cout << this->OOB_predictor_.at(n).at(m) << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

/*
 * you should run the CalculateOOBPredictor() function first
 */

void RFDeployment::CalculateOOBPredictorResult(TrainingSet* training_set){
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	vector<vector<int> >::iterator it1;
	for(it1  = this->OOB_predictor_.begin(); it1 != this->OOB_predictor_.end(); ++ it1){
		vector<int> temp_vec(class_num,0);
		vector<int>::iterator it2;
		for(it2 = it1->begin(); it2 != it1->end(); ++ it2){
			if(*it2 != -1){
				temp_vec.at(*it2) ++;
			}
		}
		int max = 0;
		int result = -1;
		int i;
		for(i = 0; i < class_num; ++ i){
			if(temp_vec.at(i) > max){
				max = temp_vec.at(i);
				result = i;
			}
		}
		this->OOB_predictor_result_.push_back(result);
	}
}

/*
 * the denominator is the oob training set, not all the training set
 * before you use this function result,you should run CalculateOOBPredictor() first;
 */

void RFDeployment::CalculateEachTreeOOBStrength(TrainingSet* training_set){
	int trees_num = this->RF_->get_trees_num();
	int training_set_num = training_set->get_training_set_num_();
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	//vector<int> classes_count(class_num,0);

	int i;
	for(i = 0; i < trees_num; i ++){
		int numerator = 0;
	    int denominator = 0;
		int j;
		for(j = 0;j < training_set_num; ++ j){
			int predicted_result = this->OOB_predictor_.at(j).at(i);
			if(predicted_result != -1){
				denominator ++;
				//classes_count.at(predicted_result) ++;
				if(predicted_result != value_matrix[target_attribute][j].discrete_value_){
					numerator --;
				}else{
					numerator ++;
				}
			}
		}
		//cout << "<" << numerator << "," << denominator << ">" << " ";
		this->trees_strength_.push_back((double)numerator / (double)denominator);
	}
	/*
	 * print the strengh of each tree
	 */
	/*vector<double>::iterator it;
	for(it = this->trees_strength_.begin(); it != this->trees_strength_.end(); ++ it){
		cout << *it << " ";
	}*/
}

/* 在运行此函数之前，必须先运行CalculateOOBPredictor()函数获得this->OOB_predictor
 * 首先得算出两棵树组成的强度
 */
double RFDeployment::CalculateTwoTreesCorrelation(int a, int b, TrainingSet* training_set){
	int training_set_num = training_set->get_training_set_num_();
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int i;
	vector<vector<int> > tree_class(2, vector<int>(class_num, 0));
	vector<int> max_j(training_set_num, -1);
	vector<double> p_vec;
	vector<int> OOB_num(2, 0);
	vector<int> OOB_correct(2, 0);
	for (i = 0; i < training_set_num; i++) {
		int OOB_classifier = 0;
		int actual_class = value_matrix[target_attribute][i].discrete_value_;
		vector<int> vec(class_num, 0);
		if (this->OOB_predictor_.at(i).at(a) != -1) { // this->OOB_predictor_.at(i).at(a)
			tree_class.at(0).at(this->OOB_predictor_.at(i).at(a)) ++;vec
			.at(this->OOB_predictor_.at(i).at(a)) ++;OOB_num
			.at(0) ++;if
(			actual_class == this->OOB_predictor_.at(i).at(a)) {
				OOB_correct.at(0) ++;
			}
			OOB_classifier++;
		}
		if (this->OOB_predictor_.at(i).at(b) != -1) {
			tree_class.at(1).at(this->OOB_predictor_.at(i).at(b)) ++;vec
			.at(this->OOB_predictor_.at(i).at(b)) ++;OOB_num
			.at(1) ++;if
(			actual_class == this->OOB_predictor_.at(i).at(b)) {
				OOB_correct.at(1) ++;
			}
			OOB_classifier++;
		}
		if (OOB_classifier != 0) {
			int max_error;
			bool is_j_set = false;
			int j;
			int k = 0;
			while (k < class_num) {
				if (k != actual_class) {
					if (!is_j_set) {
						j = k;
						max_error = vec.at(k);
						is_j_set = true;
					} else {
						if (max_error < vec.at(k)) {
							max_error = vec.at(k);
							j = k;
						}
					}
				}
				k++;
			}
			max_j.at(i) = j;
			p_vec.push_back(
					(double) (vec.at(actual_class) - max_error)
							/ (double) OOB_classifier);
			/*if(i < 10){
			 int x;
			 for(x = 0; x < class_num; ++ x){
			 cout << x << ":" << vec.at(x) << " ";
			 }
			 cout << "\n";
			 cout << actual_class << " " << vec.at(actual_class) << " " << max_error << " " << j << endl;
			 }*/
		}
	}

	/*
	 * calculate the strength
	 */
	double p_sum = 0.0;
	double e_sum = 0.0;
	vector<double>::iterator it;
	for (it = p_vec.begin(); it != p_vec.end(); ++it) {
		p_sum += *it;
		e_sum += (*it) * (*it);
	}
	double strength = p_sum / (double) (p_vec.size());

	double var = e_sum / (double) (p_vec.size()) - strength * strength;

	int tree_index;
	vector<double> sd(2, 0.0);
	for (tree_index = 0; tree_index < 2; tree_index++) {
		double p1 = (double) OOB_correct.at(tree_index)
				/ (double) OOB_num.at(tree_index);
		int count = 0;
		int t_index;
		if (tree_index == 0) {
			for (t_index = 0; t_index < training_set_num; t_index++) {
				if (max_j.at(t_index) != -1) {
					if (this->OOB_predictor_.at(t_index).at(a)
							== max_j.at(t_index)) { //this->OOB_predictor_.at(t_index).at(a)
						count++;
					}
				}

			}
		} else {
			for (t_index = 0; t_index < training_set_num; t_index++) {
				if (max_j.at(t_index) != -1) {
					if (this->OOB_predictor_.at(t_index).at(b)
							== max_j.at(t_index)) {
						count++;
					}
				}
			}
		}
		double p2 = (double) count / (double) OOB_num.at(tree_index);
		double base = p1 + p2 + (p1 - p2) * (p1 - p2);
		double sd_temp = pow(base, 0.5);
		sd.at(tree_index) = sd_temp;
	}
	double e_sd = (sd.at(0) + sd.at(1)) / 2;
	double correlation = var / (e_sd * e_sd);
	return correlation;
}

/*
 * a two dimension array store the oob confusion matrix,using vector<vector<int> > vec
 * vec.at(i).at(j),i,j represents the class,i is the actual class,j is the predcited class.
 */

void RFDeployment::CalculateOOBConfusionMatrix(TrainingSet* training_set){
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	int training_set_num = training_set->get_training_set_num_();
	this->OOB_confusion_matrix_ = vector<vector<int> >(class_num, vector<int>(class_num,0));
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int i;
	for(i = 0; i < training_set_num; ++ i){
		int actual_class = value_matrix[target_attribute][i].discrete_value_;
		int predicted_class = this->OOB_predictor_result_.at(i);
		/*
		 * maybe an instance does not have a OOB classifier
		 */
		if(predicted_class != -1){
			if (predicted_class == actual_class) {
				this->OOB_confusion_matrix_.at(actual_class).at(
						actual_class) ++;}
            else{
				this->OOB_confusion_matrix_.at(actual_class).at(predicted_class) ++;
			}
		}

	}
	/*int j;
	for(j = 0; j < class_num; ++ j){
		int k;
		for(k = 0; k < class_num; ++ k){
			cout << this->OOB_confusion_matrix_.at(k).at(j) << " ";
		}
		cout << endl;
	}*/

}

/*
 * we can get the OOB error rate from the OOB confusion matrix.
 */

void RFDeployment::CalculateOOBErrorRate(TrainingSet* training_set){
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	int OOB_num = 0;
	int correct_num = 0;
	int i;
	for(i = 0; i < class_num; ++ i){
		correct_num += this->OOB_confusion_matrix_.at(i).at(i);
		int j;
		for(j = 0; j < class_num; ++ j){
			OOB_num += this->OOB_confusion_matrix_.at(i).at(j);
		}
	}
	this->OOB_error_rate_ = (double)(OOB_num - correct_num) / (double)OOB_num;
	//cout << "OOB_num is " << OOB_num << endl;
	//cout << "debug:OOB error rate is " << this->OOB_error_rate_ << endl;
}

/*
 * calcualte the Q(x,j),it does use the CalculateOOBPredictor function
 */
void RFDeployment::CalculateTheOOBProportion(TrainingSet* training_set){
	int training_set_num = training_set->get_training_set_num_();
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	//AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int trees_num = this->RF_->get_trees_num();
	vector<vector<bool> > selected_status = this->RF_->get_selected_status_();

	/*
	 * note:a instance may paticipate each tree construction
	 */

	int i;
	for(i = 0; i < training_set_num; ++ i){

		vector<int> predicted_classes_num(class_num,0);

		int j;
		int count = 0;
		for(j = 0; j < trees_num; ++ j){
			if(selected_status.at(j).at(i) == false){
				count ++;
				int predict_class = ((LeafNode*)this->random_forests_.at(j)->PredictClass(training_set,i,
													this->random_forests_.at(j)->get_root_()))->get_class_();
				predicted_classes_num.at(predict_class)++;
				if( i == 0)
					cout << "predicted:" << predict_class << " ";

			}
		}
		//cout << "new:"<< count << " ";
		vector<double> temp_vec;
		if(count != 0){
			int k;

			for(k = 0; k < class_num; ++ k){
				temp_vec.push_back((double)predicted_classes_num.at(k) / (double)count);
				if( i == 0){
					cout << "la " << (double)predicted_classes_num.at(k) / (double)count << " ";
				}
			}
			this->OOB_proportion_.push_back(temp_vec);

		}else{
			/*
			 * we can judge a instance which participates all trees construction by this->OOB_proportion.at(i).size() == 0
			 */
			this->OOB_proportion_.push_back(temp_vec);
		}

	}
}

double RFDeployment::CalculateRFStrength(TrainingSet* training_set){
	//this->CalculateTheOOBProportion(training_set);
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	double sum = 0.0;
	double sum_for_expection = 0.0;
	int training_set_num = training_set->get_training_set_num_();
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	int trees_num = this->RF_->get_trees_num();
	int i;
	//cout << "\ndebug new ";
	int valid_num = 0;
	for(i = 0; i < training_set_num; ++ i){
		/*
		 * find the max_j
		 */
		vector<int> classes_num(class_num,0);
		int OOB_count = 0;
		int j;
		for(j = 0; j < trees_num; ++ j){
			if(this->OOB_predictor_.at(i).at(j) != -1){
				OOB_count ++;
				classes_num.at(this->OOB_predictor_.at(i).at(j)) ++;
			}
		}
        if(OOB_count != 0) {
        	valid_num ++;
			int actual_class = value_matrix[target_attribute][i].discrete_value_;
			int max_j;
			int max;
			bool is_max_j_set = false;
			int k;
			for(k = 0; k < class_num; ++ k) {
				/*if(i == 0){
					cout << classes_num.at(k) << " ";
				}*/
				if(k != actual_class) {
					if(!is_max_j_set) {
						max_j = k;
						max = classes_num.at(k);
						is_max_j_set = true;
					} else {
						if(classes_num.at(k) > max){
							max = classes_num.at(k);
							max_j = k;
						}
					}
				}

			}
			this->max_j_.push_back(max_j);
			//cout << "new max_j " << max_j << " ";
			double temp = (double)(classes_num.at(actual_class) - classes_num.at(max_j)) / (double)(OOB_count);
			sum += temp;
			sum_for_expection += temp * temp;
		}else{
			this->max_j_.push_back(-1);
		}
	}
	/*this->OneExpection = sum_for_expection / this->OOB_proportion_.size();
	this->RF_strength_ = sum / this->OOB_proportion_.size();*/

	this->OneExpection = sum_for_expection / valid_num;
	this->RF_strength_ = sum / valid_num;

	return this->RF_strength_;

}

double RFDeployment::CalculateRFCorreration(TrainingSet* training_set){
	int training_set_num = training_set->get_training_set_num_();
	int target_attribute = training_set->GetClassifyAttribute();
	int class_num = training_set->GetAttributeValueNum(target_attribute);
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int trees_num = this->RF_->get_trees_num();
	vector<vector<bool> > selected_status = this->RF_->get_selected_status_();
	double sd_sum = 0.0;
	int i;
	for(i = 0; i < trees_num; ++ i){
		double p1 = 0.0, p2 = 0.0;
		int i1 = 0, i2 = 0;
		int OOB_instance = 0;
		//cout << "tree " << i << endl;
		int j;
		for(j = 0; j < training_set_num; ++ j){
			if(selected_status.at(i).at(j) == false){
				OOB_instance ++;
				int actual_class = value_matrix[target_attribute][j].discrete_value_;
				int max_j = this->max_j_.at(j);
				int predict_class = ((LeafNode*)this->random_forests_.at(i)->PredictClass(training_set,j,
																	this->random_forests_.at(i)->get_root_()))->get_class_();
				if(predict_class == actual_class){
					i1 ++;
				}else if(predict_class == max_j){
					i2 ++;
				}
			}
		}

		p1 = (double)i1 / (double)OOB_instance;
		p2 = (double)i2 / (double)OOB_instance;
		/*
		 *
		 */
		double base = p1 + p2 + (p1 - p2)*(p1 - p2);
		sd_sum += pow(base,0.5);
	}
	double e_sd = sd_sum / trees_num;
	this->RF_correration_ = (this->OneExpection - this->RF_strength_*this->RF_strength_) / (e_sd * e_sd);
	return this->RF_correration_;

}



