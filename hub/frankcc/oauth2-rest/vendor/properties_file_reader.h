//  MIT License
//
//  Copyright (c) 2018 Francisco de Lanuza
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#ifndef PROPERTIES_FILE_READER_H
#define PROPERTIES_FILE_READER_H

#include <string>
#include <map>
#include <vector>
#include <type_traits>

namespace utils
{
  /**
    *  \brief JAVA-like properties file reader class
    *           Each property key is separated from the value by a '='.
    *           Property keys can be duplicated
    *           Commented lines (starting with '#' or '!') and invalid lines are discarded.
    *           Leading and trailing spaces are removed.
  */
  class PropertiesFileReader
  {
  public:
    /**
      *  \brief Constructor
      *         Reads the file and initializes the map of key/value pairs
      *  @param file_name [in] Name of the properties file
      *  @throw  string exception containing the description of the issue
      */
    PropertiesFileReader(std::string file_name);

    /**
      *  \brief Destructor (INLINE)
      */
    inline ~PropertiesFileReader() {}

    /**
      *  \brief Returns the content of the file as a multimap of strings (INLINE)
      *  @param
      *  @return multimap<string,string> Refrence to the map of (key, value) pairs of type string, with repeating keys
      */
    inline const std::multimap<std::string, std::string> & getProperties() { return _properties; }

    /**
      *  \brief Returns the values of the properties with the specified key, converted to type T
      *  @param key [in] key which value has to be returned
      *  @param values [out] a reference to the vector which will be used to return the values for the properties with the specified key
      *  @return
      */
    template <class TYPE, typename = std::enable_if<std::is_arithmetic<TYPE>::value> >
    std::vector<TYPE> propertyCast(std::string key) const;

    /**
      *  \brief Returns the values of the properties with the specified key, as strings
      *  @param key [in] key which value has to be returned
      *  @return std::vector(std::string values for the properties with the specified key
      */
    std::vector<std::string> operator() (std::string key) const;

  protected:
    /**
      *  Multimap used to store in memory the content of the file
      */
    std::multimap<std::string, std::string> _properties;

  };


  /// property(string key, vector<T> &values)
  template <class TYPE, typename = std::enable_if<std::is_arithmetic<TYPE>::value> >
  std::vector<TYPE> PropertiesFileReader::propertyCast(std::string key) const {
    auto ret = _properties.equal_range(key);
    if ( ret.first == _properties.end() )
      throw std::string("Key NOT found: " + key);

    std::vector<TYPE> values;
    values.reserve(_properties.count(key));
    while ( ret.first != ret.second ) {
      if ( std::is_floating_point<TYPE>::value)
        values.push_back(TYPE(atof( (ret.first->second).c_str() )));
      else if ( sizeof(TYPE) > sizeof(int) )
        values.push_back(TYPE(atol( (ret.first->second).c_str() )));
      else
        values.push_back(TYPE(atoi( (ret.first->second).c_str() )));

      ret.first++;
    }

    return values;
  }

}

#endif // PROPERTIES_FILE_READER_H
