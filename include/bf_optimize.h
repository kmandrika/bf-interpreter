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

#ifndef _H_BF_OPTIMIZE
#define _H_BF_OPTIMIZE

#include "bf_cells.h"

#include <stdexcept>
#include <vector>
#include <cstring>

namespace detail {

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

        //! Generates unique table values.
        static int next_unique() {
                static int unique = 0;
                return unique -= 1;
        }
};

//! The optimizer currently supports only two values.
const composite_command CC_Clear = { "[-]",    sizeof "[-]"    - 1, composite_command::next_unique() };
const composite_command CC_Add   = { "[->+<]", sizeof "[->+<]" - 1, composite_command::next_unique() };

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
                        command_index += (table[command_index] = repetition_length(
                                program
                              , program_size
                              , command_index
                              , '>'
                        ));
                        continue;
                case '<':
                        command_index += (table[command_index] = repetition_length(
                                program
                              , program_size
                              , command_index
                              , '<'
                        ));
                        continue;
                case '+':
                        command_index += (table[command_index] = repetition_length(
                                program
                              , program_size
                              , command_index
                              , '+'
                        ));
                        continue;
                case '-':
                        command_index += (table[command_index] = repetition_length(
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
                        table[command_index] = track_forward(program, command_index, program_size);
                        table[table[command_index]] = command_index;
                        break;                      
                }

                ++command_index;
        }
}

} // namespace detail

#endif /* _H_BF_OPTIMIZE */
