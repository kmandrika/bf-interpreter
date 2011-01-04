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
#include <getopt.h>
#include <cstring>

int usage()
{
        using namespace std;

	cout<<"usage: [OPTION]... <source-file>"<<endl<<endl;
	cout<<"Options:"<<endl;
	cout<<"  -e, --evaluate=program    evaluate a one line program"<<endl;
	cout<<"  -i, --ignore-unknowns     ignore unknown command within the program"<<endl;
	cout<<"  -h, --help                print this message"<<endl;

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
        option long_options[] = {
	        { "evaluate",        required_argument, 0, 'e' }
	      , { "ignore-unknowns", no_argument,       0, 'i' }
	      , { "help",            no_argument,       0, 'h' }
	};

	int c;
	int option_index = 0;

	bool ignore_unknowns = false;
	bool inline_program = false;

	while ((c = getopt_long(argc, argv, "e:ih", long_options, &option_index)) != -1) {
	        switch (c) {
		case 'i':
		        ignore_unknowns = true;
			break;
		case 'e':
		        inline_program = true;
		        break;
		case 'h':
		case '?':
		        return usage();
		}
	}

	if (inline_program && optind == argc)
	        return evaluate(argv[optind - 1], strlen(argv[optind - 1]), ignore_unknowns);

	if (!inline_program && optind < argc)
	        return evaluate(argv[optind], ignore_unknowns);

	return usage();

#if 0
	const char program[] = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";
        evaluate(program, sizeof program - 1);
#endif
}
