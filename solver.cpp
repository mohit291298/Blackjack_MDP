#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <ctime>
#include "./state.h"

using namespace std;
typedef int Action 		//0 << hit, 1 << split, 2 << stand, 3 << double

//global variables
double P;
int NUM_STATES;
double VALUES[NUM_STATES];
Action OPT_ACTIONS[NUM_STATES];


//function to convert state to int for indexing into array
int state_to_int(State s){

}

//function to convert integer to state
State int_to_state(int s){

}

//function to convert action to string for output
char* action_to_string(Action a){
	if (a == 0)
		return "H";
	else if (a == 1)
		return "SP";
	else if (a == 2)
		return "S";
	else
		return "D";
}

//transition function, returns value in interval [0,1]
double transition(State initial, Action a, State final){

}

//reward function for taking stand or double on a given state
double reward(State s, Action a){
	
}

//returns V'(s) using previousy computed state values
double bellmanBackup(State s){
	double max_val = -100.0;
	Action a;
	for(a = 0; a < 2; a++){
		double sum = 0.0;
		for(int state = 0; state < NUM_STATES; state++){
			State s1 = int_to_state(state);
			sum += transition(s, a, s1)*VALUES[state];
		}
		max_val = max(max_val, sum);
	}
	double val_stand = reward(s, 3);	
	double val_double = reward(s, 4);
	max_val = max(max_val, val_stand);
	max_val = max(max_val, val_double);
}

//function to set OPT_ACTIONS for each state
void valueIteration(){

}

//function to output OPT_ACTIONS
void output(){

}

int main(int argc, char **argv){
	return 0;
}