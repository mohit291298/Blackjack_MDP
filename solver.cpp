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
#include "./action.h"

using namespace std;

//global variables
double P;
int NUM_STATES;
double VALUES[NUM_STATES];
Action OPT_ACTIONS[NUM_STATES];


//function to convert state to int for indexing into array
int state_to_int(State s){

}

//function to convert action to string for output
char* action_to_string(Action a){

}

//transition function, returns value in interval [0,1]
double transition(State initial, Action a, State final){

}

//reward function for taking stand or double on a given state
double reward(State s, Action a){

}

//returns V'(s) using previousy computed state values
double bellmanBackup(State s){

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