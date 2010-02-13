/*
The MIT License

Copyright (c) 2010 Aristid Breitkreuz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef FMTXX_FORMATTABLE_INTERFACE
#define FMTXX_FORMATTABLE_INTERFACE

#include <fmtxx/format_options.hpp>
#include <ostream>
#include <memory>

namespace fmtxx {

struct formattable_interface {
  virtual void append(
      std::ostream &stream,
      format_options const &options = format_options()
  ) = 0;

  virtual std::auto_ptr<formattable_interface>
  subscript(std::string const &subscript) {
    return std::auto_ptr<formattable_interface>(0);
  }

  virtual std::auto_ptr<formattable_interface> clone() const = 0;

  virtual ~formattable_interface() {}
};

}

#endif
