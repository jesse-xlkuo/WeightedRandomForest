/*
 * training_set.cpp
 *
 *  Created on: 29 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */
#include"training_set.h"

void TrainingSet::set_attribute_value_mapper(
		AttributeValueMapper attribute_value_mapper) {
	this->attribute_value_mapper_ = attribute_value_mapper;
}

int TrainingSet::get_training_set_num_() {
	return this->training_set_num_;
}


int TrainingSet::GetClassifyAttribute() {
	return this->attribute_value_mapper_.GetClassifyAttribute();
}

AttributeType TrainingSet::GetAttributeType(int attribute) {
	return this->attribute_value_mapper_.GetAttributeType(attribute);
}

int TrainingSet::GetAttributeNum() {
	return this->attribute_value_mapper_.GetAttributeNum();
}

vector<int> TrainingSet::GetNormalAttributes() {
	vector<int> normal_attributes;
	int i;
	int attribute_num = this->GetAttributeNum();
	int classify_attribute = this->GetClassifyAttribute();
	for (i = 0; i < attribute_num; ++i) {
		if (i == classify_attribute) {
			continue;
		} else {
			normal_attributes.push_back(i);
		}
	}
	return normal_attributes;
}

int TrainingSet::GetAttributeValueNum(int attribute) {
	return this->attribute_value_mapper_.GetAttributeValueNum(attribute);
}

map<int, double> TrainingSet::GetClassDistribution(
		vector<int> training_set_index) {
	int classify_attribute = this->GetClassifyAttribute();
	int training_set_num = training_set_index.size();
	map<int, int> mapper;
	int i;
	int class_index;
	for (i = 0; i < training_set_num; i++) {
		class_index =
				this->value_matrix_[classify_attribute][training_set_index.at(i)].discrete_value_;
		if (mapper.find(class_index) == mapper.end()) {
			mapper.insert(map<int, int>::value_type(class_index, 1));
		} else {
			mapper[class_index]++;
		}
	}
	map<int, double> result;
	for (map<int, int>::iterator iter = mapper.begin(); iter != mapper.end();
			++iter) {
		result.insert(
				map<int, double>::value_type(iter->first,
						(double) iter->second / (double) training_set_num));
	}
	return result;
}

int TrainingSet::GetTheMostClass(vector<int> training_set_index) {
	int classify_attribute = this->GetClassifyAttribute();
	int training_set_num = training_set_index.size();
	map<int, int> mapper;
	int i;
	int class_index;
	for (i = 0; i < training_set_num; i++) {
		//class_index=this->training_set_.at(training_set_index.at(i)).GetClass(classify_attribute);
		class_index =
				this->value_matrix_[classify_attribute][training_set_index.at(i)].discrete_value_;
		if (mapper.find(class_index) == mapper.end()) {
			mapper.insert(map<int, int>::value_type(class_index, 1));
		} else {
			mapper[class_index]++;
		}
	}
	map<int, int>::iterator iter;
	int temp_num = 0;
	int result;
	for (iter = mapper.begin(); iter != mapper.end(); ++iter) {
		if (iter->second > temp_num) {
			temp_num = iter->second;
			result = iter->first;
		}
	}
	return result;
}

bool TrainingSet::IsTrainingSetSameClass(vector<int> training_set_index) {
	int training_set_num = training_set_index.size();
	if (training_set_num == 0) {
		cout << "error in training_set.cpp function: IsTrainingSetSameClass"
				<< endl;
		exit(1);
	} else if (training_set_num == 1) {
		return true;
	} else {
		int target_attribute = this->GetClassifyAttribute();
		int i = 0;
		//int class_one=this->training_set_.at(training_set_index.at(i)).GetClass(this->GetClassifyAttribute());
		int class_one =
				this->value_matrix_[target_attribute][training_set_index.at(i)].discrete_value_;
		i++;
		while (i < training_set_num) {
			if (class_one
					!= this->value_matrix_[target_attribute][training_set_index.at(
							i)].discrete_value_) {
				return false;
			}
			i++;
		}
		return true;

	}
}



map<int, vector<int> > TrainingSet::SplitByDiscreteAttribute(
		vector<int> training_set_index, int attribute) {
	vector<int> temp_vec;
	map<int, vector<int> > result;
	int i;
	for (i = 0; i < this->GetAttributeValueNum(attribute); ++i) {
		result.insert(map<int, vector<int> >::value_type(i, temp_vec));
	}
	int training_set_num = training_set_index.size();
	for (i = 0; i < training_set_num; ++i) {
		int attribute_value =
				this->value_matrix_[attribute][training_set_index.at(i)].discrete_value_;
		result[attribute_value].push_back(training_set_index.at(i));
	}
	return result;

}

map<int, vector<int> > TrainingSet::SplitByPositon(
		vector<int> training_set_index, int pos) {
	int training_set_num = training_set_index.size();
	map<int, vector<int> > mapper;
	if (pos < 0 || pos >= training_set_num) {
		cout << "wrong in training_set.cpp:line 220" << endl;
		exit(1);
	} else {
		vector<int> vec;
		int i;
		for (i = 0; i <= pos; ++i) {
			vec.push_back(training_set_index.at(i));
		}
		mapper.insert(map<int, vector<int> >::value_type(0, vec));
		vec.clear();
		for (i = pos + 1; i < training_set_num; ++i) {
			vec.push_back(training_set_index.at(i));
		}
		mapper.insert(map<int, vector<int> >::value_type(1, vec));
	}
	return mapper;
}

