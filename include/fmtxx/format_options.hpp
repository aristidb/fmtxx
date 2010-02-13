#ifndef FMTXX_FORMAT_OPTIONS_HPP
#define FMTXX_FORMAT_OPTIONS_HPP

#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <boost/cstdint.hpp>
#include <string>

namespace fmtxx {

struct format_options {
  format_options()
    : fill(boost::none),
      alignment(NO_ALIGNMENT_SPECIFIED),
      sign(NO_SIGN_SPECIFIED),
      alternate_form(false),
      width(boost::none),
      precision(boost::none),
      format(NO_FORMAT_SPECIFIED),
      radix(boost::none),
      other()
    {}

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
  boost::optional<radix_type> radix;

  std::string other;
};

}

#endif
