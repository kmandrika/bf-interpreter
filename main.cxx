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

#include "bf_eval.h"

extern const char program[] =
        "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++."
        ">.+++.------.--------.>+.>.";

int main(int argc, char* argv[])
{
        evaluate(program, sizeof program - 1);
}












































































#if 0
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <list>

#if 1
extern const char program[] = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";
#else
extern const char program[] =
    "++++[>+++++<-]>[<+++++>-]+<+["
    ">[>+>+<<-]++>>[<<+>>-]>>>[-]++>[-]+"
    ">>>+[[-]++++++>>>]<<<[[<++++++++<++>>-]+<.<[>----<-]<]"
    "<<[>>>>>[>>>[-]+++++++++<[>-<-]+++++++++>[-[<->-]+[<<<]]<[>+<-]>]<<-]<<-]";

#endif

unsigned char cells[65000 * 2] = { 0 };

void evaluate(const char* program, size_t program_size)
{
	unsigned char* ptr = cells;

        using namespace std;

	list<unsigned int> stack;

        unsigned int command_index = 0;

	while (command_index != program_size) {
	        switch (program[command_index]) {
		case ' ': break;
                case '\n': break;
		case '>': ++ptr;  break;
		case '<': --ptr;  break;
		case '+': ++*ptr; break;
		case '-': --*ptr; break;
		case '.': cout<<(unsigned int)(*ptr); cout.flush();
		        break;
		case ',': cin>>*ptr; break;
		case '[':
		        stack.push_back(command_index);
			cout<<"pushed at "<<command_index<<endl;

		        if (!*ptr) {
			        command_index = distance(program, const_cast<const char *>(strchr(program + command_index, ']'))) + 1;
				cout<<"\tdistance = "<<command_index<<endl;

				continue;
			}

			break;
		case ']':
		        cout<<program<<endl;
			cout<<setfill(' ')<<setw(command_index + 1)<<'^'<<endl;

		        if (*ptr) {
			        if (stack.size() == 0) {
				        cout<<"Mismatch"<<endl;
					return;
				}

			        command_index = stack.back() + 1;
				cout<<setfill(' ')<<setw(command_index + 1)<<'^'<<endl;
				break;
			}

			cout<<"poped at "<<command_index<<endl;

			stack.pop_back();
		        break;
		default:
		        cout<<"Invalid command found."<<endl;
			cout<<program<<endl;
			cout<<setfill(' ')<<setw(command_index + 1)<<'^'<<endl;
			return;
		}

		++command_index;
	}
}

int main(int argc, char* argv[])
{
        evaluate(program, sizeof program - 1);
}
#endif
