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
double EPSILON;
int NUM_STATES = 722;
double VALUES[NUM_STATES];
Action OPT_ACTIONS[NUM_STATES];
double ways_to_sum[22];
double ways_to_sum_fixedA[22];

//function to convert state to int for indexing into array
int state_to_int(State s){
	//((17 + 9 + 10)*10)*2 + 1 + 1
	int start_offset = s.start * 360;
	int dealer_offset = (s.dealer-1) * 36;
	int value_offset;
	if(s.typeState == 3){
		return 720;
	}
	if(s.typeState == 4){
		return 721;
	}
	switch (s.typeState){
		case 0: value_offset = s.value - 5; break;
		case 1: value_offset = s.value + 15; break;
		case 2: value_offset = s.value + 25; break;
		case 3: value_offset = 0; break;
		case 4: value_offset = 0; break;
		default: cout << "Error state_to_int"; break;
	}
	return dealer_offset + value_offset + start_offset;
}

//function to convert integer to state
State int_to_state(int s){
	State st;
	if(s == 720){
		st.typeState = 3;
		return st;
	}
	if(s == 721){
		st.typeState = 4;
		return st;
	}
	st.start = s/360;
	s = s%360;
	st.dealer = s/36 + 1;
	s = s%36;
	if(s <= 16){
		st.typeState = 0;
		st.value = s;
	}
	else if(s <= 25){
		st.typeState = 1;
		st.value = s-15
	}
	else{
		st.typeState = 2;
		st.value = s-25;
	}
	return st;
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
		    			if(diff == 9){
		    				cout << "Error : pair to soft with ace";
		    			}
		    			if(diff <= 0 || diff >= 9){
		    				return 0.0;
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
		    	cout << "Error transition no case matched 1";
		    	break;
		}
	}
	if(a == 1){
		if(initial.typeState != 2 || (initial.value == 1 && final.start != 2) || (initial.value != 1 && final.start != 1)){
			return 0.0;
		}
		if(initial.value != 1){
			if(final.typeState == 1){
				int diff = final.value - initial.value;
				if(diff < 0 || diff > 10){
					return0.0;
				}
				if(diff == 10){
					return P_face;
				}
				return P_non_face;
			}
			if(final.typeState == 2){
				return;
			}
			if(final.typeState == 3){
				return;
			}
			if(final.typeState == 4){
				return;
			}
		}
		else{
			if(final.typeState == 1){
				int diff = final.value - initial.value;
				if(diff < 0 || diff > 10){
					return0.0;
				}
				if(diff == 10){
					return P_face;
				}
				return P_non_face;
			}
			if(final.typeState == 2){
				return;
			}
			if(final.typeState == 3){
				return;
			}
			if(final.typeState == 4){
				return;
			}
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
		for(int card = 1; card < 10; card++){
			State final = update_state_hit(s, card);
			ret += P_non_face*reward(final,2);
		}
		State final = update_state_hit(s, 10);
		ret += P_face*reward(final,2);	
		return ret;		
	}	
}

State update_state_hit(State initial, int card){
	State final;
	final.start = 1;
	final.dealer = initial.dealer;
	switch(initial.typeState){
		case 0 : 
			if (card != 1) {
				final.value = initial.value + card;
				final.typeState = 0;
			}
			else{
				if(initial.value + 11 <= 21){
					final.value = initial.value;
					final.typeState = 1;
				}
				else{
					final.value = initial.value + 1;
					final.typeState = 0;
				}
			}
			if(final.value > 21)
				final.typeState = 3;
			break;
		case 1 :
			final.value = initial.value + card;
			if(final.value + 11 > 21){
				final.value += 1;
				final.typeState = 0;
			}
			else{
				final.typeState = 1;
			}
			break;
		case 2 : 
			if(initial.value != 1 && card != 1){
				final.value = (2*initial.value) + card;				
				final.typeState = 0;
			}
			else if (initial.value != 1){
				if((2*initial.value) + 11 <= 21){
					final.value = (2*initial.value);
					final.typeState = 1;
				}
				else{
					final.value = (2*initial.value) + 1;
					final.typeState = 0;
				}
			}
			else if (card != 1){
				if(12 + card <= 21){
					final.value = 1 + card;
					final.typeState = 1;
				}
				else{
					final.value = 2 + card;
					final.typeState = 0;
				}
			}
			else{
				final.value = 2;
				final.typeState = 1;
			}
			if(final.value > 21){
				final.typeState = 3;
			}
			break;
		case 3 : cout << "Error update state hit called on busted "; final.typeState = 3; break;
		default : cout << "Error : update_state_hit"; break;
	}
	return final;
}

