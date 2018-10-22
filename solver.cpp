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
double ways_to_sum[22];
double ways_to_sum_fixedA[22];

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
		    		if (diff <= 0){
		    			return 0.0;
		    		}
		    		if(diff == 10){
		    			return P_face;
		    		}
		    		return P_non_face;
		    	}
		    	if(final.typeState == 1){
		    		if(final.value == initial.value && (final.value + 11 <= 21)){
		    			return P_non_face;
		    		}
		    		return 0.0;
		    	}
		    	if(final.typeState == 3){
		    		if(initial.value <= 11){
		    			return 0.0;
		    		}
		    		return P_face + (initial.value - 12)*P_non_face;
		    	}
		        break;
		    case 1: // when original is a soft hand;
		    	if(final.typeState == 0){
		    		int diff = final.value - (initial.value + 1);
		    		if(initial.value + 11 + diff <= 21 || diff <= 0 || diff > 10){
		    			return 0.0;
		    		}
		    		if(diff == 10){
		    			return P_face;
		    		}
		    		return P_non_face;
		    	}
		    	if(final.typeState == 1){
		    		int diff = final.value - initial.value;
		    		if(final.value > 10 || diff <= 0){
		    			return 0.0;
		    		}
		    		if(diff == 10){
		    			return P_face;
		    		}
		    		return P_non_face;
		    	}
		    	if(final.typeState == 3){
		    		return 0.0;
		    	}
		        break;
		    case 2: // when original hand is a pair;
		        if(final.typeState == 0){
		    		if(initial.value != 1){
		    			int diff = final.value - (2*initial.value);
		    			if(diff < 0 || diff > 10){
		    				return 0.0;
		    			}
		    			if(diff == 10){
		    				return P_face;
		    			}
		    			return P_non_face;
		    		}
		    		else{
		    			if(final.value == 12){
		    				return P_face;
		    			}
		    			return 0.0;
		    		}	
		    	}
		    	if(final.typeState == 1){
		    		if(initial.value != 1){
		    			if(initial.value > 5 || (final.value) != (2*initial.value)){
		    				return 0.0;
		    			}
		    			return P_non_face;
		    		}
		    		else{
		    			int diff = final.value - 1;
		    			if(diff < 0 || diff > 10){
		    				return 0.0;
		    			}
		    			if(diff == 10){
		    				return P_face;
		    			}
		    			return P_non_face;
		    		}
		    	}
		    	if(final.typeState == 3){
		    		int diff;
		    		if(initial.value == 1){

		    		}
		    		else{
		    			diff = 21 - (2*initial.value);
		    			if(diff >= 10){
		    				return 0.0;
		    			}
		    			return P_face + (9-diff)*P_non_face;
		    		}
		    	}
		        break;
		    default: // code to be executed if n doesn't match any cases
		}
	}
}

double calc_ways_to_sum(){
	ways_to_sum[0] = 0;
	//A
	ways_to_sum[1] = P_non_face;
	//AA and 2
	ways_to_sum[2] = ways_to_sum[1]*P_non_face + P_non_face;

	for(int i = 3;i<=21;i++){
		// 3 to 9 sum cases
		if(i<=9){
			for(int j=i-1;j>0;j--){
				// getting j valued sum and then getting (i-j) valued card(always non_face till i = 9)
				ways_to_sum[i] += ways_to_sum[j]*P_non_face;
			}
			// getting i valued card only(Non face)
			ways_to_sum[i] += P_non_face;
		}
		else if(i==10){
			for(int j=i-1;j>0;j--){
				// getting j valued sum and then getting (i-j) valued card(always non_face for i = 10)
				ways_to_sum[i] += ways_to_sum[j]*P_non_face;
			}
			// getting 10 valued card only
			ways_to_sum[i] += P_face;
		}
		else if(i==11){
			for(int j=i-1;j>(i-10);j--){
				// getting j valued sum and then getting (i-j) valued card(non_face for j>(i-10))
				ways_to_sum[i] += ways_to_sum[j]*P_non_face;
			}
				// getting i-10 valued sum and then getting 10 valued card(face)
			ways_to_sum[i] += ways_to_sum[i-10]*P_face;
			// getting 11 valued card only
			ways_to_sum[i] += P_non_face;
		}
		else{
			for(int j=i-1;j>(i-10);j--){
				// getting j valued sum and then getting (i-j) valued card(non_face for j>(i-10))
				ways_to_sum[i] += ways_to_sum[j]*P_non_face;
			}		
				// getting i-10 valued sum and then getting 10 valued card(face)
			ways_to_sum[i] += ways_to_sum[i-10]*P_face;
				// getting i-11 valued sum and then getting 11 valued card(non_face)			
			ways_to_sum[i] += ways_to_sum[i-11]*P_non_face;
		}
	}
}

