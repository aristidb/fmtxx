#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/cstdint.hpp>
#include <string>
#include <iostream>
#include <ostream>

typedef boost::int_least32_t index_number_type;

typedef boost::variant<std::string, index_number_type> index_type;

struct format_options {
  typedef index_type subscript_type;
  index_type subscript;

  typedef char fill_type;
  boost::optional<fill_type> fill;

  enum alignment_type {
    NO_ALIGNMENT_SPECIFIED = -1,
    left_aligned = '<',
    right_aligned = '>',
    numeric_left_aligned = '=',
    center_aligned = '^'
  };
  alignment_type alignment;

  enum sign_type {
    NO_SIGN_SPECIFIED = -1,
    both_sign = '+',
    negative_only_sign = '-',
    space_sign = ' '
  };
  sign_type sign;

  bool alternate_form;

  typedef boost::uint32_t width_type;
  boost::optional<width_type> width;

  typedef boost::uint32_t precision_type;
  boost::optional<precision_type> precision;

  enum format_type {
    NO_FORMAT_SPECIFIED = -1,
    // general
    localised_number = 'n',
    // integer
    binary = 'b',
    character_from_integer = 'c',
    decimal_integer = 'd',
    octal_integer = 'o',
    hex_integer = 'x',
    up_hex_integer = 'X',
    // floating point
    exponent_notation = 'e',
    up_exponent_notation = 'E',
    fixed_point = 'f',
    up_fixed_point = 'F',
    general_format = 'g',
    up_general_format = 'G',
    percentage = '%'
  };
  format_type format;

  typedef boost::uint32_t radix_type;
  radix_type radix;

  std::string other;
};

struct formattable {
  virtual void append(std::string &str, format_options const &options) = 0;
  virtual std::string as_parameter_string() = 0;
  virtual ~formattable() = 0;
};

formattable::~formattable() {}

int main() {
  std::cout << "Hello Formatting.\n";
  std::cout << "sizeof(format_options) = " << sizeof(format_options) << "\n";
}