State update_state_split(State initial, int card_new){
	State final;
	if(initial.value != 1){
		final.start = 0;
	}
	else{
		final.start = 1;
	}
	final.dealer = initial.dealer;
	
	if (initial.typeState != 2){
		cout << "Error : update state split called on non pair state";
	}
	else{
		if(card_new == initial.value){
			final.value = initial.value;
			final.typeState = 2;
		}
		else{
			if(card_new != 1 && initial.value != 1){
				final.value = initial.value + card_new;
				final.typeState = 0;
			}
			else if(initial.value != 1){
				if(initial.value == 10){
					final.value = 21;
					final.typeState = 4;
				}
				else{
					final.value = initial.value;
					final.typeState = 1;	
				}
			}
			else if(card_new != 1){
				final.value = card_new;
				final.typeState = 1;
			}
			else{
				final.value = 1;
				final.typeState = 2;
			}
		}
	}
	return final;
}

double val_split(State s){
	int sum 0.0;
	for(int card1 = 1; card1 < 10; card1++){
		State final1 = update_state_hit(s, card1);
		int s1 = state_to_int(final1);
		for(int card2 = 0; card2 < 10; card2++){
			State final2 = update_state_hit(s, card2);
			int s2 = state_to_int(final2);
			sum += P_non_face*P_non_face*(VALUES[s1] + VALUES[s2]);
		}
		State final2 = update_state_hit(s, 10);
		int s2 = state_to_int(final2);
		sum += P_non_face*P_face*(VALUES[s1] + VALUES[s2]);
	}
	State final1 = update_state_hit(s, 10);
	int s1 = state_to_int(final1);		
	for(int card2 = 1; card2 < 10; card2++){
		State final2 = update_state_hit(s, card2);
		int s2 = state_to_int(final2);
		sum += P_face*P_non_face*(VALUES[s1] + VALUES[s2]);
	}
	State final2 = update_state_hit(s, 10);
	int s2 = state_to_int(final2);
	sum += P_face*P_face*(VALUES[s1] + VALUES[s2]);
	return sum;
}

double val_hit(State s){
	double sum = 0.0;
	for(int card = 0; card < 10; card++){
		State final = update_state_hit(initial, card);
		int s = state_to_int(final);
		sum += P_non_face*VALUES[s];
	}
	State final = update_state_hit(initial, 10);
	int s = state_to_int(final);
	sum += P_face*VALUES[s];
	return sum;
}

//returns V'(s) using previousy computed state values
double bellmanBackup(State s){
	double max_val = -100.0;
	double val_split = -100.0;
	double val_hit = -100.0;
	double val_stand = -100.0;
	double val_double = -100.0;

	if(s.typeState == 3){
		cout << "Error : bellmanBackup called on busted";
		return -1.0;
	}

	if(s.typeState == 4){
		cout << "Error : bellmanBackup called on blackjack";
		return 2.5;
	}

	/* calculating V_n(s) for split */
	if(s.typeState == 2 && s.start = 0){
		val_split = val_split(s);
	}

	/* calculating V_n(s) for hit */
	val_hit = val_hit(s);
	
	/* calculating V_n(s) for stand */
	val_stand = reward(s, 2);

	/* calculating V_n(s) for double */	
	if(s.start == 0){
		val_double = 2*reward(s, 3);
	}

	max_val = max(max_val, val_split);
	max_val = max(max_val, val_hit);
	max_val = max(max_val, val_stand);
	max_val = max(max_val, val_double);

	return max_val;
}

