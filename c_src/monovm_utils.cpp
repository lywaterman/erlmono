
#include "monovm_utils.hpp"
#include <string>
#include "errors.hpp"

namespace monovm {

/////////////////////////////////////////////////////////////////////////////
//
//

class mono_dummy_t : public boost::static_visitor<void>
{
public :
    typedef mono_dummy_t self_t;

	MonoObject* mono_value;
	vm_t & _vm;

    mono_dummy_t(vm_t & vm) : mono_value(NULL), _vm(vm) {};

    void operator()(int32_t const& value)
    {
		mono_value = mono_value_box(_vm.domain, mono_get_int32_class(), &value); 
    }
    void operator()(int64_t const& value)
    {
		mono_value = mono_value_box(_vm.domain, mono_get_int64_class(), &value);
    }
    void operator()(double const& value)
    {
		mono_value = mono_value_box(_vm.domain, mono_get_double_class(), &value);
    }
    void operator()(erlcpp::num_t const& value)
    {
        self_t & self = *this;
        boost::apply_visitor(self, value);
    }
    void operator()(erlcpp::lpid_t const& value)
    {
        throw errors::unsupported_type("pid_not_convert_to_binary");
    }
    void operator()(erlcpp::atom_t const& value)
    {
		if (value == "true")
		{
			gboolean val = TRUE;
			mono_value = mono_value_box(_vm.domain, mono_get_boolean_class(), &val);;
		}
		else if (value == "false")
		{
			gboolean val = FALSE;
			mono_value = mono_value_box(_vm.domain, mono_get_boolean_class(), &val);;
		}
		else if (value == "null")
		{
			mono_value = NULL;
		}
		else
		{
			mono_value = mono_string_new_len(_vm.domain, value.c_str(), value.size());
		}
    }

    void operator()(erlcpp::binary_t const& value)
    {
		mono_value = mono_string_new_len(_vm.domain, value.data(), value.size());
    }

    void operator()(erlcpp::list_t const& value)
    {
    	MonoArray* mono_array = mono_array_new(_vm.domain, mono_get_object_class(), value.size()); 

        erlcpp::list_t::const_iterator i, end = value.end();
		int index = 0;
        for( i = value.begin(); i != end; ++i )
        {
    		mono_dummy_t p;

    		boost::apply_visitor(p, *i);
    		mono_array_setref(mono_array, index, p.mono_value);

			index++;
        }

		mono_value = (MonoObject*)mono_array;
    }

	//放弃tuple数据结构，用tuple来表示hashtable,数目也就必须是2的倍数, 不是的话
    void operator()(erlcpp::tuple_t const& value)
    {
    	if (value.size() % 2 == 0) {
			MonoArray* array = mono_array_new(_vm.domain, mono_get_object_class(), value.size());

       	    for( erlcpp::tuple_t::size_type i = 0, end = value.size(); i != end; i=i+2 )
       	    {
    			mono_dummy_t k(_vm);
    			mono_dummy_t v(_vm);
       	        boost::apply_visitor(k, value[i]);
    			boost::apply_visitor(v, value[i+1]);

				MonoObject* pair = mono_object_new(_vm.domain, _vm.myPairClass);

				if (!pair)
					assert(0);
    			mono_runtime_object_init(pair);

				mono_field_set_value(pair, _vm.key_field, k.mono_value);
				mono_field_set_value(pair, _vm.value_field, v.mono_value);

				mono_array_setref(array, i, pair);
       	    }

			mono_value = (MonoObject*)mono_array;
    	} else {
        	throw errors::unsupported_type("tuple_not_pairs");
    	}
   }

};

/////////////////////////////////////////////////////////////////////////////
//
//

PyObject* term_to_monovalue(vm_t& vm, erlcpp::term_t const& val) {
	mono_dummy_t p(vm);
    boost::apply_visitor(p, val);
	
	return p.mono_value;
}

erlcpp::term_t monovalue_to_term(vm_t& vm, MonoObject* monovalue) {
	MonoClass * cls = mono_object_get_class(monovalue);
	
	if (strcmp(class_name, "Int16") == 0) {
		int32_t p = *(int32_t*)mono_object_unbox(ooo);
		return erlcpp::num_t(value);
	} else if (strcmp(class_name, "Int32") == 0) {
		int32_t p = *(int32_t*)mono_object_unbox(ooo);
		return erlcpp::num_t(value);
	} else if (strcmp(class_name, "Int64") == 0) {

	}


	if (type_name == "int" or type_name == "long") {
		long value = PyInt_AsLong(monovalue);

		if (value == -1) {
			throw errors::unsupported_type("PyInt_AsLong_Fail");
		}

		return erlcpp::num_t(value);

	} else if (type_name == "float") {
		double value = PyFloat_AsDouble(monovalue);

		if (value < 0) {
			throw errors::unsupported_type("PyFloat_AsDouble_Fail");
		}
		return erlcpp::num_t(value);

	} else if (type_name == "str") {
		Py_ssize_t len = PyString_Size(monovalue);
		const char * val = PyString_AsString(monovalue);
		return erlcpp::binary_t(erlcpp::binary_t::data_t(val, val+((int)len)));

	} else if (type_name == "bool") {
		if (monovalue == Py_True) {
			return erlcpp::atom_t("true");
		} else {
			return erlcpp::atom_t("false");
		}
	} else if (type_name == "NoneType") {
		return erlcpp::atom_t("none");

	} else if (type_name == "list") {
		erlcpp::list_t result;
		int list_size = (int)PyList_Size(monovalue);

		for(int32_t index = 0; index < list_size; ++index)
		{
			erlcpp::term_t val = monovalue_to_term(PyList_GetItem(monovalue, index));

			result.push_back(val);
		}
		return result;

	} else if (type_name == "dict") {
		PyObject* keys = PyDict_Keys(monovalue);
		PyObject* values = PyDict_Values(monovalue);	

		Py_ssize_t size = PyList_Size(keys);

		erlcpp::tuple_t result(size * 2);

		for (int32_t index = 0; index < size; ++index) {
			result[2*index] = monovalue_to_term(PyList_GetItem(keys, index));
			result[2*index+1] = monovalue_to_term(PyList_GetItem(values, index));
		}

		Py_DECREF(keys);
		Py_DECREF(values);

		return result;
	} else {
    	throw errors::unsupported_type(tp_name);
	}
}


/////////////////////////////////////////////////////////////////////////////

} // namespace monovm 
