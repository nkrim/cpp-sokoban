/* ****************************************************** *
 * AUTHOR:      Noah Krim
 * ASSIGNMENT:  Programming Project 01
 * CLASS:       CS_08
 * ------------------------------------------------------ *
 * FILE: board.cpp
 *  Definition file for the board class.
 * ****************************************************** */

#include "board.h"

#include <fstream>
#include <sstream>

/* ====================================================== *
 * Private utility functions
 * ====================================================== */
unsigned board::hash_coord(unsigned r, unsigned c) const {
    return hash_value*r + c;
}
void board::unhash_coord(unsigned h, unsigned& r, unsigned& c) const {
    r = h/hash_value;
    c = h%hash_value;
}

/* ====================================================== *
 * Constructors
 * ====================================================== */
board::board()
    :original_data(NULL),data(NULL)
    ,rows(0),cols(0),hash_value(0)
    ,loaded(false),solved(false)
{}
board::board(unsigned rows, unsigned cols)
    :original_data(NULL),data(NULL)
    ,rows(rows),cols(cols),hash_value(std::max(rows,cols)+1)
    ,loaded(false),solved(false)
{
    // invalid dimensions producing high hash-value
    // return early so that the pointers aren't allocated
    // and the is_valid() function will return false
    if(hash_value > BOARD_MAX_HASH_VALUE)
        return;

    // allocate 2d data arrays and fill with walls
    original_data = new char*[rows];
    for(int r=0; r<rows; r++) {
        original_data[r] = new char[cols];
        for(int c=0; c<cols; c++) {
            original_data[r][c] = (char)Tile::floor;
        }
    }
    data = new char*[rows];
    for(int r=0; r<rows; r++) {
        data[r] = new char[cols];
        for(int c=0; c<cols; c++) {
            data[r][c] = (char)Tile::floor;
        }
    }

    // all other variables initialized when parsed
}

/* ====================================================== *
 * Destructor
 * ====================================================== */
board::~board() {
    if(original_data) {
        for(unsigned r=0; r<rows; r++)
            delete [] original_data[r];
        delete [] original_data;
    }

    if(data) {
        for(unsigned r=0; r<rows; r++)
            delete [] data[r];
        delete [] data;
    }
}

/* ====================================================== *
 * Copy-Swap Implementation
 * ====================================================== */
board::board(const board& o)
    :board(o.rows, o.cols)
{
    // copy variables
    this->goal_coords = o.goal_coords;
    this->hist = o.hist;
    this->original_player_pos_r = o.original_player_pos_r;
    this->original_player_pos_c = o.original_player_pos_c;
    this->player_pos_r = o.player_pos_r;
    this->player_pos_c = o.player_pos_c;
    this->loaded = o.loaded;
    this->solved = o.solved;

    // copy data
    if(o.is_valid()) {
        for(int r=0; r<rows; r++) {
            for(int c=0; c<cols; c++) {
                original_data[r][c] = o.original_data[r][c];
                data[r][c] = o.data[r][c];
            }
        }
    }
}
board::board(board&& rv)
    :board()
{
    swap(*this, rv);
}
board& board::operator=(board o) {
    swap(*this, o);
    return *this;
}
void swap(board& a, board& b) {
    using std::swap; 

    swap(a.original_data, b.original_data);
    swap(a.data, b.data);

    swap(a.goal_coords, b.goal_coords);
    swap(a.hist, b.hist);

    swap(a.rows, b.rows);
    swap(a.cols, b.cols);
    swap(a.hash_value, b.hash_value);

    swap(a.original_player_pos_r, b.original_player_pos_r);
    swap(a.original_player_pos_c, b.original_player_pos_c);
    swap(a.player_pos_r, b.player_pos_r);
    swap(a.player_pos_c, b.player_pos_c);
    swap(a.loaded, b.loaded);
}

/* ====================================================== *
 * Operator Overloads 
 * ====================================================== */
std::ostream& operator<<(std::ostream& o, const board& b) {
    const char border = ' ';
    for(int c=0; c<b.cols+4; c++)
        o << border;
    o << std::endl;

    for(int r=0; r<b.rows; r++) {
        o << border << border;
        for(int c=0; c<b.cols; c++)
            o << b.data[r][c];
        o << border << border << std::endl;
    }

    for(int c=0; c<b.cols+4; c++)
        o << border;
    o << std::endl;

    return o;
}

/* ====================================================== *
 * Board Parsing 
 * ====================================================== */
std::vector<board> board::parse_from_file(const char* filename) { 
    
    std::fstream f(filename, std::fstream::in);
    std::string line;
    std::stringstream buffer;
    std::vector<board> bs;

    // file could not open
    if(!f.is_open()) {
        std::cerr << "Error: board::parse_from_file() -> unable to open board file `"
                  << filename << '`';
        return bs;
    }

    // read initial line
    std::getline(f, line);
    buffer = std::stringstream(line);
    unsigned input_levels;
    buffer >> input_levels;

    bs.reserve(input_levels);

    for(int i=0; i<input_levels; i++) {
        if(f.eof())
            return bs;
        std::getline(f, line);
        buffer = std::stringstream(line);
        unsigned input_rows, input_cols;
        buffer >> input_rows >> input_cols;

        // initialize
        if(std::min(input_rows,input_cols) == 0 
        || std::max(input_rows,input_cols) > BOARD_MAX_HASH_VALUE) {
            std::cerr << "Error: board::parse_from_file() -> invalid row/col values: "
                        << input_rows << ',' << input_cols << std::endl;
            return bs;
        }
        board b(input_rows, input_cols);

        // read from file
        for(int r=0; r<b.rows; r++) {
            if(f.eof())
                return bs;
            std::getline(f, line);
            for(int c=0; c<std::min(b.cols,(unsigned)line.size()); c++) {
                char x = line[c];
                b.original_data[r][c] = x;
                b.data[r][c] = x;

                // check for goal
                if(x == (char)Tile::goal || x == (char)Tile::box_on_goal) {
                    b.goal_coords.push_back(b.hash_coord(r,c));
                }
                else if(x == (char)Tile::player) {
                    b.original_player_pos_r = r;
                    b.original_player_pos_c = c;
                    b.player_pos_r = r;
                    b.player_pos_c = c;
                }
            }
        }

        // return board
        b.loaded = true;
        bs.push_back(b);
    }

    return bs;
}

