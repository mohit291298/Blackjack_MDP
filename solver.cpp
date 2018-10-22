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
double P_face, P_non_face;
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
	if (a == 2 || a == 3){
		cout << "Error : transition";
		return 0.0;
	}
	if(initial.dealer != final.dealer || final.start == 0){
		return 0.0;
	}
	//for hit action
	if(a == 0){
		//after hit, start = 2 and final state cannot be a pair
		if(final.start != 2 || final.typeState == 2){
			return 0.0;
		}
		switch (initial.typeState)
		{
		    case 0: // when original is a hard hand;
		    	if(final.typeState == 0){
		    		int diff = final.value - initial.value;
		    	}
		        break;
		    case 2: // code to be executed if n = 2;
		        break;
		    default: // code to be executed if n doesn't match any cases
		}
	}
}

//reward function for taking stand or double on a given state
double reward(State s, Action a){
	
}

//returns V'(s) using previousy computed state values
double bellmanBackup(State s){
	double max_val = -100.0;
	/* calculating V_n(s) for split */
	double val_split;
	int sum_split 0.0;
	if(s.typeState == 2){
		//init_split = s.value/2;
		for(int state = 0; state < NUM_STATES; state++){
			State s1 = int_to_state(state);
			sum_split += transition(s, 1, s1)*VALUES[state];
		}
	}
	val_split = sum_split*2 - 1;
	/* calculating V_n(s) for hit */
	double val_hit;
	double sum = 0.0;
	for(int state = 0; state < NUM_STATES; state++){
		State s1 = int_to_state(state);
		sum += transition(s, 0, s1)*VALUES[state];
	}
	val_hit = sum;
	/* calculating V_n(s) for stand */
	double val_stand = reward(s, 3);
	/* calculating V_n(s) for double */	
	double val_double = reward(s, 4);

	max_val = max(max_val, val_stand);
	max_val = max(max_val, val_double);
	return max_val;
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