double calc_ways_to_sum_fixedA(){
	ways_to_sum_fixedA[0] = 0;
	//A
	ways_to_sum_fixedA[1] = 1;
	//AA and 2
	ways_to_sum_fixedA[2] = ways_to_sum_fixedA[1]*P_non_face;

	for(int i = 3;i<=21;i++){
		// 3 to 9 sum cases
		if(i<=9){
			for(int j=i-1;j>0;j--){
				// getting j valued sum and then getting (i-j) valued card(always non_face till i = 9)
				ways_to_sum_fixedA[i] += ways_to_sum_fixedA[j]*P_non_face;
			}
			// // getting i valued card only(Non face)
			// ways_to_sum_fixedA[i] += P_non_face;
		}
		else if(i==10){
			for(int j=i-1;j>0;j--){
				// getting j valued sum and then getting (i-j) valued card(always non_face for i = 10)
				ways_to_sum_fixedA[i] += ways_to_sum_fixedA[j]*P_non_face;
			}
			// getting 10 valued card only
			// ways_to_sum_fixedA[i] += P_face;
		}
		else if(i==11){
			for(int j=i-1;j>(i-10);j--){
				// getting j valued sum and then getting (i-j) valued card(non_face for j>(i-10))
				ways_to_sum_fixedA[i] += ways_to_sum_fixedA[j]*P_non_face;
			}
				// getting i-10 valued sum and then getting 10 valued card(face)
			ways_to_sum_fixedA[i] += ways_to_sum_fixedA[i-10]*P_face;
			// getting 11 valued card only
			ways_to_sum_fixedA[i] += 1;
		}
		else{
			for(int j=i-1;j>(i-10);j--){
				// getting j valued sum and then getting (i-j) valued card(non_face for j>(i-10))
				ways_to_sum_fixedA[i] += ways_to_sum_fixedA[j]*P_non_face;
			}		
				// getting i-10 valued sum and then getting 10 valued card(face)
			ways_to_sum_fixedA[i] += ways_to_sum_fixedA[i-10]*P_face;
				// getting i-11 valued sum and then getting 11 valued card(non_face)			
			ways_to_sum_fixedA[i] += ways_to_sum_fixedA[i-11]*P_non_face;
		}
	}
}

double compute_reward(int start,int end){
	double final = 0.0;

	double temp = ways_to_sum[end];
	for(int j = end-1;j>=start;j--){
		temp = temp - ways_to_sum[j]*P_non_face;
	}
	final = temp;

	return final;	
}

double calc_non_bust(int start,int end){
	double final = 0.0;
	final += ways_to_sum[start];
	
	for(int i= start + 1;i<=end;i++){
		temp = ways_to_sum[i];
		for(int j = i-1;j>=start;j++){
			temp = temp - ways_to_sum[j]*P_non_face;
		}
		final += temp;
	}
	return final;	
}

double compute_reward_fixedA(int start,int end){
	double final = 0.0;

	double temp = ways_to_sum_fixedA[end];
	for(int j = end-1;j>=start;j--){
		temp = temp - ways_to_sum_fixedA[j]*P_non_face;
	}
	final = temp;

	return final;	
}

