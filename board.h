/* ****************************************************** *
 * AUTHOR:      Noah Krim
 * ASSIGNMENT:  Programming Project 01
 * CLASS:       CS_08
 * ------------------------------------------------------ *
 * FILE: board.h
 *  Header file for the board of the game.
 *  Supports parsing board layout files, rendering the board,
 *  and handles the logic for rules and movement.
 * ****************************************************** */

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>
#include <string>

#include "history.h"

// Global constants
const unsigned BOARD_MAX_HASH_VALUE = 46000; // safe value for hashing

// Tile enums
enum class Tile: char {
    wall = '#',
    floor = ' ',
    box = 'B',
    goal = '.',
    player = '@',
    box_on_goal = 'G',
};

// Direction enums
enum class Dir {
    up=0, left, down, right
};

class board {
private:
    // Dynamically Allocated board states
    char** original_data;
    char** data;

    // Locations of goals
    // They are the only data that can be overwritten
    // but must persist, so this is where their info is stored
    std::vector<unsigned> goal_coords;

    // History object
    history_stack hist;

    // Dimensional member constants
    unsigned rows;
    unsigned cols;
    unsigned hash_value; // max(rows,cols)+1, used for hashing pairs

    // Mutable data
    unsigned original_player_pos_r;
    unsigned original_player_pos_c;
    unsigned player_pos_r;
    unsigned player_pos_c;
    bool loaded;
    bool solved;

    // Private utility functions
    unsigned hash_coord(unsigned r, unsigned c) const;
    void unhash_coord(unsigned h, unsigned& r, unsigned& c) const;
public:
    // Constructors
    board(); // Empty board constructor for use as return for invalid parsing
    board(unsigned rows, unsigned cols);

    // Destructor
    virtual ~board();

    // Copy-Swap Implementation
    board(const board& o);
    board(board&& rv);
    board& operator=(board o);
    friend void swap(board& a, board& b);

    // Operator overloads
    friend std::ostream& operator<<(std::ostream& o, const board& b);

    // Board parsing from file
    static std::vector<board> parse_from_file(const char* filename);

    // Movement functions
    bool move(Dir d);

    // Board manipulations
    void reset();
    bool undo();
    void clear_history();
    bool is_coord_goal(unsigned r, unsigned c) const;

    // Enum conversion
    static bool to_tile(const char in, Tile& out);

    // General accessors
    unsigned get_rows() const;
    unsigned get_cols() const;
    unsigned num_goals() const;
    unsigned num_moves() const;
    bool check_goals();
    bool is_valid() const;
    bool is_loaded() const;
    bool is_solved() const;
};

#endif