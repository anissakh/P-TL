
#include <set>
#include <ilcplex/ilocplex.h>

#include "MainKnapsack.h"


MainKnapsack::MainKnapsack(string filename,string type_inst, string num_inst, int init_population_size , string matrix_filename){

	filename_instance = filename;
	type_instance = type_inst;
	num_instance = num_inst;

	readFilenameInstance(filename_instance);

	readWS_Matrix(matrix_filename);

	GenerateInitialPopulation(init_population_size);

//	readInitPopulationFile(init_population_filename);

	readParetoFront();


}


/**
 * ************************************************* READING PART (INITIALIZATION)  *************************************************
 */

//void MainKnapsack::readInitPopulationFile(string filename){
//
//	Population.clear();
//	ifstream fic(filename.c_str());
//
//
//	if (!(fic) or filename.find(".ks") == std::string::npos){
//		cerr<<"Error occurred"<<endl;
//	}
//
//	string line;
//	float total_weight;
//
//	while(!fic.eof()){
//		total_weight = 0;
//
//		getline(fic,line);
//
//		if( line.size() == 0)
//			continue;
//
//		set<int> set_items;
//		char *cline = new char[line.length() + 1];
//		std::strcpy(cline, line.c_str());
//
//		char * pch = strtok (cline," 	,");
//		while (pch != NULL){
//			set_items.insert(atoi(pch));
//			total_weight += std::get<0>(Items_information[atoi(pch)]);
//			pch = strtok (NULL, " 	,");
//		}
//
//		if(total_weight > Backpack_capacity){
//			cerr<<"Not feasable initialization"<<endl;
//			exit(1);
//		}
//
//		AlternativeKnapsack* alt = new AlternativeKnapsack(set_items, this);
//		Population.push_back(alt);
//
//	}
//	cout<<"   Taille de la population initiale ("<<to_string(Population.size())<<")"<<endl;
//
//	if( Population.size() == 0){
//		set<int> empt;
//		AlternativeKnapsack *alt = new AlternativeKnapsack(empt, this);
//		Population.push_back(alt);
//	}
//	fic.close();
//}




void MainKnapsack::readFilenameInstance(string filename){

	string line;
	char buff[100];
	int i = 0;

	vector< float > line_value;

	ostringstream FileName;
	FileName.str("");
	FileName <<filename.c_str()<<".dat";
	ifstream fic(FileName.str());


	//comments
	getline(fic,line);
	while( line[0] == 'c' )
		getline(fic,line);

	//number of items
	n_items = 0;
	if( line[0] == 'n')
		sscanf(line.c_str(),"%s %d",buff,&n_items);

	Items_information.resize(n_items);

	//comments
	getline(fic,line);
	while( line[0] == 'c' )
		getline(fic,line);

	//items information


	while(line[0] == 'i'){

		line.erase(line.begin());
		char *cline = new char[line.length() + 1]; // or
		std::strcpy(cline, line.c_str());

		line_value.clear();
		char * pch = strtok (cline," 	");
		while (pch != NULL){
			line_value.push_back(atof(pch));
			pch = strtok (NULL, " 	");
		}

		float weight = line_value[0];

		line_value.erase(line_value.begin());

		Items_information[i] = make_tuple(weight,line_value);


		getline(fic,line);
		i++;
	}

	//number of criteria
	p_criteria = 0;
	p_criteria = line_value.size();

	//comments
	while( line[0] == 'c' )
		getline(fic,line);


	//total capacity
	if( line[0] == 'W' )
		sscanf(line.c_str(),"%s %f",buff,&Backpack_capacity);

	cout<<"Information sur l'instance : "<<endl;
	cout<<"   Instance de taille ("<<to_string(n_items)<<")"<<endl;
	cout<<"   Capacité du sac-à-dos ("<<to_string(Backpack_capacity)<<")"<<endl;
	cout<<"   Nombre de criètres ("<<to_string(p_criteria)<<")"<<endl;

	fic.close();
}


