
#ifndef DYN_DETAIL_BOOST_MPL_AUX_ARITY_HPP_INCLUDED
#define DYN_DETAIL_BOOST_MPL_AUX_ARITY_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: arity.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-11 02:19:02 -0400 (Sat, 11 Oct 2008) $
// $Revision: 49267 $

#include <dyn_detail/boost/mpl/aux_/config/dtp.hpp>

#if defined(DYN_DETAIL_BOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)

#   include <dyn_detail/boost/mpl/aux_/nttp_decl.hpp>
#   include <dyn_detail/boost/mpl/aux_/config/static_constant.hpp>

namespace dyn_detail {
namespace boost { namespace mpl { namespace aux {

// agurt, 15/mar/02: it's possible to implement the template so that it will 
// "just work" and do not require any specialization, but not on the compilers
// that require the arity workaround in the first place
template< typename F, DYN_DETAIL_BOOST_MPL_AUX_NTTP_DECL(int, N) >
struct arity
{
    DYN_DETAIL_BOOST_STATIC_CONSTANT(int, value = N);
};

}}}
}

#endif // DYN_DETAIL_BOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES

#endif // DYN_DETAIL_BOOST_MPL_AUX_ARITY_HPP_INCLUDED
