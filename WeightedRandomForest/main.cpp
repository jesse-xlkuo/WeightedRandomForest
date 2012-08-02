/*
 * main.cpp
 *
 *  Created on: 31 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 *
 *  main function is in the file
 */


#include<iostream>
#include<ctime>
#include"utility.h"
#include"attribute_value_mapper.h"
#include"c4_5_attribute_selection_method.h"
#include"training_set.h"
#include"decision_tree.h"
#include"random_forests.h"
#include"RF_deployment.h"
#include"IGR.h"
#include<fstream>
/*
 * #include<unistd.h>
 * include the getopt function,you can find the getopt function information in
 * http://www.gnu.org/savannah-checkouts/gnu/libc/manual/html_node/Using-Getopt.html#Using-Getopt
 * -n 80 -f /home/meng/workspace/DataSet/fbis -o -t
 * out of bag 评估应该在生成模型都应该得到评估，不给用户选择。
 * 模型保存应该在所有随机森林数据都产生再保存，注意顺序
 */
#include<unistd.h>
using namespace std;

int main(int argc,char* argv[]){
	clock_t start = clock();
	bool have_model = false;
	bool model_test = false;
	bool OOB_estimate = false;
	bool save_model = false;
	string model_name = "";
	bool lable_task = false;
	void about();
	about();
	void help();
    string stream_name;
    int tree_num;
    int opt;
    opterr = 0;
    while((opt = getopt(argc,argv,"p:l:f:n:htos:")) != -1){
    	switch(opt){
    	case 'l':
    		have_model = true;
    		model_name = optarg;
    		break;
    	case 'p':
    		lable_task = true;
    		stream_name = optarg;
    		break;
		case 'f':
			stream_name = optarg;
			//cout << "Data stream is " << stream_name << endl;
			break;
		case 'n':
			tree_num = atoi(optarg);
			//cout << "Trees number is " << tree_num << endl;
			break;
		case 't':
			model_test = true;
			break;
		case 'h':
			help();
			break;
		case 'o':
			OOB_estimate = true;
			break;
		case 's':
			model_name = optarg;
			save_model = true;
			break;
    	}
    }

    if(have_model){
    	RandomForests random_forests;
    	random_forests.load_random_forests(random_forests,model_name.c_str());
    	cout << "out bag of error rate is " << random_forests.get_OOB_error_rate_() << endl;
    	cout << "random forests strength is " << random_forests.get_strength() << endl;
    	cout << "random forests correlation is " << random_forests.get_correlation_() << endl;
    	cout << "random forests c/s2 is " << random_forests.get_c_s2_() << endl;
    	if(lable_task){
    		string unlable_data = stream_name + ".unlable";
    		cout << unlable_data <<endl;
    		string names = stream_name + ".names";
    		cout << names <<endl;
    		AttributeValueMapper * attribute_value_mapper = new AttributeValueMapper(names);
			if (attribute_value_mapper->MapAttributeValue() == FAIL) {
				cout << "Can not read the names file !" << endl;
				return 1;
			}
			TrainingSet* training_set = new TrainingSet();
			training_set->set_source_name_(stream_name);
			training_set->set_attribute_value_mapper(*attribute_value_mapper);
			training_set->ProduceTrainingSetMatrix(unlable_data);
			random_forests.PredictClassToFile(training_set,attribute_value_mapper,"out");
//			vector<int> training_set_index_predict;
//			int test_num = training_set->get_training_set_num_();
//			int i;
//			for (i = 0; i < test_num; i++) {
//				training_set_index_predict.push_back(i);
//			}
//			cout << random_forests.GetErrorRate(training_set,training_set_index_predict) <<endl;

    	}
    	return 0;
    }

	string data_file = stream_name + ".data";
	string test_file = stream_name + ".test";
	string name_file = stream_name + ".names";

	cout << "\nTry to generate the training set......" << endl;

    AttributeValueMapper * attribute_value_mapper = new AttributeValueMapper(name_file);
	if ( attribute_value_mapper->MapAttributeValue() == FAIL ){
		cout << "Can not read the names file !" << endl;
		return 2;
	}

	TrainingSet*  training_set = new TrainingSet();
	training_set->set_source_name_(stream_name);
	training_set->set_attribute_value_mapper(*attribute_value_mapper);
	training_set->ProduceTrainingSetMatrix(data_file);

	cout << "Read "<< training_set->get_training_set_num_()
			<< " cases(" << training_set->GetAttributeNum() - 1
			<< " attributes)from the data"<< endl;

	cout << "\ntry to generate the random forests......" << endl;

	RandomForests* random_forests = new RandomForests(training_set,tree_num);
	random_forests->GenerateRF();

	cout << "random forests has generated successfully!" << endl;



	if(OOB_estimate){

		RFDeployment RF_deployment(random_forests);

		RF_deployment.CalculateOOBPredictor(training_set);

		RF_deployment.CalculateOOBPredictorResult(training_set);
		RF_deployment.CalculateOOBConfusionMatrix(training_set);
		RF_deployment.CalculateOOBErrorRate(training_set);
//		cout <<"out of bag error rate:" << RF_deployment.get_OOB_error_rate_() << endl;
		random_forests->set_OOB_error_rate_(RF_deployment.get_OOB_error_rate_());
		//RF_deployment.CalculateTwoTreesCorrelation(0,1,training_set);
		/*
		 * before we calculate the CalculateCorBetweenTwoTrees(),should first run CalculateOOBRFCS2() function
		 */
		//RF_deployment.CalculateOOBRFCS2(training_set);
		//cout << RF_deployment.CalculateCorBetweenTwoTrees(0,0,training_set) << endl;
		//cout << RF_deployment.CalculateTwoTreesCorrelation(0,0,training_set) << endl;

		RF_deployment.CalculateRFStrength(training_set);
		RF_deployment.CalculateRFCorrelation(training_set);
		double s = RF_deployment.get_RF_strength_();
		random_forests->set_strength_(s);
		double c = RF_deployment.get_RF_correlation();
		random_forests->set_correlation_(c);
		random_forests->set_c_s2_(c / (s*s));

		random_forests->save_random_forests(*random_forests,"model");

		cout << "out bag of error rate is " << random_forests->get_OOB_error_rate_() << endl;
		cout << "random forests strength is" << random_forests->get_strength() << endl;
		cout << "random forests correlation is " << random_forests->get_correlation_() << endl;
		cout << "random forests c/s2 is " << random_forests->get_c_s2_() << endl;


//		RF_deployment.CalculateCorBetweenEachTwoTree(training_set);
//
//		vector<vector<double> > cor = RF_deployment.get_cor_vec_();
//		ofstream o("correlation");
//		int i;
//		for(i = 0; i < tree_num; ++ i){
//			int j;
//			for(j = 0; j < tree_num; ++ j){
//				o << cor.at(i).at(j) << "\n";
//			}
//		}
//		o.close();
//		vector<point> pos = RF_deployment.CalculateRelativePos();
//		cout << "point num:" << pos.size() << endl;
//		for(i = 0; i < tree_num; ++ i){
//			cout << "i:" << i << " " <<  pos.at(i).x_ << " " << pos.at(i).y_ << "\n";
//
//		}

	}

	if(model_test){
			TrainingSet* test_set = new TrainingSet();
			test_set->set_attribute_value_mapper(*attribute_value_mapper);
			test_set->ProduceTrainingSetMatrix(test_file);
			delete attribute_value_mapper;
			vector<int> training_set_index_predict;
			int test_num = test_set->get_training_set_num_();
			int i;
			for (i = 0; i < test_num; i++) {
				training_set_index_predict.push_back(i);
			}

			cout << "read " << test_num <<  " test cases from the test file" << endl;
			RFDeployment RF_deployment(random_forests);
			//RF_deployment.CalculateOOBRFCS2(training_set);
			//RF_deployment.CalculateCortweenEachTwoTree(training_set);
			/*
			 * test:get each tree strengthTrainingSet
			 */
			//RF_deployment.CalculateOOBRFCS2(training_set);
			//vector<double> trees_strength = RF_deployment.GetEachTreeStrength();
			//for(vector<double>::iterator it = trees_strength.begin(); it != trees_strength.end(); ++it){
			//	cout << *it << " ";
			//}
			//cout << endl;
			RandomForests rf;
			rf.load_random_forests(rf, "model");
	        cout << 1 - rf.GetErrorRate(test_set,training_set_index_predict) << endl;


			cout << "the model is tested with the predicting accuracy of "<< 1 - RF_deployment.GetErrorRate(test_set,training_set_index_predict)
				 << endl;
		}
	clock_t finish = clock();
	cout << "the program execution time is " << ((finish - start) / CLOCKS_PER_SEC ) / 60.0 << " minutes" << endl;
	return 0;
}