void MainKnapsack::readWS_Matrix(string filename){

	if(filename.find("PARETO") != std::string::npos)
		change_to_pareto_selection();

	else {
		ifstream fic(filename.c_str());
		string line;
		int i = 0;
		if (!(fic) or filename.find(".csv") == std::string::npos){
			cerr<<"Error occurred fileMatrix"<<endl;
		}

		WS_matrix.resize(p_criteria,vector< float >());

		while( !fic.eof() and i < p_criteria){
			WS_matrix[i].clear();
			getline(fic,line);

			char *cline = new char[line.length() + 1]; // or
			std::strcpy(cline, line.c_str());

			char * pch = strtok (cline," 	,;");
			while (pch != NULL){
				WS_matrix[i].push_back(atof(pch));
				pch = strtok (NULL, " 	,;");
			}
			i++;
		}
	}

	n_objective = WS_matrix[0].size();


	cout<<"   Matrice des objectives :"<<endl;
	for(int i = 0; i < p_criteria; i++){
		cout<<"   ";
		for(int j = 0; j < n_objective; j++)
			cout<<WS_matrix[i][j]<< " ";
		cout<<endl;
	}
}


void MainKnapsack::write_solution(string filename){

	ostringstream FileName;
	FileName.str("");
	FileName <<filename.c_str();
	ofstream fic(FileName.str().c_str());

	for(list< Alternative *>::iterator alt = OPT_Solution.begin(); alt != OPT_Solution.end(); alt++){
		for(int i = 0; i < p_criteria; i++)
			fic<< (*alt)->get_criteria_values()[i]<< " ";
		fic<<endl;
	}
}


void MainKnapsack::readParetoFront(){

	string file_extension = filename_instance+".eff";
	ifstream fic(file_extension.c_str());
	string line;

	if (!(fic) or file_extension.find(".eff") == std::string::npos){
		cerr<<"Error occurred paretofront"<<endl;
	}

	ParetoFront.clear();

	while(!fic.eof()){

		getline(fic,line);
		if (line.size() == 0)
			continue;

		vector< float > vector_objective = Tools::decompose_line_to_float_vector(line);

		ParetoFront.push_back(vector_objective);
	}

}


void MainKnapsack::write_coeff_functions(string filename ){

	ostringstream FileName;
	FileName.str("");
	FileName <<filename.c_str();
	ofstream fic(FileName.str().c_str(), ios::app);

	fic<<endl<<"Optimal solution :"<<endl;
	fic<<"("; for(int i = 0; i < p_criteria; i++) fic<< OPT_Alternative->get_criteria(i) <<", "; fic<<")"<<endl;
	fic<<endl<<"Matrice des objectives & Optimal solution:"<<endl;

	for(int i = 0; i < p_criteria; i++){
		fic<<"   ";
		for(int j = 0; j < n_objective; j++)
			fic<<to_string(WS_matrix[i][j])<< " ";
		fic<<endl;
	}
	fic<<endl;

	fic.close();
}


void MainKnapsack::GenerateInitialPopulation(int size_population){

	float bp = 0;

	vector<int> all_items;

	for(int i = 0; i < n_items; i++)
		all_items.push_back(i);

	for (int i = 0; i < size_population; i++){
		bp = 0;
		set<int> individu;
		list<int> items(all_items.begin(), all_items.end());

		while(bp < Backpack_capacity  and (items.size() != 0)  and (rand()*1.0/RAND_MAX) < 0.7){
			list<int>::iterator iteratour = items.begin();
			advance(iteratour, rand()%items.size() );

			int itm = *iteratour;// items[ rand()%items.size() ];
			items.remove( itm );

			if ( (get_weight_of(itm) + bp) > Backpack_capacity )
				continue;

			individu.insert(itm);
			bp += get_weight_of(itm);
		}
		AlternativeKnapsack * alt = new AlternativeKnapsack(individu, this);
		Population.push_back(alt);
	}

}


void MainKnapsack::save_new_point(string filename, Alternative * alt){
	ostringstream FileName;
	FileName.str("");
	FileName <<filename.c_str();
	ofstream fic(FileName.str().c_str(),ios::app);

	for(int i = 0; i < p_criteria; i++)
		fic<< alt->get_criteria_values()[i]<< " ";
	fic<<endl;

	fic.close();
}

