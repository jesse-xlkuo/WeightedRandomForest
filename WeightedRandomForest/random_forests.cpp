/*
 * random_forests.cpp
 *
 *  Created on: 31 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */


/*
 * a.the random number generate?
 * b.is the training set repeatalbe?
 * c.the attribute can be repeatable
 */


#include"random_forests.h"

int RandomForests::seed_num_ = 1000;

RandomForests::RandomForests(){

}

RandomForests::RandomForests(TrainingSet* training_set,int trees_num, int subspace){
	this->training_set_=training_set;
	this->trees_num_=trees_num;
	this->subspace_ = subspace;
}

vector<DecisionTree*> RandomForests::get_trees(){
	return this->random_forests_;
}

void RandomForests::GenerateRF(){
	vector<vector<int> > training_sets=this->GetRandomTrainingSet();
	C4_5AttributeSelectionMethod::subspace_ = this->subspace_;
	double i = 0;
	vector<vector<int> >::iterator iter;
	for(iter=training_sets.begin();iter!=training_sets.end();++iter){
		DecisionTree* decision_tree=new DecisionTree(this->training_set_);
		Node* root=decision_tree->GenerateDecisionTreeByC4_5(*iter,this->training_set_->GetNormalAttributes());
		//Node* root=decision_tree->GenerateDecisionTreeByC4_5(this->training_set_,training_set_index,this->training_set_->GetNormalAttributes());
		decision_tree->set_root_(root);
		this->node_num_.push_back(Node::node_num_);
		Node::node_num_ = 0;
		this->random_forests_.push_back(decision_tree);
		i ++ ;
		cout << i << " trees have generated" << endl;
	}

}


//void RandomForests::PrintTrees(){
//	vector<DecisionTree*>::iterator iter;
//	int i = 1;
//	for(iter = this->random_forests_.begin(); iter != this->random_forests_.end(); ++ iter){
//		string name;
//		stringstream ss;
//		ss << i;
//		ss >> name;
//		//name += "-";
//		//name += this->training_set_->get_source_name_();
//		//cout << "name = " << name << endl;
//		ofstream f(name.c_str());
//		f << "DecisionTree {\n\t"
//				<< "Name = \"" << this->training_set_->get_source_name_() << "\"\n\t"
//				<< "TrainingData = \"" << this->training_set_->get_training_set_num_() << "\"\n\t"
//				<< "NumberOfAttributes = \"" << this->training_set_->GetAttributeNum() << "\"\n\t"
//				<< "Classes = \"" << this->training_set_->GetAllClassName() << "\"\n\t"
//				<< "Precision = \"0.8,0.8" << "\"\n\t"
//				<< "SingleTreeStrength = \"0.8" << "\"\n\t"
//				<< "NumberNode = \"" << this->node_num_.at(i-1) << "\"\n"
//				<< "}\n";
//
//		Node* root = (*iter)->get_root_();
//		(*iter)->TraverseTree(f,this->training_set_,root);
//		i ++;
//	}
//}

//int RandomForests::PredictClass(TrainingSet* training_set,int tuple){
//
//}

vector<vector<int> > RandomForests::GetRandomTrainingSet(){
	int max= this->training_set_->get_training_set_num_();
	this->selected_status_ = vector<vector<bool> >(this->trees_num_,vector<bool>(max,false));
	vector<vector<int> > all_vec;
	vector<int> vec;
	int random_num;
	int i;
	int k = 0;
	for(i=0;i<this->trees_num_;++i){
		int j;
		for(j=0;j<max;++j){
			srand(unsigned(time(NULL)+k));
			random_num=rand() % max;
			vec.push_back(random_num);
			this->selected_status_.at(i).at(random_num) = true;
			k++;
		}
		/*
		 * in the bagging, the duplicated instance cat not be discarded
		 */
//		sort(vec.begin(),vec.end());
//		vector<int>::iterator it=unique(vec.begin(),vec.end());
//		vec.resize(it-vec.begin());
		all_vec.push_back(vec);
		vec.clear();

	}
	this->bagging_set_ = all_vec;
	return all_vec;
}

int RandomForests::PredictClass(TrainingSet* training_set,int tuple){
	map<int,int> mapper;
	vector<DecisionTree*>::iterator iter;
	for(iter=this->random_forests_.begin();iter!=this->random_forests_.end();++iter){
		int target_class=((*iter)->PredictClass(training_set,tuple,(*iter)->get_root_()))->get_class_();
		if(mapper.find(target_class)==mapper.end()){
			mapper.insert(map<int,int>::value_type(target_class,1));
		}else{
			mapper[target_class]++;
		}

	}
	int max=0;
	int result;
	map<int,int>::iterator map_iter;
	for(map_iter=mapper.begin();map_iter!=mapper.end();++map_iter){
		if(map_iter->second>=max){
			result=map_iter->first;
			max = map_iter->second;
		}
	}
	return result;
}

double RandomForests::GetErrorRate(TrainingSet* training_set,vector<int> training_set_index){
	int error=0;
	AttributeValue** value_matrix = training_set->GetValueMatrixP();
	int target_attribute = training_set->GetClassifyAttribute();
	vector<int>::iterator iter;
	for(iter=training_set_index.begin();iter!=training_set_index.end();++iter){
		if(this->PredictClass(training_set,*iter) != value_matrix[target_attribute][*iter].discrete_value_){
			error++;
		}
	}
	cout<<"the error num is "<<error<<endl;
	return ((double)error)/((double)training_set_index.size());
}


void RandomForests::GetEachTreeErrorRate(TrainingSet* training_set,vector<int> training_set_index){
	double rate=0;
	int tree_num=0;
	for(vector<DecisionTree*>::iterator it=this->random_forests_.begin();it!=this->random_forests_.end();++it){
		double error_rate=(*it)->GetErrorRate(training_set,training_set_index);
		rate+=error_rate;
		++tree_num;
		cout<<"the tree error rate:"<<error_rate<<endl;
	}
	cout<<"the average error rate:"<<rate/tree_num<<endl;
	
}

vector<vector<int> > RandomForests::get_bagging_set(){
	return this->bagging_set_;
}

int RandomForests::get_trees_num(){
	return this->trees_num_;
}

vector<vector<bool> > RandomForests::get_selected_status_(){
	return this->selected_status_;
}

vector<int> RandomForests::GetRandomSubSpace(vector<int> attribute_list) {
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

void RandomForests::set_OOB_error_rate_(double rate){
	this->OOB_error_rate_ = rate;
}
double RandomForests::get_OOB_error_rate_(){
	return this->OOB_error_rate_;
}
void RandomForests::set_strength_(double strength){
	this->strength_ = strength;
}
double RandomForests::get_strength(){
	return this->strength_;
}
void RandomForests::set_correlation_(double correlation){
	this->correlation_ = correlation;
}
double RandomForests::get_correlation_(){
	return this->correlation_;
}
void RandomForests::set_c_s2_(double c_s2){
	this->c_s2_ = c_s2;
}
double RandomForests::get_c_s2_(){
	return this->c_s2_;
}

void RandomForests::PredictClassToFile(TrainingSet* training_set,
		               AttributeValueMapper* attribute_value_mapper,
		               string file_name){
	ofstream out(file_name.c_str());
	int case_num = training_set->get_training_set_num_();
	int target_attribute = attribute_value_mapper->GetClassifyAttribute();
	int i;
	int lable;
	string result;
	for(i = 0; i < case_num; ++ i){
		lable = this->PredictClass(training_set,i);
		result = attribute_value_mapper->GetAttributeValueName(target_attribute,lable);
		out << result << "\n";
	}
	out.close();
}
