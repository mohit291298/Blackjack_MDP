#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <ctime>
#include <stdlib.h>
#include "./state.h"

using namespace std;
typedef int Action;		//0 << hit, 1 << split, 2 << stand, 3 << double
#define NUM_STATES 731
#define EPSILON 0.0000001

//global variables

double P;
double P_face, P_non_face;
//const int NUM_STATES = 722;
double VALUES[NUM_STATES];
Action OPT_ACTIONS[NUM_STATES];
double ways_to_sum[22];
double ways_to_sum_fixedA[22];

double P_SUM[7][10];

//function declaration
double P_hard_sum(int sum, int initial);
double P_soft_sum(int sum, int initial);

//function to convert state to int for indexing into array
int state_to_int(State s){
	//((17 + 9 + 10)*10)*2 + 10 + 1
	int start_offset = s.start * 360;
	int dealer_offset = (s.dealer-1) * 36;
	int value_offset;
	if(s.typeState == 3){
		return 730;
	}
	if(s.typeState == 4){
		return 719 + s.dealer;
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
	if(s == 730){
		st.typeState = 3;
		return st;
	}
	if(s < 730 && s > 719){
		st.typeState = 4;
		st.dealer = s-719;
		return st;
	}
	st.start = s/360;
	s = s%360;
	st.dealer = s/36 + 1;
	s = s%36;
	if(s <= 16){
		st.typeState = 0;
		st.value = s+5;
	}
	else if(s <= 25){
		st.typeState = 1;
		st.value = s-15;
	}
	else{
		st.typeState = 2;
		st.value = s-25;
	}
	return st;
}

//function to convert action to string for output
string action_to_string(Action a){
	if (a == 0)
		return "H";
	else if (a == 1)
		return "P";
	else if (a == 2)
		return "S";
	else
		return "D";
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

double get_val_hand(State s){
	switch(s.typeState){
		case 0: return s.value;
		case 1: return s.value + 11;
		case 2: return 2*(s.value);
		case 3: cout << "Error : get_val_hand called on busted"; break;
		case 4: cout << "Error : get_val_hand called on blackjack"; break;
		default: cout << "Error get_val_hand"; break;
	}
}

double P_hard_sum(int sum, int initial){
	double pr = 0.0;
	int diff = sum - initial;
	if(initial >= 17 && diff != 0){
		return 0;
	}
	if(diff < 0)
		return 0;
	if(diff == 0)
		return 1;
	for(int i = 2; i <= 9; i++){
		pr += P_non_face*P_hard_sum(sum, initial+i);
	}
	//for ace
	if(initial + 11 <= 21){
		pr += P_non_face*P_soft_sum(sum, initial+11);
	}
	else{
		pr += P_non_face*P_hard_sum(sum, initial+1);
	}
	//for face card
	pr += P_face*P_hard_sum(sum, initial+10);
	return pr;
}

double P_soft_sum(int sum, int initial){
	double pr = 0.0;
	int diff = sum - initial;
	if(initial >= 17 && diff != 0){
		return 0;
	}
	if(diff < 0)
		return 0;
	if(diff == 0)
		return 1;
	for(int i = 1; i <= 9; i++){
		if(initial + i <= 21)
			pr += P_non_face*P_soft_sum(sum, initial+i);
		else
			pr += P_non_face*P_hard_sum(sum, initial-11+1+i);
	}
	//for face card
	if(initial + 10 <= 21)
		pr += P_face*P_soft_sum(sum, initial+10);
	else{
		pr += P_face*P_hard_sum(sum, initial-11+1+10);
	}
	return pr;
}

//sum = 22 is sum to blackjack, 23 is busted 
double P_sum_to(int sum, int card){
	if(sum == 23){
		return 1-(P_sum_to(17, card) + P_sum_to(18, card) + P_sum_to(19, card) + P_sum_to(20, card) + P_sum_to(21, card) + P_sum_to(22, card));
	}
	if(sum == 22){
		if(card == 1){
			return P_face;
		}
		else if(card == 10){
			return P_non_face;
		}
		else{
			return 0.0;
		}
	}
	if(P_SUM[sum-17][card-1] != -1){
		return P_SUM[sum-17][card-1];
	}
	double pr = 0.0;
	if(card == 1){
		if(sum != 21)
			pr = P_soft_sum(sum, 11);
		else{
			for(int i = 1; i <= 9; i++){
				pr += P_non_face*P_soft_sum(sum, 11 + i);
			}
		}
	}
	else if(card == 10){
		if(sum != 21)
			pr = P_hard_sum(sum, 10);
		else{
			for(int i = 2; i <= 6; i++){
				pr += P_non_face*P_hard_sum(sum, 10+i);
			}
		}	
	}
	else{
		pr = P_hard_sum(sum, card);
	}
	P_SUM[sum-17][card-1] = pr;
	return pr;
}

//reward function for taking stand or double on a given state
double reward_new(State s, Action a){
	if(s.typeState == 3){
		//cout << "Error reward_new called on busted";
		return -1.0;
	}
	if(s.typeState == 4){
		return 1.5*(1 - P_sum_to(22, s.dealer));
	}
	double rew = 0.0;
	double val_hand = get_val_hand(s);
	if(val_hand > 21){
		cout << "Error reward_new value of hand > 21";
		return 0.0;
	}
	State final;
	switch(a){
		case 2:
			rew += P_sum_to(23, s.dealer)*1;
			rew += P_sum_to(22, s.dealer)*-1;
			if(val_hand >= 17){
				for(int i = 17; i < val_hand; i++){
					rew += P_sum_to(i, s.dealer)*1;
				}
				for(int i = val_hand+1; i <= 21; i++){
					rew += P_sum_to(i, s.dealer)*-1;
				}
			}
			else{
				for(int i = 17; i <= 21; i++){
					rew += P_sum_to(i, s.dealer)*-1;
				}
			}
			return rew;
		case 3:
			for(int card = 1; card < 10; card++){
				final = update_state_hit(s, card);
				rew += 2*P_non_face*reward_new(final, 2);
			}
			final = update_state_hit(s, 10);
			rew += 2*P_face*reward_new(final, 2);
			return rew;
		default: cout << "Error : reward new"; break;
	}
	return rew;
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
	double sum = 0.0;
	for(int card1 = 1; card1 < 10; card1++){
		State final1 = update_state_split(s, card1);
		int s1 = state_to_int(final1);
		for(int card2 = 1; card2 < 10; card2++){
			State final2 = update_state_split(s, card2);
			int s2 = state_to_int(final2);
			sum += P_non_face*P_non_face*(VALUES[s1] + VALUES[s2]);
		}
		State final2 = update_state_split(s, 10);
		int s2 = state_to_int(final2);
		sum += P_non_face*P_face*(VALUES[s1] + VALUES[s2]);
	}
	State final1 = update_state_split(s, 10);
	int s1 = state_to_int(final1);		
	for(int card2 = 1; card2 < 10; card2++){
		State final2 = update_state_split(s, card2);
		int s2 = state_to_int(final2);
		sum += P_face*P_non_face*(VALUES[s1] + VALUES[s2]);
	}
	State final2 = update_state_split(s, 10);
	int s2 = state_to_int(final2);
	sum += P_face*P_face*(VALUES[s1] + VALUES[s2]);
	return sum;
}

double val_hit(State s){
	double sum = 0.0;
	for(int card = 1; card < 10; card++){
		State final = update_state_hit(s, card);
		int s_int = state_to_int(final);
		sum += P_non_face*VALUES[s_int];
	}
	State final = update_state_hit(s, 10);
	int s_int = state_to_int(final);
	sum += P_face*VALUES[s_int];
	return sum;
}

double reward_split_aces(State initial){
	double rew = 0.0;
	for(int card1 = 1; card1 <= 9; card1++){
		State final1 = update_state_split(initial, card1);
		for(int card2 = 1; card2 <= 9; card2++){
			State final2 = update_state_split(initial, card2);
			rew += P_non_face*P_non_face*(reward_new(final1, 2) + reward_new(final2, 2));
		}
		State final2 = update_state_split(initial, 10);
		rew += P_non_face*P_face*(reward_new(final1, 2) + reward_new(final2, 2));
	}
	State final1 = update_state_split(initial, 10);
	for(int card2 = 1; card2 <= 9; card2++){
		State final2 = update_state_split(initial, card2);
		rew += P_face*P_non_face*(reward_new(final1, 2) + reward_new(final2, 2));
	}
	State final2 = update_state_split(initial, 10);
	rew += P_face*P_face*(reward_new(final1, 2) + reward_new(final2, 2));
	return rew;
}

//returns V'(s) using previousy computed state values
double bellmanBackup(State s){
	double max_val = -100.0;
	double val_split_val = -100.0;
	double val_hit_val = -100.0;
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
	if(s.typeState == 2 && s.start == 0){
		val_split_val = val_split(s);
	}

	/* spltting aces handled seperately */
	if(s.typeState == 2 && s.start == 0 && s.value == 1){
		val_split_val = reward_split_aces(s);
	}

	/* calculating V_n(s) for hit */
	val_hit_val = val_hit(s);
	
	/* calculating V_n(s) for stand */
	val_stand = reward_new(s, 2);

	/* calculating V_n(s) for double */	
	if(s.start == 0){
		val_double = reward_new(s, 3);
	}

	max_val = max(max_val, val_split_val);
	max_val = max(max_val, val_hit_val);
	max_val = max(max_val, val_stand);
	max_val = max(max_val, val_double);

	return max_val;
}

//function to set the opimum action in each state
Action setPolicy(State s){
	double max_val = -100.0;
	double val_split_val = -100.0;
	double val_hit_val = -100.0;
	double val_stand = -100.0;
	double val_double = -100.0;

	if(s.typeState == 3){
		cout << "Error : setPolicy called on busted";
		return 2;
	}

	if(s.typeState == 4){
		cout << "Error : setPolicy called on blackjack";
		return 2;
	}

	/* calculating V_n(s) for split */
	if(s.typeState == 2 && s.start == 0){
		val_split_val = val_split(s);
	}

	/* spltting aces handled seperately */
	if(s.typeState == 2 && s.start == 0 && s.value == 1){
		val_split_val = reward_split_aces(s);
	}

	/* calculating V_n(s) for hit */
	val_hit_val = val_hit(s);
	
	/* calculating V_n(s) for stand */
	val_stand = reward_new(s, 2);

	/* calculating V_n(s) for double */	
	if(s.start == 0){
		val_double = reward_new(s, 3);
	}

	max_val = max(max_val, val_split_val);
	max_val = max(max_val, val_hit_val);
	max_val = max(max_val, val_stand);
	max_val = max(max_val, val_double);

	if(s.typeState == 2 && s.value == 1 && s.dealer == 1 && s.start == 0){
		cout << "AA, A\n";
		cout << "split : " << val_split_val << "\n";
		cout << "hit : " << val_hit_val << "\n";
		cout << "stand : " << val_stand << "\n";
		cout << "double : " << val_double << "\n";
	}

	if((max_val == val_split_val) || (std::abs(max_val- val_split_val)<std::abs(std::min(max_val,val_split_val))*std::numeric_limits<double>::epsilon()))
		return 1;
	else if((max_val == val_hit_val) || (std::abs(max_val- val_hit_val)<std::abs(std::min(max_val,val_hit_val))*std::numeric_limits<double>::epsilon()))
		return 0;
	else if((max_val == val_stand) || (std::abs(max_val- val_stand)<std::abs(std::min(max_val,val_stand))*std::numeric_limits<double>::epsilon()))
		return 2;
	else
		return 3;
} 

//function to set OPT_ACTIONS for each state
void valueIteration(){
	double updated_values[NUM_STATES];
	double max_diff;

	//initializing
	for(int i = 0; i < NUM_STATES; i++){
		VALUES[i] = 0.0;
	}
	State s0;
	s0.typeState = 4;
	for(int i = 720; i <= 729; i++){
		s0.dealer = i-719;
		VALUES[i] = reward_new(s0, 0);
	}
	VALUES[730] = -1.0;

	cout << "\nexitted";
	//value iteration
	int iter = 0;
	while(true){
		iter++;
		cout << endl << iter;
		max_diff = 0.0;
		for(int s = 0; s < NUM_STATES-11; s++){
			State initial = int_to_state(s);
			// cout<<initial.dealer<<"\n";
			updated_values[s] = bellmanBackup(initial);
			max_diff = max(max_diff, fabs(updated_values[s] - VALUES[s]));
		}
		// cout<<"OUTTTTTT\n";
		for(int s = 0; s < NUM_STATES-11; s++){
			VALUES[s] = updated_values[s];
		}
		if(max_diff <= EPSILON){
			break;
		}
	}

	cout << "\nexitted";

	for(int s = 0; s < NUM_STATES-11; s++){
		State initial = int_to_state(s);
		OPT_ACTIONS[s] = setPolicy(initial);	
	}

	cout << "\nexitted";
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
	int val = 1;
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
	P = atof(argv[1]);
	P_face = P;
	P_non_face = (1-P)/9;

	for(int i = 0; i < 7; i++){
		for(int j = 0; j < 10; j++){
			P_SUM[i][j] = -1.0;
		}
	}

/*	for(int val = 17; val <= 23; val++){
		for(int d = 1; d <= 10; d++){
			cout << P_sum_to(val, d) << " ";
		}
		cout << "\n";
	}*/
	// for(int i=0;i<=722;i++){
	// 	State s = int_to_state(i);
	// 	if(i != (state_to_int(s))){
	// 		cout<<i<<" "<<state_to_int(s)<<" false\n";
	// 	}
	// }

	valueIteration();
	output();

	for(int i =0; i < 7; i++){
		for(int j = 0; j < 10; j++){
			cout << P_SUM[i][j] << " ";
		}
		cout << "\n";
	}

	return 0;
}