//int main(){
//	string stream_name = "/home/meng/workspace/DataSet/fbis";
//	string name_file = stream_name + ".names";
//	string data_file = stream_name + ".data";
//	AttributeValueMapper * attribute_value_mapper = new AttributeValueMapper(name_file);
//	if ( attribute_value_mapper->MapAttributeValue() == FAIL ){
//			cout << "Can not read the names file !" << endl;
//			return 2;
//	}
//	TrainingSet*  training_set = new TrainingSet();
//	training_set->set_source_name_(stream_name);
//	training_set->set_attribute_value_mapper(*attribute_value_mapper);
//	training_set->ProduceTrainingSetMatrix(data_file);
//	DecisionTree decision_tree;
//	vector<int> training_set_index;
//	int training_set_num = training_set->get_training_set_num_();
//	int i;
//	for(i = 0; i < training_set_num; ++ i){
//		training_set_index.push_back(i);
//	}
//	decision_tree.GenerateDecisionTreeByC4_5(training_set, training_set_index ,training_set->GetNormalAttributes());
//}

//int main(){
//	vector<double> vec;
//	int i;
//	for (i = 0; i < 10000; i ++){
//		vec.push_back((double)i / 10000.0);
//	}
//	IGR igr;
//	igr.CalculateWeight(vec);
//	cout << "selected:" << igr.GetSelectedResult() << endl;;
//
//}