/**
 * ************************************************* SOLVING PART *************************************************
 */

//Filter the final archive
void MainKnapsack::filter_efficient_set(){

	list< Alternative* > fixed_opt_set(OPT_Solution.begin(),OPT_Solution.end());
	for(list< Alternative* >::iterator el1 = fixed_opt_set.begin(); el1 != fixed_opt_set.end(); ++el1){
		for(list< Alternative* >::iterator el2 = fixed_opt_set.begin(); el2 != fixed_opt_set.end(); ++el2){
			if((*el1)->get_id() == (*el2)->get_id())
				continue;
			if( (*el1)->dominates(*el2) == 1)
				OPT_Solution.erase(el2);
			else if((*el1)->dominates(*el2) == -1)
				OPT_Solution.erase(el1);

		}
	}
}


list< Alternative * > MainKnapsack::MOLS(double starting_time_sec){

	Alternative* alt;
	list< Alternative* > Local_front(0);

	int nb_iteration=0;

	//First initialization
	for(list< Alternative* >::iterator p = Population.begin(); p != Population.end(); ++p)
		Update_Archive(*p,OPT_Solution);


	while((Population.size() > 0)  and ((clock() /CLOCKS_PER_SEC) - starting_time_sec <= 180 ) ){
		nb_iteration++;
		//get first element
		alt = Population.front();
		save_new_point(filename_instance+".expl",alt);

		vector<Alternative *> current_neighbors = alt->get_neighborhood();

		for(vector< Alternative* >::iterator neighbor = current_neighbors.begin(); neighbor != current_neighbors.end(); ++neighbor){
			//Prefiltrage
			if( alt->dominates(*neighbor) != 1 )
				Update_Archive(*neighbor,Local_front);
		}

		for(list< Alternative* >::iterator new_alt = Local_front.begin(); new_alt != Local_front.end(); ++new_alt){
			//Filter OPT_Solution set
			if ( Update_Archive(*new_alt, OPT_Solution) )
				Population.push_back(*new_alt);
		}

		//remove first element
		Population.pop_front();
		Local_front.clear();

	}

	cout<<"Number of iteration "<<nb_iteration<<endl;

	filter_efficient_set();

	write_solution(filename_instance+".sol");

	return OPT_Solution;
}




list< Alternative * > MainKnapsack::MOLS(double starting_time_sec,int steps){

	Alternative* alt;
	list< Alternative* > Local_front(0);

	int nb_iteration=0;

	//First initialization
	for(list< Alternative* >::iterator p = Population.begin(); p != Population.end(); ++p)
		Update_Archive(*p,OPT_Solution);


	while((Population.size() > 0)  and ((clock() /CLOCKS_PER_SEC) - starting_time_sec <= 180 )  and nb_iteration < steps){
		nb_iteration++;
		//get first element
		alt = Population.front();
		save_new_point(filename_instance+".expl",alt);

		vector<Alternative *> current_neighbors = alt->get_neighborhood();

		for(vector< Alternative* >::iterator neighbor = current_neighbors.begin(); neighbor != current_neighbors.end(); ++neighbor){
			//Prefiltrage
			if( alt->dominates(*neighbor) != 1 )
				Update_Archive(*neighbor,Local_front);
		}

		for(list< Alternative* >::iterator new_alt = Local_front.begin(); new_alt != Local_front.end(); ++new_alt){
			//Filter OPT_Solution set
			if ( Update_Archive(*new_alt, OPT_Solution) or  (rand()*1.0/RAND_MAX) < 0.5 )
				Population.push_back(*new_alt);
		}

		//remove first element
		Population.pop_front();
		Local_front.clear();

	}

	cout<<"Number of iteration "<<nb_iteration<<endl;

	filter_efficient_set();

	write_solution(filename_instance+".sol");

	return OPT_Solution;
}



void MainKnapsack::HYBRID_WS_PLS(double starting_time_sec,int steps){

	//WS
	MOLS(starting_time_sec,steps);

	Population = OPT_Solution;

	change_to_pareto_selection();
	//PLS
	MOLS(starting_time_sec);

}


