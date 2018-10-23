#ifndef STATE_H
#define STATE_H

class State{
public:
	//typeState, 0 << hard, 1 << soft, 2 << pair, 3 << busted, 4 << blackjack
	int typeState;
	//value << hard value for hard, x for soft (Ace + x), p for pair (p,p) : 1 for Ace, 10 for Face Card
	int value;
	//dealer's face up card
	int dealer;
	//if it is the start state or not, 0 << initial, 1 << not initial
	int start;

	//constructor
	State(int typeState, int value, int dealer, int start){
		this->typeState = typeState;
		this->value = value;
		this->dealer = dealer;
		this->start = start;
	}
};

#endif