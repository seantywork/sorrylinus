#include "properties_file_reader.h"

#include <fstream>

using namespace utils;

/// ***********************************************************************************************************************
/// ************************************************* PUBLIC **************************************************************

/// CONSTRUCTOR()
PropertiesFileReader::PropertiesFileReader(std::string file_name) {

  // Open file
  std::ifstream prop_file(file_name, std::ios::in);
  if (!prop_file.is_open())
    throw std::string("File cannot be opened: " + file_name);

  // Read all not commented or blank lines
  std::string line {""};
  std::string key;
  std::string value;
  size_t i {0};
  while (!prop_file.eof()){
    getline(prop_file, line);
    if ( line.find('=') == std::string::npos) // Not a property
      continue;

    // Add key and value, trimming leading and trailing spaces, and skipping if the first character is '#'
    key.clear();
    i = 0;
    while (line[i] != '='){
      if (!isspace(line[i]))
        key.push_back(line[i]);
      i++;
    };
    if (key.length() == 0 || key[0] == '#' || key[0] == '!') // Not a property
      continue;

    // Read value
    value.clear();
    while ( isspace(line[++i]) );
    value  = line.substr(i);
    // And remove trailing spaces
    i = value.length();
    while ( isspace(value[--i]) );
    value.erase(++i);

    _properties.insert(std::make_pair(key, value));
  }

  // Close file
  prop_file.close();
}


/// operator() (string key)
std::vector<std::string> PropertiesFileReader::operator() (std::string key) const {
  auto ret = _properties.equal_range(key);
  if ( ret.first == _properties.end() )
    throw std::string("Key NOT found: " + key);

  std::vector<std::string> values;
  values.reserve(_properties.count(key));
  while ( ret.first != ret.second ) {
    values.push_back(ret.first->second);
    ret.first++;
  }

  return values;
}
