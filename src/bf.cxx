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

#include "bf_evaluate.h"
#include "bf_reader.h"

#include <cstdlib>
#include <getopt.h>
#include <cstring>
#include <stdexcept>
#include <iostream>

int usage()
{
        using namespace std;

        cout<<"Usage: [OPTION]... <source-file>"<<endl<<endl;
        cout<<"Options:"<<endl;
        cout<<"  -e, --evaluate=program    evaluate a one line program"<<endl;
        cout<<"  -i, --ignore-unknowns     ignore unknown command within the program"<<endl;
        cout<<"  -s, --use-signed-cells    use a signed type for each cell"<<endl;
        cout<<"  -h, --help                print this message"<<endl;

        return EXIT_SUCCESS;
}

struct options_t {
        options_t() : ignore_unknowns(false), inline_program(false), use_signed(false) {
        }

        bool ignore_unknowns;        // ignore any unknown characters encountered
        bool inline_program;         // -e was speficied
        bool use_signed;             // use signed cells
};

int resolve_options_and_evaluate(const options_t& options, int optind, int argc, char* argv[])
{
        typedef signed   int S_storage;
        typedef unsigned int U_storage;

        if (options.inline_program && optind == argc) {
                return options.use_signed ?
                        evaluate<S_storage>(argv[optind - 1], strlen(argv[optind - 1]), options.ignore_unknowns) :
                        evaluate<U_storage>(argv[optind - 1], strlen(argv[optind - 1]), options.ignore_unknowns);
        }

        if (!options.inline_program && optind < argc) {
                try {
                        detail::reader_t program(argv[optind]);

                        return options.use_signed ?
                                evaluate<S_storage>(program.raw(), program.size(), options.ignore_unknowns) :
                                evaluate<U_storage>(program.raw(), program.size(), options.ignore_unknowns);
                }
                catch (std::runtime_error& e) {
                        std::cout<<e.what()<<std::endl;
                        return EXIT_FAILURE;
                }

        }

        return usage();
}

int main(int argc, char* argv[])
{
        option long_options[] = {
                { "evaluate",         required_argument, 0, 'e' }
              , { "use-signed-cells", no_argument,       0, 's' }
              , { "ignore-unknowns",  no_argument,       0, 'i' }
              , { "help",             no_argument,       0, 'h' }
        };

        options_t options;

        int c;
        int option_index = 0;

        while ((c = getopt_long(argc, argv, "e:ihs", long_options, &option_index)) != -1) {
                switch (c) {
                case 'i':
                        options.ignore_unknowns = true;
                        break;
                case 'e':
                        options.inline_program = true;
                        break;
                case 's':
                        options.use_signed = true;
                        break;
                case 'h':
                case '?':
                        return usage();
                }
        }

        return resolve_options_and_evaluate(options, optind, argc, argv);
}