/* ====================================================== *
 * Movement Functions
 * ====================================================== */
bool board::move(Dir d) { 
    int delta_r = (int)d&1 ? 0 : (int)d-1;
    int delta_c = (int)d&1 ? (int)d-2 : 0;

    bool pushing_box = false;
    bool push_on_goal = false;
    bool valid_move = false;
    
    // check one ahead
    char one_ahead = data[player_pos_r+delta_r][player_pos_c+delta_c];
    switch(one_ahead) {
        case (char)Tile::box: 
        case (char)Tile::box_on_goal:
            pushing_box = true; 
            break;
        case (char)Tile::floor:
        case (char)Tile::goal:
            valid_move = true;
            break;
    }

    // if not pushing and invalid, exit early
    if(!pushing_box && !valid_move)
        return false;

    // check box conditions
    char two_ahead = data[player_pos_r+2*delta_r][player_pos_c+2*delta_c];
    switch(two_ahead) {
        case (char)Tile::goal:
            push_on_goal = true;
        case (char)Tile::floor:
            valid_move = true;
            break;
    }

    // if move still invalid, exit
    if(!valid_move)
        return false;

    // perform move
    hist.push_move(hash_coord(player_pos_r, player_pos_c));
    hist.push_change(player_pos_r, player_pos_c, (char)Tile::player);
    hist.push_change(player_pos_r+delta_r, player_pos_c+delta_c, one_ahead);
    data[player_pos_r+delta_r][player_pos_c+delta_c] = (char)Tile::player;
    data[player_pos_r][player_pos_c] = is_coord_goal(player_pos_r,player_pos_c) 
                                        ? (char)Tile::goal : (char)Tile::floor;
    // perform box push
    if(pushing_box) {
        hist.push_change(player_pos_r+2*delta_r, player_pos_c+2*delta_c, two_ahead);
        data[player_pos_r+2*delta_r][player_pos_c+2*delta_c] 
            = push_on_goal ? (char)Tile::box_on_goal : (char)Tile::box;
    }

    player_pos_r += delta_r;
    player_pos_c += delta_c;

    return true;
}

/* ====================================================== *
 * Board Manipulations
 * ====================================================== */
void board::reset() {
    hist.push_move(hash_coord(player_pos_r, player_pos_c));
    for(int r=0; r<rows; r++) {
        for(int c=0; c<cols; c++) {
            if(data[r][c] != original_data[r][c])
                hist.push_change(r, c, data[r][c]);
            data[r][c] = original_data[r][c];
        }
    }
    // If no changes were actually added, pop move
    if(hist.num_changes_in_move() == 0) {
        hist.pop_move(player_pos_r); // dump trash value
    }
    player_pos_r = original_player_pos_r;
    player_pos_c = original_player_pos_c;
}
bool board::undo() { 
    history_change hc;
    while(hist.pop_change(hc)) {
        data[hc.get_row()][hc.get_col()] = hc.get_before();
    } 
    unsigned player_pos_h;
    bool result = hist.pop_move(player_pos_h);
    if(result)
        unhash_coord(player_pos_h, player_pos_r, player_pos_c);
    return result;
}
void board::clear_history() {
    hist = history_stack();
}
bool board::is_coord_goal(unsigned r, unsigned c) const {
    unsigned h = hash_coord(r,c);
    for(auto it=goal_coords.begin(); it!=goal_coords.end(); ++it) {
        if(h == *it)
            return true;
    }
    return false;
}

/* ====================================================== *
 * Enum Conversion
 * ====================================================== */
static const unsigned TILE_COUNT = 5;
static const char tiles[] = {
    (char)Tile::wall,(char)Tile::floor,(char)Tile::box,
    (char)Tile::goal,(char)Tile::player,(char)Tile::box_on_goal
};
bool board::to_tile(const char in, Tile& out) {
    for(int i=0; i<TILE_COUNT; i++) {
        if(in == tiles[i]) {
            out = (Tile)in;
            return true;
        }
    }
    return false;
}

/* ====================================================== *
 * Accessors
 * ====================================================== */
unsigned board::get_rows() const { return rows; }
unsigned board::get_cols() const { return cols; }
unsigned board::num_goals() const { return goal_coords.size(); }
unsigned board::num_moves() const { return hist.num_moves(); }
bool board::check_goals() {
    unsigned r, c;
    for(auto it=goal_coords.begin(); it!=goal_coords.end(); ++it) {
        unhash_coord(*it, r, c);
        if(data[r][c] != (char)Tile::box_on_goal)
            return false;
    }
    solved = true;
    return true;
}
bool board::is_valid() const { 
    return original_data!= NULL && data!=NULL;
}
bool board::is_loaded() const { return loaded; }
bool board::is_solved() const { return solved; }
