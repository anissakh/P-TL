
#ifndef ____ALTERNATIVE___
#define ____ALTERNATIVE___



#include <vector>
#include <set>


using namespace std;


class Alternative {

protected:
	int id_alt;															//unique ID
	vector< int > alternatives;											//vector 1/0 of present items (=1) or absent (=0)
	vector< float > criteria_values;									//vector of criteria value
	vector< float > objective_values;									// objectives values
	vector< Alternative* > neighborhood;								//Neighborhood set
	vector< vector< float > > local_WS_matrix;							//local direction of optimization
	//	LSStructure * mainLSStructure;
public:
	static int id;

	//GETTERS
	int get_id(){ return id_alt; };
	vector< float > get_objective_values(){ return objective_values; };
	vector< float > get_criteria_values() { return criteria_values; };
	inline void set_local_WS_matrix(vector<vector<float > > matrix){ local_WS_matrix = matrix; };

	//ABSTRACT METHOD
	virtual vector< Alternative* > get_neighborhood() = 0;
	virtual int dominates(Alternative* alt) = 0;
	virtual void update() = 0;

//	virtual void print_alternative() = 0;
//	virtual void print_objective_values() = 0;



};





#endif
