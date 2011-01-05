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

#ifndef _H_BF_EVAL
#define _H_BF_EVAL

#include "bf_state.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>

namespace detail {

inline int display_error_cause(const char* message, const char* program, unsigned int command_index)
{
        //! Display error and what caused it.
        std::cout<<"Error: "<<message<<", cause:"<<std::endl
                 <<program<<std::endl<<std::setfill(' ')<<std::setw(command_index + 1)<<'^'<<std::endl;

        return EXIT_FAILURE;
}

//! finds the matching ']' for a '['
unsigned int track_forward(const char* program, unsigned int command_index, unsigned int bound)
{
        unsigned int i = command_index;
        unsigned int op_count = 0;

        while (i != bound) {
                switch (program[i]) {
                case ']': --op_count; break;
                case '[': ++op_count; break;
                }

                if (op_count == 0)
                        return i;
                ++i;
        }

        throw std::runtime_error("can't find corresponding command");
        return 0;
}

inline unsigned int repetition_length(const char* program, size_t program_size, unsigned int command_index, char command)
{
        unsigned int length = 0;

        while (program[command_index++] == command && ++length) {
        }

        return length;
}

struct composite_command {
        const char*       command;           // Sequence of commands that make up the composite
        const size_t      command_length;    // Length of the sequence
        const int         table_value;       // Value for the optimizations table
};

//! The optimizer currently supports only two values.
const composite_command CC_Clear = { "[-]",    sizeof "[-]"    - 1, -1 };
const composite_command CC_Add   = { "[->+<]", sizeof "[->+<]" - 1, -2 };

typedef cells_t<std::vector<int> > optimizations_table;

//! Given a composite command pattern, determines whether command_index is at the start of the command.
inline bool is_composite_command(const char* program, size_t program_size, unsigned int& command_index, const char* pattern, size_t pattern_size)
{
        return command_index + pattern_size - 1 < program_size && !memcmp(
                static_cast<const void *>(program + command_index)
              , static_cast<const void *>(pattern)
              , pattern_size
        );
}

//! Optimizes composite and repeated commands. Creates a jump table for corresponding
//  '[' and ']' commands.
void optimize(const char* program, size_t program_size, optimizations_table& table)
{
        unsigned int command_index = 0;

        while (command_index != program_size) {
                switch (program[command_index]) {
                case ' ':
                case '\n':
                        break;
                case '>':
                        command_index += (table[command_index] = detail::repetition_length(
                                program
                              , program_size
                              , command_index
                              , '>'
                        ));
                        continue;
                case '<':
                        command_index += (table[command_index] = detail::repetition_length(
                                program
                              , program_size
                              , command_index
                              , '<'
                        ));
                        continue;
                case '+':
                        command_index += (table[command_index] = detail::repetition_length(
                                program
                              , program_size
                              , command_index
                              , '+'
                        ));
                        continue;
                case '-':
                        command_index += (table[command_index] = detail::repetition_length(
                                program
                              , program_size
                              , command_index
                              , '-'
                        ));
                        continue;
                case '[':
                        //! Cell clear.
                        if (is_composite_command(program, program_size, command_index, CC_Clear.command, CC_Clear.command_length)) {
                                table[command_index] = CC_Clear.table_value;
                                command_index += CC_Clear.command_length;
                                continue;
                        }

                        //! Add current cell to the next cell, clear current cell, put result into next cell.
                        if (is_composite_command(program, program_size, command_index, CC_Add.command, CC_Add.command_length)) {
                                table[command_index] = CC_Add.table_value;
                                command_index += CC_Add.command_length;
                                continue;
                        }

                        //! Find the matching ']'
                        table[command_index] = detail::track_forward(program, command_index, program_size);
                        table[table[command_index]] = command_index;
                        break;                      
                }

                ++command_index;
        }
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
                        }

                        ++command_index;
                }
        }
        catch (std::runtime_error& e) {
                return detail::display_error_cause(e.what(), program, command_index);
        }

        return EXIT_SUCCESS;
}

namespace detail {

struct reader_t {
        explicit reader_t(const char* filename) : length_(0) {
                std::ifstream file(filename, std::fstream::in);

                if (!file)
                        throw std::runtime_error("Couldn't read file.");

                std::string line;

                while (std::getline(file, line)) {
                        raw_.insert(raw_.end(), line.begin(), line.end());
                        length_ += line.size();
                }
        }

        const char* raw() { return raw_.c_str(); }
        size_t size() const { return length_; }

private:
        std::string raw_;
        size_t length_;
};

} // namespace detail

int evaluate(const char* filename, bool ignore_unknowns = false)
{
        try {
                detail::reader_t reader(filename);

                return evaluate(reader.raw(), reader.size(), ignore_unknowns);
        }
        catch (std::runtime_error& e) {
                std::cout<<e.what()<<std::endl;
        }

        return EXIT_FAILURE;
}

#endif /* _H_BF_EVAL */
