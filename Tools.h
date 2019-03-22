#ifndef __TOOLS__
#define __TOOLS__



#include <math.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <vector>
#include <time.h>
#include <ostream>
#include <fstream>


#pragma once


using namespace std;


class Tools{

public:

	static float euclidian_distance(vector<float> v1, vector<float> v2);
	static float get_ratio(vector<float> v, vector<float> v_opt, vector<float> weights);
	static bool equal_vectors(vector<float> v1, vector<float> v2);
	static vector< float > decompose_line_to_float_vector(string line);
	static string print_vector(vector<float> v);
	static vector<float> generate_random_WS_aggregator(int n_w);
	static void generate_random_WS(string filename, int nb_criteria);
	static void compute_average_column_files(string filename, int nb_column);
	static void copy_into(string src_filename, string dest_filename);


};

#endif
