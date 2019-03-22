
#include "MainKnapsack.h"
#include <time.h>
#include "Tools.h"
using namespace std;


//
//MainKnapsack* main_Knapsack_PLS(string filename_instance,string type_instance, string num_instance, int size_population, string weighted_DM_preferences){
//
//	MainKnapsack * knaps = new MainKnapsack(filename_instance, type_instance+"_PLS", num_instance, size_population);
//
//	clock_t t1 = clock();
//	knaps->MOLS(t1/CLOCKS_PER_SEC); 								 //3min
//	float t2 = (clock() - t1) * 1.0/CLOCKS_PER_SEC;
//	cout<<"Execution time : "<<t2<<" sec"<<endl<<endl;
//
//	knaps->evaluate_solutions(weighted_DM_preferences,t2);
//
//	knaps->pareto_front_evaluation();
//
//	return knaps;
//
//
//}


MainKnapsack* main_Knapsack(string filename_instance,string type_instance, string num_instance, int size_population, string weighted_DM_preferences){

	string pref_filename = "./WS_Matrix.csv";

	MainKnapsack * knaps = new MainKnapsack(filename_instance, type_instance, num_instance, size_population, pref_filename);

	clock_t t1 = clock();
	knaps->MOLS(t1/CLOCKS_PER_SEC); 								 //3min
	float t2 = (clock() - t1) * 1.0/CLOCKS_PER_SEC;

	cout<<"Execution time : "<<t2<<" sec"<<endl<<endl;

	knaps->evaluate_solutions(weighted_DM_preferences,t2);

	knaps->pareto_front_evaluation();

	return knaps;
}




//MainKnapsack* main_Knapsack_WSPLS(string filename_instance,string type_instance, string num_instance, int size_population, string weighted_DM_preferences){
//
//	string pref_filename = "./WS_Matrix.csv";
//
//	MainKnapsack * knaps = new MainKnapsack(filename_instance, type_instance+"_WS", num_instance, size_population, pref_filename);
//
//	clock_t t1 = clock();
//	knaps->HYBRID_WS_PLS(t1/CLOCKS_PER_SEC, 3);
//	float t2 = (clock() - t1) * 1.0/CLOCKS_PER_SEC;
//
//	cout<<"Execution time : "<<t2<<" sec"<<endl<<endl;
//
//	knaps->evaluate_solutions(weighted_DM_preferences,t2);
//
//	knaps->pareto_front_evaluation();
//
//	return knaps;
//}












int main(int argc, char** argv){

	string WS_DM = "./weighted_DM_preferences.ks";

	string type_inst = "C";
	string taille = "100";

	MainKnapsack * knaps;




////PLS
//		for(int i = 7; i < 8; i++){
//			//without extension
//			string filename_instance = "./Instances_Knapsack/Type_"+type_inst+"/"+taille+"_items/2KP"+taille+"-T"+type_inst+"-"+to_string(i);
//			knaps = main_Knapsack_PLS(filename_instance, type_inst,to_string(i),1,WS_DM);
//		}


	//WSLS
	for(int step = 0; step < 8; step++){
		//SET COEFF_OBJECTIVE      !!!!!!!!!!!!!!!!!!!!! CHANGE DMS WSUMM FOR TEST1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		Tools::copy_into("./Data/WS_Learning/Test2/Iteration_"+to_string(step),"WS_Matrix.csv");

		for(int i = 0; i < 10; i++){

			for(int k = 0; k < 30; k++){
	//		Tools::generate_random_WS("WS_Matrix.csv",2);
				//without extension
				string filename_instance = "./Instances_Knapsack/Type_"+type_inst+"/"+taille+"_items/2KP"+taille+"-T"+type_inst+"-"+to_string(i);

				knaps = main_Knapsack(filename_instance, type_inst,to_string(i),1,WS_DM);
			}

			Tools::compute_average_column_files("./Data/DistTime/"+type_inst+"/I"+to_string(i)+"_"+taille+".eval",2);

			Tools::compute_average_column_files("./Data/ParetoFront/"+type_inst+"/I"+to_string(i)+"_"+taille+".front",3);

			knaps->write_coeff_functions("./Data/DistTime/"+type_inst+"/I"+to_string(i)+"_"+taille+".eval");
			knaps->write_coeff_functions("./Data/ParetoFront/"+type_inst+"/I"+to_string(i)+"_"+taille+".front");

		}

	}



//	Tools::generate_random_WS("WS_Matrix.csv",2);
//	for(int i = 7; i < 8; i++){
//		//without extension
//		string filename_instance = "./Instances_Knapsack/Type_"+type_inst+"/"+taille+"_items/2KP"+taille+"-T"+type_inst+"-"+to_string(i);
//		knaps = main_Knapsack_WSPLS(filename_instance, type_inst,to_string(i),1,WS_DM);
//	}



	return 1;

}