double calc_non_bust_fixedA(int start,int end){
	double final = 0.0;
	final += ways_to_sum_fixedA[start];
	
	for(int i= start + 1;i<=end;i++){
		temp = ways_to_sum_fixedA[i];
		for(int j = i-1;j>=start;j++){
			temp = temp - ways_to_sum_fixedA[j]*P_non_face;
		}
		final += temp;
	}
	return final;	
}

//reward function for taking stand or double on a given state
double reward(State s, Action a){
	double ret = 0.0;
	if(a==2){
		int act_val;
		int to_return = 0.0;
		int diff_up = 21 - s.dealer;
		int diff_down = 17 - s.dealer;
		if(s.typeState == 3){
			return -1.0;
			// cout<<"Error\n";
		}
		else if(s.typeState == 0){
			act_val = s.value;
		}
		else if(s.typeState == 1){
			act_val = 11 + s.value;
		}
		else{
			act_val = 2*s.value;
		}
		if(s.dealer !=1){
			if(act_val<17){
				//assuming dealer does not have A
				//The dealer has to just not get busted to win
				return -(2*calc_non_bust(diff_down,diff_up) -1);
			}
			else if(act_val<=21){
				// The dealer has to prevent busting and also to get more than us
				//More than us case
				diff_down = act_val - s.dealer + 1;
				for(int i=diff_down;i<=diff_up;i++){
					to_return = to_return - compute_reward(17-s.dealer,i);
				}
				//Less than us case
				for(int i = 17-s.dealer;i<=(act_val - s.dealer - 1);i++){
					to_return = to_return + compute_reward(17-s.dealer,i)
				}
				//Busted also added
				return to_return + (1- calc_non_bust(17 - s.dealer,diff_up));
			}
		}
		else{
			if(act_val<17){
				//assuming dealer has A
				//The dealer has to just not get busted to win
				return -(2*calc_non_bust_fixedA(17,21) -1);
			}
			else if(act_val<=21){
				// The dealer has to prevent busting and also to get more than us
				//More than us case
				diff_down = act_val + 1;
				diff_up = 21;
				for(int i=diff_down;i<=diff_up;i++){
					to_return = to_return - compute_reward_fixedA(17,i);
				}
				//Less than us case
				for(int i = 17;i<=(act_val - 1);i++){
					to_return = to_return + compute_reward_fixedA(17,i)
				}
				//Busted also added
				return to_return + (1- calc_non_bust_fixedA(17,diff_up));
			}
		}
	}
	else if(a == 3){
		for(int state = 0; state < NUM_STATES; state++){
			State s1 = int_to_state(state);

			ret += transition(s, 0, s1)*reward(s1,2);
		}
		return ret;		
	}	
}

//returns V'(s) using previousy computed state values
double bellmanBackup(State s){
	double max_val = -100.0;
	/* calculating V_n(s) for split */
	double val_split = -100.0;
	int sum_split 0.0;
	if(s.typeState == 2){
		//init_split = s.value/2;
		for(int state = 0; state < NUM_STATES; state++){
			State s1 = int_to_state(state);
			sum_split += transition(s, 1, s1)*VALUES[state];
		}
	val_split = sum_split*2 - 1;
	}

	/* calculating V_n(s) for hit */

	double val_hit;
	double sum = 0.0;
	for(int state = 0; state < NUM_STATES; state++){
		State s1 = int_to_state(state);
		sum += transition(s, 0, s1)*VALUES[state];
	}
	val_hit = sum;
	/* calculating V_n(s) for stand */
	double val_stand = reward(s, 2);
	/* calculating V_n(s) for double */	
	double val_double = 2*reward(s, 3);

	max_val = max(max_val, val_stand);
	max_val = max(max_val, val_double);
	max_val = max(max_val, val_split);
	max_val = max(max_val, val_hit);

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