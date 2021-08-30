/* ****************************************************** *
 * AUTHOR:      Noah Krim
 * ASSIGNMENT:  Programming Project 01
 * CLASS:       CS_08
 * ------------------------------------------------------ *
 * FILE: history.h
 *  Header file for the change history of the game.
 *  Provides a memory-optimized solution to undoing changes
 *  performed by the user.
 * ****************************************************** */

#ifndef HISTORY_H
#define HISTORY_H

#include <vector>

class history_change {
private:
	unsigned row;
	unsigned col;
	char before;
public:
	history_change()
		:row(0),col(0),before(0) { }
	history_change(unsigned row, unsigned col, unsigned before)
		:row(row),col(col),before(before) { }
	unsigned get_row() const { return row; }
	unsigned get_col() const { return col; }
	char get_before() const { return before; }
};

class history_move {
private:
	std::vector<history_change> change_stack;
	int before_player_pos_h;
public:
	history_move(int before_player_pos_h);
	unsigned push_change(const history_change& hc);
	bool pop_change(history_change& hc);
	unsigned num_changes() const;
	unsigned get_player_pos_h() const;
};

class history_stack {
private:
	std::vector<history_move> move_stack;
public:
	// constructor
	history_stack();
	// swap
	friend void swap(history_stack& a, history_stack& b);
	// push to stack
	unsigned push_move(unsigned before_player_pos_h);
	unsigned push_change(unsigned row, unsigned col, char before);
	// pop from stack
	bool pop_change(history_change& hc);
	bool pop_move(unsigned& player_pos);
	// accessors
	unsigned num_moves() const;
	unsigned num_changes_in_move() const;
};

#endif