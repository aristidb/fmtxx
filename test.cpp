#include <fmtxx/format_options.hpp>
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
#include <memory>
#include <algorithm>
#include <iostream>
#include <ostream>
#include <locale>

using namespace fmtxx;

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

  std::auto_ptr<formattable_interface> clone() const {
    return std::auto_ptr<formattable_interface>(new formattable(val));
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
  format_parser(formattable_container &cont, std::ostream &stream)
    : cont(cont), stream(stream)
    {}

  formattable_container &cont;
  std::ostream &stream;

  typedef std::string::const_iterator iterator;

  bool isdig(char ch) {
    switch (ch) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      return true;
    default:
      return false;
    }
  }

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
    ++begin;
    std::ostringstream composed;
    while (begin != end) {
      if (*begin == '{') {
        inner_format_element(begin, end, composed);
      } else if (*begin == '}') {
        ++begin;
        break;
      } else {
        composed << *begin++;
      }
    }
    if (begin == end)
      throw format_error("Premature end of format element");
    std::string const &definition = composed.str();
    iterator dbegin = definition.begin(), dend = definition.end();
    std::auto_ptr<formattable_interface> pformattable(find_formattable(dbegin, dend));
    format_options opt;
    if (dbegin != dend)
      format_specifier(dbegin, dend, opt);
    pformattable->append(stream, opt);
  }

  void format_specifier(iterator &begin, iterator end, format_options &opt) {
    //TODO
  }

  void inner_format_element(iterator &begin, iterator end, std::ostringstream &composed) {
    ++begin; // skip {
    std::auto_ptr<formattable_interface> p(find_formattable(begin, end));
    p->append(composed);
  }

  std::auto_ptr<formattable_interface> find_formattable(iterator &begin, iterator end) {
    if (begin == end)
      throw format_error("Premature end of format element");
    formattable_interface *p = 0;
    if (isdig(*begin)) { // integer
      boost::uint32_t position = 0;
      boost::uint32_t mult = 1;
      do {
        position = position + (*begin - '0')  * mult;
        mult *= 10;
      } while (++begin != end && isdig(*begin));
      if (position > cont.positional_container.size())
        throw format_error("Invalid format element position");
      p = &cont.positional_container[position];
    } else {
      iterator it = begin;
      while (begin != end && *begin != '}' && *begin != ':' && *begin != '.')
        ++begin;
      std::string name(it, begin);
      formattable_container::named_container_type::iterator found =
        cont.named_container.find(name);
      if (found == cont.named_container.end())
        throw format_error("Invalid format element name");
      p = found->second;
    }

    if (begin == end)
      return p->clone();

    if (*begin == '}' || *begin == ':') {
      ++begin;
      return p->clone();
    } else if (*begin == '.') {
      iterator it = ++begin;
      while (begin != end && *begin != '}')
        ++begin;
      if (begin == end)
        throw format_error("Premature end of format element");
      std::string subscript(it, begin);
      ++begin;
      return p->subscript(subscript);
    } else {
      throw format_error("Invalid format element");
    }
  }
};

template<typename Seq>
void vformat(std::string const &format, Seq const &seq, std::ostream &stream) {
  formattable_container cont;
  formattable_container_add add_cont = { cont };
  boost::fusion::for_each(seq, add_cont);
  format_parser parser(cont, stream);
  parser.parse(format);
}

int main() {
  std::cout << "Hello Formatting.\n";
  std::cout << "sizeof(format_options) = " << sizeof(format_options) << "\n";
  vformat("{0{1}} {x}\n", boost::fusion::make_vector(4, 0, named("x", 5.2)), std::cout);
}