void TrainingSet::DeleteOneAttribute(vector<int>& attribute_list,
		int attribute) {
	vector<int>::iterator iter;
	for (iter = attribute_list.begin(); iter != attribute_list.end(); ++iter) {
		if ((*iter) == attribute) {
			attribute_list.erase(iter);
			break;
		}
	}

}

string TrainingSet::GetAttributeName(int attribute) {
	return this->attribute_value_mapper_.GetAttributeName(attribute);
}

string TrainingSet::GetAttributeValueName(int attribute, int index) {
	return this->attribute_value_mapper_.GetAttributeValueName(attribute, index);
}

vector<int> TrainingSet::GetClassesNum(vector<int> training_set_index) {
	int classify_attribute = this->GetClassifyAttribute();
	int training_set_num = training_set_index.size();
	int class_num = this->GetAttributeValueNum(classify_attribute);
	vector<int> mapper;
	int i;
	for (i = 0; i < class_num; ++i) {
		mapper.push_back(0);
	}
	int class_index;
	for (i = 0; i < training_set_num; i++) {
		class_index =
				this->value_matrix_[classify_attribute][training_set_index.at(i)].discrete_value_;
		mapper.at(class_index)++;}
return mapper	;
}

void TrainingSet::TestMatrix() {
	int attribute_num = this->GetAttributeNum();
	int training_set_num = this->get_training_set_num_();
	int i, j;
	for (i = 0; i < attribute_num; ++i) {
		if (this->GetAttributeType(i) == CONTINUOUS) {
			for (j = 0; j < training_set_num; ++j) {
				cout << this->value_matrix_[i][j].continuous_value_ << " ";
			}
		} else {
			for (j = 0; j < training_set_num; ++j) {
				cout << this->value_matrix_[i][j].discrete_value_ << " ";
			}
		}
		cout << endl;
	}

}

AttributeValue** TrainingSet::GetValueMatrixP() {
	return this->value_matrix_;

}

StatusCode TrainingSet::ProduceTrainingSetMatrix(string filePath) {
	if (filePath.empty()) {
		return FAIL;
	}
	ifstream reader;
	reader.open(filePath.c_str(), ios_base::in);
	if (!reader.is_open()) {
		return FAIL;
	}

	int attribute_num = this->attribute_value_mapper_.GetAttributeNum();
	this->value_matrix_ = (AttributeValue**) malloc(sizeof(AttributeValue*) * attribute_num);
	int index;
	for (index = 0; index < attribute_num; ++index) {
		this->value_matrix_[index] = (AttributeValue*) malloc(sizeof(AttributeValue) * 100);
	}

	int training_set_num = 0;
	int line_count = 0;
	char tempCharArr[100000];
	vector<string> tempStrVec;
	while (reader.getline(tempCharArr, 100000)) {
		string tempStr = tempCharArr;
		split(tempStr, ',', tempStrVec);
		/*
		 * we have got one line data
		 */

		line_count ++;
		training_set_num ++;
		if (tempStrVec.size() == attribute_num) {
			int j;
			for (j = 0; j < attribute_num; ++j) {
				if (this->attribute_value_mapper_.GetAttributeType(j) == CONTINUOUS) {
					double continuous_value = atof(tempStrVec.at(j).c_str());
					//stringstream ss;
					//ss << tempStrVec.at(j);
					//ss >> continuous_value;
					this->value_matrix_[j][training_set_num - 1].continuous_value_ =
							continuous_value;
				} else {
					map<string, int>::iterator it;
					if (this->attribute_value_mapper_.GetMapperValue(j,tempStrVec.at(j), it)) {

						this->value_matrix_[j][training_set_num - 1].discrete_value_ = it->second;
					} else {
						cout << " a error in data file: line " << training_set_num << endl;
						exit(1);
					}
				}
			}
		} else {
			cout << "a error in data file:line " << training_set_num << endl;
			cout << "the program exit !" << endl;
			exit(1);
		}
		if(line_count >= 100){
			for (index = 0; index < attribute_num; ++index) {
					this->value_matrix_[index] = (AttributeValue*) realloc(this->value_matrix_[index],sizeof(AttributeValue) * (training_set_num + 100));
			}
			line_count = 0;
		}
		tempStrVec.clear();
	}
	this->training_set_num_ = training_set_num;
	reader.close();
	return SUCCESS;

}

StatusCode TrainingSet::split(const string &str, char delim, vector<string> &strVec) {
	string tempWord;
	stringstream tempSS(str);
	while (getline(tempSS, tempWord, delim)) {
		strVec.push_back(tempWord);
	}
	return SUCCESS;

}


string TrainingSet::GetAllDAttributeValue(int attribute){
	string result = "";
	int attribute_value_num = this->GetAttributeValueNum(attribute);
	int i = 0;
	while(i < attribute_value_num - 1){
		result += this->GetAttributeValueName(attribute,i);
		result += ",";
		i ++;
	}
	result += this->GetAttributeValueName(attribute,i);
	return result;

}

void TrainingSet::set_source_name_(string source_name){
	this->source_name_ = source_name;
}
string TrainingSet::get_source_name_(){
	return this->source_name_;
}

string TrainingSet::GetAllClassName(){
	int target_attribute = this->GetClassifyAttribute();
	int class_num = this->GetAttributeValueNum(target_attribute);
	int i = 0;
	string result = "";
	while(i < class_num - 1){
		result += this->GetAttributeValueName(target_attribute,i);
		result += ",";
		++ i;
	}
	result += this->GetAttributeValueName(target_attribute,i);
	return result;

}
