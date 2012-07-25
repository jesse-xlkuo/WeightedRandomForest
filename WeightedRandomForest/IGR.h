/*
 * IGR.h
 *
 *  Created on: 25 Jul, 2012
 *      Author: meng
 */

#ifndef IGR_H_
#define IGR_H_
#include<vector>
#include<cmath>
using namespace std;


class IGR {
private:
	int m_; //subspace size
	vector<double> weights_;
public:
	IGR();
	void CalculateWeight(vector<double> gain_ratio);
	int GetSelectedResult();
};

#endif /* IGR_H_ */