bool MainKnapsack::Update_Archive(Alternative* p, list< Alternative* > &set_SOL){

	vector< Alternative* > to_remove;
	int dom_val;

	for(list< Alternative* >::iterator alt = set_SOL.begin(); alt != set_SOL.end(); ++alt){

		dom_val = (*alt)->dominates(p);
		if(dom_val == 1)			// alt dominates p
			return false;

		else if( dom_val == -1 )   // p dominates alt
			to_remove.push_back(*alt);
	}

	for(vector< Alternative* >::iterator rm = to_remove.begin(); rm != to_remove.end(); ++rm)
		set_SOL.remove(*rm);

	set_SOL.push_back(p);
	return true;
}








/**
 * ************************************************* EVALUATION PART *************************************************
 */

//Get minimum gap from OPT_Alternative and save the objective values in vect_criteria
float MainKnapsack::nearest_alternative(string filename, vector<float > weight_DM,  vector< float > & vect_criteria ){

	ifstream fic(filename.c_str());

	vector< float > criteria_val(p_criteria,0);
	float min_ratio = -1, tmp_ratio = 1;
	string line;

	while(!fic.eof()){

		getline(fic,line);

		if(line.size() == 0)
			continue;

		vector< float >tmp_criteria_values = Tools::decompose_line_to_float_vector(line);

		tmp_ratio = Tools::get_ratio(tmp_criteria_values, OPT_Alternative->get_criteria_values(), weight_DM);

		if( (min_ratio == -1) or (tmp_ratio < min_ratio) ){
			min_ratio = tmp_ratio;
			criteria_val = tmp_criteria_values;
		}

		if( min_ratio == 0 )  // equal to DMs preferences
			break;
	}

	fic.close();
	vect_criteria = criteria_val;
	return min_ratio;

}

//Get optima values of objective with WS aggregator
void MainKnapsack::solve_plne_ws_function(vector<float> weighted_sum){

	IloEnv   env;
	IloModel model(env);
	vector<IloNumVar > x(n_items);
	IloRangeArray Constraints(env);

	//VARIABLES
	for(int i = 0; i < n_items; i++){
		x[i] = IloNumVar(env, 0.0, 1.0, ILOINT);
		ostringstream varname;
		varname.str("");
		varname<<"x_"<<i;
		x[i].setName(varname.str().c_str());
	}

	//CONSTRAINTS
	IloExpr c1(env);

	for(int j = 0; j < n_items ; j++)
		c1 += x[j] * get_weight_of(j);

	Constraints.add(c1 <= Backpack_capacity );

	model.add(Constraints);

	//OBJECTIVE
	IloObjective obj=IloAdd(model, IloMaximize(env));

	for(int i = 0; i < n_items; i++){
		float coeff = 0.;
		for(int j = 0; j < p_criteria ; j++)
			  coeff += get_utility(i,j) * weighted_sum[j];

		obj.setLinearCoef(x[i], coeff);
	}

//	cout<<obj<<endl;

	//SOLVE
	IloCplex cplex(model);

	cplex.setOut(env.getNullStream());

	if ( !cplex.solve() ) {
		 env.error() << "Failed to optimize LP" << endl;
		 exit(1);
	}

	//GET SOLUTION
	set< int > items;
	for(int i = 0; i < n_items; i++){
		if( cplex.getValue(x[i]) > 0)
			items.insert(i);
	}
	env.end();

	OPT_Alternative = new AlternativeKnapsack(items, this);

}