void about(){
	cout    << "\nWeightedRandomForestsV1.0.\n"
		    << "Author:Qinghan Meng.email:qinghan.meng@gmail.com\n"
		    << endl;
}
void help(){
	cout<< "-f   " << "refer the data stream name\n"
	    << "-n   " << "refer the tree numbers\n"
	    << "-t   " << "indicate do the model test using the test file\n"
	    << "-o   " << "do out-of-bag estimate\n"
	    << "-h   " << "show the help information\n"
	    << "-s   " << "save the model,you can indicate the path and name of the model"
	    << endl;

}







































//
//	vector<int> training_set_index;
//	int training_set_num = training_set->get_training_set_num_();
//	int i;
//	for(i=0;i<training_set_num;i++){
//		training_set_index.push_back(i);
//	}

		

	

    /*cout << "step five !" << endl;

	C4_5AttributeSelectionMethod method(p,training_set_index,p->GetNormalAttributes());
	method.HandleContinuousAttribute(0);*/
//
//    cout << "Try to generate the decision  tree! " << endl;
//	DecisionTree decision_tree;
//	Node* root=decision_tree.GenerateDecisionTreeByC4_5(training_set,training_set_index,training_set->GetNormalAttributes());
//	decision_tree.set_root_(root);
//	decision_tree.TraverseTree(training_set,decision_tree.get_root_(),0);
//
//    cout << "Decision tree is generated successfully !" << endl;
//
//    cout << "Try to evaluate the tree by test data ! " << endl;
//	DataFileReader dt;
//	dt.setFilePath(test_file);
//	dt.setDelim(',');
//	if ( dt.read() == SUCCESS ){
//		TrainingSet* test = new TrainingSet();
//        test->set_attribute_value_mapper(*attribute_value_mapper);
//	    test->ProduceTrainingSetMatrix(dt);
//        vector<int> training_set_index_predict;
//	    int test_num = test->get_training_set_num_();
//	    for(i=0;i<test_num;i++){
//		    training_set_index_predict.push_back(i);
//	    }
//	    cout<<"the error rate is:"<<decision_tree.GetErrorRate(test,training_set_index_predict)<<endl;
//	}else {
//		cout << " Can't find the test data ! " << endl;
//	}


//    cout << "Try to produce the random forests which have " << tree_num << " trees !" << endl;
//	RandomForests* random_forests = new RandomForests(training_set,tree_num);
//	random_forests->GenerateRF();
//	//random_forests->PrintTrees();
//	cout << "The random forests generates successfully !" << endl;
//	cout << "Try to evaluate on the tree !" << endl;
//	DataFileReader *dt = new DataFileReader();
//	dt->setFilePath(test_file);
//	dt->setDelim(',');
	//if ( dt->read() == SUCCESS ){
//
//
	//}

