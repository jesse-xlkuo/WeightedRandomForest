/*
 * RFDeployment.h
 *
 *  Created on: 28 Mar, 2012
 *      Author: meng
 */

#ifndef RFDEPLOYMENT_H_
#define RFDEPLOYMENT_H_
#include"utility.h"
#include"random_forests.h"
#include"decision_tree.h"
#include<cmath>
#include<fstream>
using namespace std;

struct confusion_matrix {
	int true_p_;
	int false_n_;
	int false_p_;
	int true_n_;
};

class RFDeployment {
private:

	RandomForests* RF_;
	vector<confusion_matrix> c_matrix_;
	vector<vector<int> > OOB_confusion_matrix_;
	double OOB_error_rate_;
	vector<DecisionTree*> random_forests_;
	double RF_strength_;
	double RF_correration_;
	vector<double> trees_strength_;
	vector<vector<int> > OOB_distribution_;
	/*
	 * OOB_p is two dimension array,Q(x,j)
	 */
	vector<vector<double> > OOB_proportion_;
	vector<int> max_j_;
	//vector<vector<int> > OOB_predict_result_;
	double OneExpection;
	/*
	 * OOB_predicted has training_set_num * trees_num,each element store the predict result of a oob
	 * classifer,if a tree does not a oob classifier, the value is -1; I think it is important when
	 * we calculate the strength,correlation, oob error rate and so on.
	 */
	vector<vector<int> > OOB_predictor_;
	vector<int> OOB_predictor_result_;

	vector<vector<double> > cor_vec_; // correlation between each two trees


public:
	RFDeployment();
	RFDeployment(RandomForests* RF);
	int PredictClass(TrainingSet* training_set,int tuple);
	double GetErrorRate(TrainingSet* training_set,vector<int> training_set_index);
	void GenerateConfusionMatrix(TrainingSet* training_set,vector<int> training_set_index);
	void GetEachTreeErrorRate(TrainingSet* training_set,vector<int> training_set_index);
	vector<vector<double> > get_cor_vec_();
	/*
	 * Get the Random Forest strength by out of bag estimate.
	 * the parameter training_set point to the origin training
	 */
	void CalculateOOBRFCS2(TrainingSet* training_set);
	// the generalized error is estimated by strength and correlation
	//double CalculateCorBetweenTwoTrees(int a, int b, TrainingSet* training_set);

	vector<double> GetEachTreeStrength();
	void CalculateOOBPredictor(TrainingSet* training_set);
	void CalculateOOBPredictorResult(TrainingSet* training_set);
	void CalculateTheOOBProportion(TrainingSet* training_set);
	void CalculateOOBErrorRate(TrainingSet* training_set);
	double CalculateRFStrength(TrainingSet* training_set);
	double CalculateRFCorreration(TrainingSet* training_set);
	void CalculateOOBConfusionMatrix(TrainingSet* training_set);
	void CalculateEachTreeOOBStrength(TrainingSet* training_set);
	double CalculateTwoTreesCorrelation(int a, int b, TrainingSet* training_set);
	void CalculateCorBetweenEachTwoTree(TrainingSet* training_set);
	/*
	 * for VisForests
	 */
	vector<point> CalculateRelativePos();

};

#endif /* RFDEPLOYMENT_H_ */