//Evaluation the quality of PLS and WS-PLS solutions to DMs real preferences
void MainKnapsack::evaluate_solutions(string weighted_DM_preferences,float time){

	ifstream fic_read(weighted_DM_preferences.c_str());
	string line;
	int i = 0;
	vector<float> vector_criteria;
	vector<float > weight_DM(p_criteria,0);


	if (!(fic_read) or weighted_DM_preferences.find(".ks") == std::string::npos){
		cerr<<"Error occurred eval_sol"<<endl;
	}
	//read WS_DMs_preference

	getline(fic_read,line);

	weight_DM = Tools::decompose_line_to_float_vector(line);

	cout<<"----------------------- EVALUATION ----------------------"<<endl;
	//set best alternative OPT_Alternative according to WS_DM
	solve_plne_ws_function(weight_DM);

	cout<<"Preference du décideur ([ " << Tools::print_vector(weight_DM) <<" ]) : "<<endl;
	cout<<"    "<< Tools::print_vector(OPT_Alternative->get_criteria_values())<<endl;

	//Get minimum objective values difference between the best alternative and WS-MOLS front computed
	float min_mols_ratio = nearest_alternative(filename_instance+".sol", weight_DM, vector_criteria);
	cout<<"Solution found in (efficient) front "<<endl;
	cout<<"   ratio ( "<<min_mols_ratio<<" )"<<endl;
	cout<<"   vector objective ( "<<Tools::print_vector(vector_criteria)<<" )"<<endl;


//	write evaluation
	ofstream fic("./Data/DistTime/"+type_instance+"/I"+num_instance+"_"+to_string(n_items)+".eval", ios::app);
	fic<<min_mols_ratio<<","<<time<<endl;

	Tools::update_dist_time(min_mols_ratio,time);
	cout<<"----------------------- END EVALUATION ----------------------"<<endl<<endl;

	fic.close();
}












/**
 * ************************************************* PF MEASUREMENT PART *************************************************
 */

float MainKnapsack::PR_D3(){

	int opt_size_front = 0, nb_found = 0;

	vector< Alternative* > tmp_opt(OPT_Solution.begin(),OPT_Solution.end());
	for(vector< vector<float > >::iterator pareto_sol = ParetoFront.begin(); pareto_sol != ParetoFront.end(); ++pareto_sol){
		for(vector< Alternative* >::iterator alt = tmp_opt.begin(); alt != tmp_opt.end(); ++alt){
			if( Tools::equal_vectors((*alt)->get_criteria_values(),*pareto_sol) ){
				nb_found += 1;
				tmp_opt.erase(alt);
				break;
			}
		}
		opt_size_front++;
	}

	return nb_found*100.0/opt_size_front;
}

float MainKnapsack::average_distance_D1(){

	float min_dist = -1;
	float avg_dist = 0.;
	for(vector< vector<float > >::iterator pareto_sol = ParetoFront.begin(); pareto_sol != ParetoFront.end(); ++pareto_sol){
		min_dist = -1;
		for(list<Alternative*>::iterator eff_sol = OPT_Solution.begin(); eff_sol != OPT_Solution.end(); ++eff_sol){
			float euclid_dist_tmp = Tools::euclidian_distance((*eff_sol)->get_criteria_values(), *pareto_sol);

			if(euclid_dist_tmp < min_dist  or min_dist == -1)
				min_dist = euclid_dist_tmp;
		}

		avg_dist += min_dist;
	}

	return avg_dist/ ParetoFront.size();
}

float MainKnapsack::maximum_distance_D2(){
	float min_dist = -1;
	float max_dist_PF = 0.;

	for(vector< vector<float > >::iterator pareto_sol = ParetoFront.begin(); pareto_sol != ParetoFront.end(); ++pareto_sol){
		min_dist = -1;
		for(list<Alternative*>::iterator eff_sol = OPT_Solution.begin(); eff_sol != OPT_Solution.end(); ++eff_sol){
			float euclid_dist_tmp = Tools::euclidian_distance((*eff_sol)->get_criteria_values(), *pareto_sol);
			if(euclid_dist_tmp < min_dist  or min_dist == -1)
				min_dist = euclid_dist_tmp;
		}

		if( max_dist_PF < min_dist)
			max_dist_PF = min_dist;
	}

	return max_dist_PF;
}


void MainKnapsack::pareto_front_evaluation(){

	float D1 = average_distance_D1();
	float D2 = maximum_distance_D2();
	float D3 = PR_D3();

	Tools::update_indicators(D1,D2,D3);

	ofstream write_fic("./Data/ParetoFront/"+type_instance+"/I"+num_instance+"_"+to_string(n_items)+".front", ios::app);

	write_fic<<D1<<","<<D2<<","<<D3<<endl;

	write_fic.close();

}






