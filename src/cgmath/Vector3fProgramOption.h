// Copyright 2008 Drew Olbrich

#ifndef CGMATH__VECTOR3F_PROGRAM_OPTION__INCLUDED
#define CGMATH__VECTOR3F_PROGRAM_OPTION__INCLUDED

#include <boost/program_options/value_semantic.hpp>

#include "Vector3f.h"

// A partial template specialization of boost::program_options::typed_value
// that allows it to parse Vector3f types.

namespace boost {

namespace program_options {

template<class charT>
class typed_value<cgmath::Vector3f, charT> 
    : public value_semantic_codecvt_helper<charT>,
        public typed_value_base
{
public:
    typed_value(cgmath::Vector3f *store_to) 
        : m_store_to(store_to),
          m_name("x y z") {
    }

    // Set the name of the argument displayed in the usage message
    // so it's something other than "x y z".
    // Example:
    //     bpo::value<cgmath::Vector3f>()->set_name("r g b")
    typed_value *set_name(const std::string &name) {
        m_name = name;
        return this;
    }

    // For boost::program_options::value_semantic:
    virtual std::string name() const {
        return m_name;
    }

    // For boost::program_options::value_semantic:
    virtual bool is_composing() const {
        return false;
    }

    // For boost::program_options::value_semantic:
    virtual bool is_required() const {
        return false;
    }

    // For boost::program_options::value_semantic:
    unsigned min_tokens() const { 
        return 3; 
    }

    // For boost::program_options::value_semantic:
    unsigned max_tokens() const { 
        return 3; 
    }

    // For boost::program_options::value_semantic_codecvt_helper:
    void xparse(boost::any &value_store,
        const std::vector<std::basic_string<charT> > &new_tokens) const {
        cgmath::Vector3f value;
        for (size_t index = 0; index < new_tokens.size(); ++index) {
            value[index] = boost::lexical_cast<float>(new_tokens[index]);
        }
        value_store = boost::any(value);
    }

    // For boost::program_options::value_semantic:
    virtual bool apply_default(boost::any &) const {
        return false;
    }

    // For boost::program_options::value_semantic:
    virtual void notify(const boost::any &value_store) const {
        const cgmath::Vector3f *value = boost::any_cast<const cgmath::Vector3f>(&value_store);
        if (m_store_to != NULL) {
            *m_store_to = *value;
        }
    }

    // For boost::program_options::typed_value_base:
    virtual const std::type_info &value_type() const {
        return typeid(cgmath::Vector3f);
    }

private:
    cgmath::Vector3f *m_store_to;
    std::string m_name;
};
    
} // namespace program_options

} // namespace boost

#endif // CGMATH__VECTOR3F_PROGRAM_OPTION__INCLUDED
