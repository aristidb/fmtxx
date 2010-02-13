#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <boost/cstdint.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/ref.hpp>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <ostream>

typedef boost::int_least32_t index_number_type;

typedef boost::variant<std::string, index_number_type> index_type;

struct format_options {
  format_options()
    : subscript(boost::none),
      fill(boost::none),
      alignment(NO_ALIGNMENT_SPECIFIED),
      sign(NO_SIGN_SPECIFIED),
      alternate_form(false),
      width(boost::none),
      precision(boost::none),
      format(NO_FORMAT_SPECIFIED),
      radix(boost::none),
      other()
    {}

  typedef index_type subscript_type;
  boost::optional<index_type> subscript;

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

struct formattable_interface {
  virtual void append(
      std::ostream &stream,
      format_options const &options = format_options()
  ) = 0;
  virtual ~formattable_interface() = 0;
};

formattable_interface::~formattable_interface() {}

template<typename T, typename Cond = void>
class formattable : public formattable_interface {
public:
  formattable(T const &val) : val(val) {}

  void append(std::ostream &stream, format_options const &options) {
    // TODO: actually use format options
    stream << val;
  }

private:
  T val;
};

template<typename T>
struct named_type {
  typedef T value_type;

  std::string name;
  value_type value;

  named_type(std::string const &name, T const &value)
    : name(name), value(value)
    {}
};

template<typename T>
named_type<T> named(std::string const &name, T const &value) {
  return named_type<T>(name, value);
}

struct formattable_container {
  typedef boost::ptr_vector<formattable_interface> positional_container_type;
  positional_container_type positional_container;
  typedef boost::ptr_map<std::string, formattable_interface> named_container_type;
  named_container_type named_container;

  template<typename T>
  void add(T const &val) {
    this->positional_container.push_back(new formattable<T>(val));
  }

  template<typename T>
  void add(named_type<T> const &named) {
    std::string k = named.name;
    formattable_interface *p = new formattable<T>(named.value);
    this->named_container.insert(k, p);
  };
};

struct formattable_container_add {
  formattable_container &cont;

  template<typename T>
  void operator() (T const &x) const {
    cont.add(x);
  }
};

struct format_error : std::runtime_error {
  format_error(std::string const &w) : std::runtime_error(w) {}
  ~format_error() throw() {}
};

struct format_parser {
  formattable_container &cont;
  std::ostream &stream;

  typedef std::string::const_iterator iterator;

  void parse(std::string const &str) {
    std::string::const_iterator begin = str.begin(), end = str.end();
    parse(begin, end);
  }

  void parse(iterator &begin, iterator end) {
    while (begin != end)
      element(begin, end);
  }

  void element(iterator &begin, iterator end) {
    if (*begin == '{') {
      iterator it = begin;
      if (++it != end && *it == '{') {
        stream << '{';
        begin = ++it;
      } else {
        format_element(begin, end);
      }
    } else if (*begin == '}') {
      if (++begin != end && *begin == '}') {
        stream << '}';
        ++begin;
      } else {
        throw format_error("Unbalanced } in format string");
      }
    } else {
      stream << *begin;
      ++begin;
    }
  }

  void format_element(iterator &begin, iterator end) {
    std::string composed;
    composed.reserve(end - begin);
    while (begin != end) {
      if (*begin == '{')
        inner_format_element(begin, end, composed);
      else
        composed += *begin;
    }
  }

  void inner_format_element(iterator &begin, iterator end, std::string &composed) {
    //TODO
  }
};

template<typename Seq>
void vformat(std::string const &format, Seq const &seq, std::ostream &stream) {
  formattable_container cont;
  formattable_container_add add_cont = { cont };
  boost::fusion::for_each(seq, add_cont);
  for (std::size_t i = 0; i < cont.positional_container.size(); ++i) {
    stream << i << ": ";
    cont.positional_container[i].append(stream);
    stream << '\n';
  }
  for (formattable_container::named_container_type::iterator it = cont.named_container.begin(); it != cont.named_container.end(); ++it) {
    stream << it->first << ": ";
    it->second->append(stream);
    stream << '\n';
  }
}

int main() {
  std::cout << "Hello Formatting.\n";
  std::cout << "sizeof(format_options) = " << sizeof(format_options) << "\n";
  vformat("{0} {x}", boost::fusion::make_vector(4, named("x", 5.0)), std::cout);
}
