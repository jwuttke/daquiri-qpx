#pragma once

#include <string>
#include <list>
#include <vector>
#include <nlohmann/json.hpp>

#include <pugixml.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class XMLable {
 public:
  XMLable() {}

  virtual void to_xml(pugi::xml_node &node) const = 0;
  virtual void from_xml(const pugi::xml_node &node) = 0;
  virtual std::string xml_element_name() const = 0;

  ///////these functions needed by XMLableDB///////////
  //virtual bool operator== (const XMLable&) const = 0;
  //virtual bool shallow_equals(const XMLable&) const = 0;

};

template<typename T>
class XMLableDB : public XMLable {
public:
  XMLableDB() {}
  XMLableDB(std::string xml_name) {xml_name_ = xml_name;}
  //  std::string name() const {return name_;}
  std::string xml_element_name() const {return xml_name_;}

  bool empty() const {return my_data_.empty();}
  size_t size() const {return my_data_.size();}
  void clear() {my_data_.clear();}

  bool operator!= (const XMLableDB& other) const {return !operator==(other);}
  bool operator== (const XMLableDB& other) const
  {
    if (xml_name_ != other.xml_name_)
      return false;
    if (my_data_.size() != other.my_data_.size())
      return false;
    //ordering does not matter, deep compare on elements
    for (auto &q : my_data_)
      if (!other.has(q))
        return false;
    return true;
  }

  bool shallow_equals(const XMLableDB& other) const
  {
    return (xml_name_ == other.xml_name_);
    //&& (name_ == other.name_));
  }

  bool has(const T& t) const
  {
    for (auto &q : my_data_)
      if (q == t)
        return true;
    return false;
  }

  bool has_a(const T& t) const
  {
    for (auto &q : my_data_)
      if (t.shallow_equals(q))
        return true;
    return false;
  }
  
  void add(T t)
  {
    if (t == T())
      return;
    if (!has_a(t))
      my_data_.push_back(t);
  }

  void add_a(T t)
  {
    if (t == T())
      return;
    my_data_.push_back(t);
  }

  
  void replace(T t)
  {
    if (t == T())
      return;
    bool replaced = false;
    for (auto &q : my_data_)
      if (q.shallow_equals(t)) {
        replaced = true;
        q = t;
      }
    if (!replaced)
      my_data_.push_back(t);
  }

  void replace(size_t i, T t)
  {
    if ((i >= 0) && (i < size())) {
      typename std::list<T>::iterator it = std::next(my_data_.begin(), i);
      (*it) = t;
    }
  }
 
  void remove(const T &t)  //using deep compare
  {
    typename std::list<T>::iterator it = my_data_.begin();
    while (it != my_data_.end()) {
      if (*it == t)
        it = my_data_.erase(it);
      ++it;
    }
  }

  void remove_a(const T &t)   //using shallow compare
  {
    typename std::list<T>::iterator it = my_data_.begin();
    while (it != my_data_.end()) {
      if (it->shallow_equals(t))
        it = my_data_.erase(it);
      ++it;
    }
  }
  
  void remove(size_t i)
  {
    if ((i >= 0) && (i < size())) {
      typename std::list<T>::iterator it = std::next(my_data_.begin(), i);
      my_data_.erase(it);
    }
  }
    
  T get(T t) const
  {
    for (auto &q: my_data_)
      if (q.shallow_equals(t))
        return q;
    return T();    
  }
  
  T get(size_t i) const
  {
    if ((i >= 0) && (i < size())) {
      typename std::list<T>::const_iterator it = std::next(my_data_.begin(), i);
      return *it;
    }
    return T();    
  }
  
  void up(size_t i)
  {
    if ((i > 0) && (i < size())) {
      typename std::list<T>::iterator it = std::next(my_data_.begin(), i-1);
      std::swap( *it, *std::next( it ) );
    }
  }
    
  void down(size_t i)
  {
    if ((i >= 0) && ((i+1) < size())) {
      typename std::list<T>::iterator it = std::next(my_data_.begin(), i);
      std::swap( *it, *std::next( it ) );
    }
  }

  void write_xml(std::string file_name) const
  {
    pugi::xml_document doc;
    pugi::xml_node root = doc.root();
    this->to_xml(root);
    doc.save_file(file_name.c_str());
  }
  
  void read_xml(std::string file_name)
  {
    pugi::xml_document doc;
    if (!doc.load_file(file_name.c_str()))
      return;


    pugi::xml_node root = doc.child(xml_name_.c_str());

    if (root) {
      this->from_xml(root);
    }
  }

  std::vector<T> to_vector() const
  {
    if (!my_data_.empty())
      return std::vector<T>(my_data_.begin(), my_data_.end());
    else
      return std::vector<T>();
  }

  void from_vector(std::vector<T> vec)
  {
    my_data_.clear();
    if (!vec.empty())
      my_data_ = std::list<T>(vec.begin(), vec.end());
  }

  void to_xml(pugi::xml_node &node) const
  {
    pugi::xml_node child = node.append_child(xml_name_.c_str());
    for (auto &q : my_data_)
      q.to_xml(child);
  }

  void from_xml(const pugi::xml_node &node)
  {
    if (node.name() != xml_name_)
      return;
    for (pugi::xml_node &child : node.children()) {
      T t;
      t.from_xml(child);
      if (!has(t))  // if doesn't have it, append, no duplicates
        my_data_.push_back(t);
    }
  }

  friend void to_json(json& j, const XMLableDB& t)
  {
    j = t.my_data_;
//    for (auto k : t.my_data_)
//      j.push_back(json(k));
  }

  friend void from_json(const json& j, XMLableDB& t)
  {
    for (auto it : j)
      t.my_data_.push_back(it);
  }

  std::list<T> my_data_;

// protected:
  std::string xml_name_;
};
