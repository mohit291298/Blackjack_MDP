#ifndef STATE_H
#define STATE_H

class State{
public:
	//typeState, 0 << hard, 1 << soft, 2 << pair, 3 << busted
	int typeState;
	//value << hard value for hard, x for soft (Ace + x), p for pair (p,p)
	int value;
	//dealer's card
	int dealer;
	//if it is the initial state or not, 0 << initial, 1 << not
	int initial;
};

#endif