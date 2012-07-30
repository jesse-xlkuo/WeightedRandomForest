/*
 * IGR.cpp
 *
 *  Created on: 25 Jul, 2012
 *      Author: meng
 */

#include "IGR.h"
#include<iostream>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>
boost::mt19937 gen;

IGR::IGR(){

}

void IGR::CalculateWeight(vector<double> gain_ratio){
//	this->weights_ = (double*)malloc(gain_ratio.size()*sizeof(double));
	this->m_ = log((double) gain_ratio.size()) / log(2.0) + 1;
	double sum = 0.0;
	vector<double> vec;
	double temp;
	vector<double>::iterator it;
	for(it = gain_ratio.begin(); it != gain_ratio.end(); ++ it){
		temp = pow(*it,0.5);
		vec.push_back(temp);
		sum += temp;
	}
	for(it = vec.begin(); it != vec.end(); ++ it){
		//this->weights_[i] = *it / sum;
		this->weights_.push_back(*it / sum);
	}

}

/*
 * Note:the same weights distirbution ,the same random integer generated
 */

int IGR::GetSelectedResult(){
	boost::random::discrete_distribution<> dist(this->weights_);
	int rand_num;
	int max;
	bool is_max_set = false;
	int i;
	for(i = 0; i < this->m_; i ++){

		rand_num = dist(gen);

		if(is_max_set){
			if(this->weights_[rand_num] > this->weights_[max]){
				max = rand_num;
			}
		}else{
			max = rand_num;
			is_max_set = true;
		}

	}
	if(is_max_set){
		return max;
	}else{
		return -1;
	}


}
