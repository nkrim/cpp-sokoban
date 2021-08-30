/* ****************************************************** *
 * AUTHOR:      Noah Krim
 * ASSIGNMENT:  Programming Project 01
 * CLASS:       CS_08
 * ------------------------------------------------------ *
 * FILE: history.cpp
 *  Definition file for the history classes.
 * ****************************************************** */

#include "history.h"

// history_move 
// ========================================================
history_move::history_move(int before_player_pos_h)
    : before_player_pos_h(before_player_pos_h)
{ }
unsigned history_move::push_change(const history_change& hc) {
    change_stack.push_back(hc);
    return change_stack.size();
}
bool history_move::pop_change(history_change& hc) {
    if(change_stack.size() == 0)
        return false;
    hc = change_stack.back();
    change_stack.pop_back();
    return true;
}
unsigned history_move::num_changes() const {
    return change_stack.size();
}
unsigned history_move::get_player_pos_h() const {
    return before_player_pos_h;
}


// history_stack
// ========================================================
// constructor
history_stack::history_stack()
{ }

// swap
void swap(history_stack& a, history_stack& b) {
    using std::swap;

    swap(a.move_stack, b.move_stack);
}

// push to stack
unsigned history_stack::push_move(unsigned before_player_pos_h) {
    move_stack.emplace_back(before_player_pos_h);
    return move_stack.size();
}
unsigned history_stack::push_change(unsigned row, unsigned col, char before) {
    if(move_stack.size() == 0)
        return 0;
    
    return move_stack[move_stack.size()-1].push_change(history_change(row, col, before));
}

// pop from stack
bool history_stack::pop_change(history_change& hc) {
    if(move_stack.size() == 0 || move_stack.back().num_changes() == 0)
        return false;

    return move_stack[move_stack.size()-1].pop_change(hc);
}
bool history_stack::pop_move(unsigned &player_pos) {
    if(move_stack.size() == 0)
        return false;

    player_pos = move_stack.back().get_player_pos_h();
    move_stack.pop_back();
    return true;
}

// accessors
unsigned history_stack::num_moves() const {
    return move_stack.size();
}
unsigned history_stack::num_changes_in_move() const {
    return move_stack.size()>0 ? move_stack.back().num_changes() : 0;
}