//function to set OPT_ACTIONS for each state
void valueIteration(){
	double updated_values[NUM_STATES];
	double max_diff;

	//initializing
	for(int i = 0; i < NUM_STATES; i++){
		VALUES[i] = 0.0;
	}
	VALUES[720] = -1.0;
	VALUES[721] = 2.5;

	//value iteration
	while(true){
		max_diff = 0.0;
		for(int s = 0; s < NUM_STATES-2; s++){
			State initial = int_to_state(s);
			updated_values[s] = bellmanBackup(initial);
			max_diff = max(max_diff, fabs(updated_values[s] - VALUES[s]));
		}
		for(int s = 0; s < NUM_STATES-2; s++){
			VALUES[s] = updated_values[s];
		}
		if(max_diff <= EPSILON){
			break;
		}
	}

	for(int s = 0; s < NUM_STATES-2; s++){
		State initial = int_to_state(s);
		OPT_ACTIONS[s] = setPolicy(initial);	
	}
}

//function to set the opimum action in each state
Action setPolicy(State s){
	double max_val = -100.0;
	double val_split = -100.0;
	double val_hit = -100.0;
	double val_stand = -100.0;
	double val_double = -100.0;

	if(s.typeState == 3){
		cout << "Error : bellmanBackup called on busted";
		return -1.0;
	}

	if(s.typeState == 4){
		cout << "Error : bellmanBackup called on blackjack";
		return 2.5;
	}

	/* calculating V_n(s) for split */
	if(s.typeState == 2 && s.start = 0;){
		val_split = val_split(s);
	}

	/* calculating V_n(s) for hit */
	val_hit = val_hit(s);
	
	/* calculating V_n(s) for stand */
	val_stand = reward(s, 2);

	/* calculating V_n(s) for double */	
	if(s.start == 0){
		val_double = 2*reward(s, 3);
	}

	max_val = max(max_val, val_split);
	max_val = max(max_val, val_hit);
	max_val = max(max_val, val_stand);
	max_val = max(max_val, val_double);

	if(max_val == val_split)
		return 1;
	else if(max_val == val_hit)
		return 0;
	else if(max_val == val_stand)
		return 2;
	else
		return 3;
} 

//function to output OPT_ACTIONS
void output(){
	ofstream fout;
	fout.open("Policy.txt", ios::out);
	
	for(int val = 5; val <= 19; val++){
		fout << val << "\t";
		for(int d = 2; d <= 10; d++){
			State s(0, val, d, 0);
			int state = state_to_int(s);
			fout << action_to_string(OPT_ACTIONS[state]) << " ";
		}
		State s(0, val, 1, 0);
		int state = state_to_int(s);
		fout << action_to_string(OPT_ACTIONS[state]) << "\n";
	}
	for(int val = 2; val <= 9; val++){
		fout << "A" << val << "\t";
		for(int d = 2; d <= 10; d++){
			State s(1, val, d, 0);
			int state = state_to_int(s);
			fout << action_to_string(OPT_ACTIONS[state]) << " ";
		}
		State s(1, val, 1, 0);
		int state = state_to_int(s);
		fout << action_to_string(OPT_ACTIONS[state]) << "\n";
	}
	for(int val = 2; val <= 10; val++){
		fout << val << val << "\t";
		for(int d = 2; d <= 10; d++){
			State s(2, val, d, 0);
			int state = state_to_int(s);
			fout << action_to_string(OPT_ACTIONS[state]) << " ";
		}
		State s(2, val, 1, 0);
		int state = state_to_int(s);
		fout << action_to_string(OPT_ACTIONS[state]) << "\n";
	}
	val = 1;
	fout << "AA" << "\t";
	for(int d = 2; d <= 10; d++){
		State s(2, val, d, 0);
		int state = state_to_int(s);
		fout << action_to_string(OPT_ACTIONS[state]) << " ";
	}
	State s(2, val, 1, 0);
	int state = state_to_int(s);
	fout << action_to_string(OPT_ACTIONS[state]);

	fout.close();
}

int main(int argc, char **argv){
	return 0;
}