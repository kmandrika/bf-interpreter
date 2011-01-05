//! A simple Brainfuck interpreter.
/*!
    Copyright (C) 2011  Konstantin Mandrika

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _H_BF_EVALUATE
#define _H_BF_EVALUATE

#include "bf_state.h"
#include "bf_optimize.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdlib>

namespace detail {

inline int display_error_cause(const char* message, const char* program, unsigned int command_index)
{
        //! Display error and what caused it.
        std::cout<<"Error: "<<message<<", cause:"<<std::endl
                 <<program<<std::endl<<std::setfill(' ')<<std::setw(command_index + 1)<<'^'<<std::endl;

        return EXIT_FAILURE;
}

} // namespace detail

//! Optimizes and evaluates the program.
int evaluate(const char* program, size_t program_size, bool ignore_unknowns = false)
{
        detail::state_t<unsigned int, std::vector<unsigned int> > state;
        detail::optimizations_table table(65536, 1);

        detail::optimize(program, program_size, table);

        unsigned int command_index = 0;

        try {
                while (command_index != program_size) {
                        int table_value = table[command_index];

                        switch (program[command_index]) {
                        case ' ':
                        case '\n':
                                break;
                        case '>':
                                command_index += table_value;
                                state.increment_pc(table_value);
                                continue;
                        case '<':
                                command_index += table_value;
                                state.decrement_pc(table_value);
                                continue;
                        case '+':
                                command_index += table_value;
                                state.increment_current_cell(table_value);
                                continue;
                        case '-':
                                command_index += table_value;
                                state.decrement_current_cell(table_value);
                                continue;
                        case '.':
                                std::cout<<static_cast<unsigned char>(state.get())<<std::flush;
                                break;
                        case ',':
                                std::cin>>state.set();
                                break;
                        case '[':
                                if (table_value == detail::CC_Clear.table_value) {
                                        state.set() = 0;
                                        command_index += detail::CC_Clear.command_length;
                                        continue;
                                }

                                if (table_value == detail::CC_Add.table_value) {
                                        unsigned int this_cell = state.get();
                                        unsigned int next_cell;

                                        state.set() = 0;
                                        state.increment_pc();

                                        next_cell = state.get();

                                        state.set() = this_cell + next_cell;
                                        state.decrement_pc();

                                        command_index += detail::CC_Add.command_length;
                                        continue;
                                }

                                if (state.get() == 0)
                                        command_index = table_value;
                                break;
                        case ']':
                                if (state.get() != 0)
                                        command_index = table_value;
                                break;
                        default:
                                if (!ignore_unknowns)
                                        throw std::runtime_error("found an invalid command");
                        }

                        ++command_index;
                }
        }
        catch (std::runtime_error& e) {
                return detail::display_error_cause(e.what(), program, command_index);
        }

        return EXIT_SUCCESS;
}

#endif /* _H_BF_EVALUATE */
