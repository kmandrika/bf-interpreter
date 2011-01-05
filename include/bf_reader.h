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

#ifndef _H_BF_READER
#define _H_BF_READER

#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>

namespace detail {

struct reader_t {
        explicit reader_t(const char* filename);

        const char* raw();
        size_t size() const;

private:
        std::string raw_;
        size_t length_;
};

reader_t::reader_t(const char* filename) : length_(0)
{
        std::ifstream file(filename, std::fstream::in);

        if (!file)
                throw std::runtime_error("Couldn't read file.");

        std::string line;

        while (std::getline(file, line)) {
                raw_.insert(raw_.end(), line.begin(), line.end());
                length_ += line.size();
        }
}

inline const char* reader_t::raw()
{
        return raw_.c_str();
}

inline size_t reader_t::size() const
{
        return length_;
}

} // namespace detail

#endif /* _H_BF_READER */
