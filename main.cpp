/* ****************************************************** *
 * AUTHOR:      Noah Krim
 * ASSIGNMENT:  Programming Project 01
 * CLASS:       CS_08
 * ------------------------------------------------------ *
 * FILE: main.cpp
 *  Main file that handles user input and console output.
 * ****************************************************** */
#include <iostream>
#include <string>
#include <vector>

#include "board.h"

const char* levels_file = "levels.txt";

void print_menu(const std::vector<board>& bs) {
    std::cout
            << "//===========================\\\\"<< std::endl
            << "|| Sokoban (Warehouseman)    ||"  << std::endl
            << "|+- - - - - - - - - - - - - -+|"  << std::endl
            << "|| type `Q` to quit          ||"  << std::endl
            << "|+===========================+|"  << std::endl
            << "|| Level Select (enter #):   ||"  << std::endl
            << "||                           ||"  << std::endl;
    for(int i=0; i<bs.size(); i++) {
        printf("|| %02d %-22s ||\n", i+1, 
            bs[i].is_solved() ? "* (solved)" : 
            (bs[i].num_moves() > 0 ? "p (in progress)" : ""));
    }
    std::cout
            << "\\\\===========================//"<<std::endl
            << "level#: ";
}

void print_instructions() {
    std::cout 
        << "/---------------------------------------------------\\"<< std::endl
        << "| Rules:                                            |" << std::endl
        << "| Cover each goal (.) by pushing a box (B) onto it. |" << std::endl
        << "| Only one box can be pushed at a time.             |" << std::endl
        << "+---------------------------------------------------+" << std::endl
        << "| Legend:                                           |" << std::endl
        << "| @ -> player          . -> goal (push boxes here)  |" << std::endl
        << "| B -> box             G -> box on goal             |" << std::endl
        << "| empty space is open, everything else is a wall    |" << std::endl
        << "+---------------------------------------------------+" << std::endl
        << "| Commands:                                         |" << std::endl
        << "|   W   : move up                                   |" << std::endl
        << "| A S D : move left / down / right                  |" << std::endl
        << "|                                                   |" << std::endl
        << "| Z : undo                                          |" << std::endl
        << "| R : reset                                         |" << std::endl
        << "| < : previous level (progress will save)           |" << std::endl
        << "| > : next level     (progress will save)           |" << std::endl
        << "| Q/M : quit to main menu                           |" << std::endl
        << "| H : show rules and commands                       |" << std::endl
        << "\\---------------------------------------------------/"<< std::endl;
}

int main() {
    int current_level = -1;
    std::vector<board> bs = board::parse_from_file("levels.txt");

    std::string line;

    while(true) {

        // menu logic
        if(current_level < 0) {
            std::cout << std::endl;
            print_menu(bs);
            std::getline(std::cin, line);
            // look for q input
            char x = ' ';
            for(auto it=line.begin(); x==' '&&it!=line.end();++it)
                x = *it;
            switch(toupper(x)) {
                case 'Q':
                    return 0;
                case 'H':
                    print_instructions();
                    break;
            }
            // look for level select input
            current_level = atoi(line.c_str())-1;
            if(current_level >= 0) {
                std::cout << std::endl;
                print_instructions();
            }
        }

        // level logic
        if(current_level >= 0) {
            board *b = &bs[current_level];
            printf("Level #%02d\n",current_level+1);
            std::cout << *b;

            std::getline(std::cin, line);
            char x = ' ';
            for(auto it=line.begin(); x==' '&&it!=line.end();++it)
                x = *it;
            if(x != ' ') {
                x = toupper(x);

                bool move_success = false;
                switch(x) {
                    case 'M':
                    case 'Q':
                        current_level = -1;
                        b = NULL;
                        break;
                    case 'H':
                        print_instructions();
                        break;
                    case 'W': 
                        move_success = b->move(Dir::up);
                        break; 
                    case 'A': 
                        move_success = b->move(Dir::left);
                        break;
                    case 'S': 
                        move_success = b->move(Dir::down);
                        break;
                    case 'D': 
                        move_success = b->move(Dir::right);
                        break;
                    case 'R':
                        b->reset();
                        break;
                    case 'Z':
                        b->undo();
                        break;
                    case '<':
                        current_level--;
                        b = NULL;
                        break;
                    case '>':
                        current_level++;
                        if(current_level >= bs.size())
                            current_level = -1;
                        b = NULL;
                        break;
                }
            }

            if(b!=NULL && b->check_goals()) {
                printf("Level #%02d\n",current_level+1);
                std::cout << *b << "Level Complete!" << std::endl;
                b->reset();
                b->clear_history();

                bool all_clear = true;
                for(auto it=bs.begin(); all_clear&&it!=bs.end(); ++it) {
                    if(!it->is_solved())
                        all_clear = false;
                }

                if(all_clear) {
                    std::cout << "You have completed the game. Congratulations!" << std::endl;
                    std::cout << "Press enter to return to the menu." << std::endl;
                    current_level = -1;
                }
                else if(current_level < bs.size()-1) {
                    std::cout << "Press enter to continue to next level." << std::endl;
                    current_level++;
                }
                else {
                    std::cout << "The last level is done, but there's still more to solve!" << std::endl;
                    std::cout << "Press enter to return to the menu." << std::endl;
                    current_level = -1;
                }
                std::getline(std::cin, line);
            }
        }
    }

    return 0;
}