#pragma once

#include "monovm.hpp"
#include "utils.hpp"

namespace monovm {

/////////////////////////////////////////////////////////////////////////////

template <class worker_t>
typename worker_t::result_type
perform_task(vm_t & vm)
{
    vm_t::task_t task = vm.get_task();
    worker_t worker(vm);
    return boost::apply_visitor(worker, task);
}

template <class result_t>
void send_result(vm_t & vm, std::string const& type, result_t const& result)
{
    boost::shared_ptr<ErlNifEnv> env(enif_alloc_env(), enif_free_env);
    erlcpp::tuple_t packet(2);
    packet[0] = erlcpp::atom_t(type);
    packet[1] = result;
    enif_send(NULL, vm.erl_pid().ptr(), env.get(), erlcpp::to_erl(env.get(), packet));
}

template <class result_t>
void send_result_caller(vm_t & vm, std::string const& type, result_t const& result, erlcpp::lpid_t const& caller)
{
    boost::shared_ptr<ErlNifEnv> env(enif_alloc_env(), enif_free_env);
    erlcpp::tuple_t packet(3);
    packet[0] = erlcpp::atom_t(type);
    packet[1] = result;
	packet[2] = caller;
    enif_send(NULL, caller.ptr(), env.get(), erlcpp::to_erl(env.get(), packet));
}

/////////////////////////////////////////////////////////////////////////////

class quit_tag {};


PyObject* term_to_pyvalue(erlcpp::term_t const& val);

erlcpp::term_t pyvalue_to_term(PyObject* pyvalue);

/////////////////////////////////////////////////////////////////////////////

}
