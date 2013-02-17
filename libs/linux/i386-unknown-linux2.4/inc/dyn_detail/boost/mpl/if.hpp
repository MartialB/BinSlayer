
#ifndef DYN_DETAIL_BOOST_MPL_IF_HPP_INCLUDED
#define DYN_DETAIL_BOOST_MPL_IF_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: if.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-11 02:19:02 -0400 (Sat, 11 Oct 2008) $
// $Revision: 49267 $

#include <dyn_detail/boost/mpl/aux_/value_wknd.hpp>
#include <dyn_detail/boost/mpl/aux_/static_cast.hpp>
#include <dyn_detail/boost/mpl/aux_/na_spec.hpp>
#include <dyn_detail/boost/mpl/aux_/lambda_support.hpp>
#include <dyn_detail/boost/mpl/aux_/config/integral.hpp>
#include <dyn_detail/boost/mpl/aux_/config/ctps.hpp>
#include <dyn_detail/boost/mpl/aux_/config/workaround.hpp>

namespace dyn_detail {
namespace boost { namespace mpl {

#if !defined(DYN_DETAIL_BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<
      bool C
    , typename T1
    , typename T2
    >
struct if_c
{
    typedef T1 type;
};

template<
      typename T1
    , typename T2
    >
struct if_c<false,T1,T2>
{
    typedef T2 type;
};

// agurt, 05/sep/04: nondescriptive parameter names for the sake of DigitalMars
// (and possibly MWCW < 8.0); see http://article.gmane.org/gmane.comp.lib.boost.devel/108959
template<
      typename DYN_DETAIL_BOOST_MPL_AUX_NA_PARAM(T1)
    , typename DYN_DETAIL_BOOST_MPL_AUX_NA_PARAM(T2)
    , typename DYN_DETAIL_BOOST_MPL_AUX_NA_PARAM(T3)
    >
struct if_
{
 private:
    // agurt, 02/jan/03: two-step 'type' definition for the sake of aCC 
    typedef if_c<
#if defined(DYN_DETAIL_BOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS)
          DYN_DETAIL_BOOST_MPL_AUX_VALUE_WKND(T1)::value
#else
          DYN_DETAIL_BOOST_MPL_AUX_STATIC_CAST(bool, DYN_DETAIL_BOOST_MPL_AUX_VALUE_WKND(T1)::value)
#endif
        , T2
        , T3
        > almost_type_;
 
 public:
    typedef typename almost_type_::type type;
    
    DYN_DETAIL_BOOST_MPL_AUX_LAMBDA_SUPPORT(3,if_,(T1,T2,T3))
};

#else

// no partial class template specialization

namespace aux {

template< bool C >
struct if_impl
{
    template< typename T1, typename T2 > struct result_
    {
        typedef T1 type;
    };
};

template<>
struct if_impl<false>
{
    template< typename T1, typename T2 > struct result_
    { 
        typedef T2 type;
    };
};

} // namespace aux

template<
      bool C_
    , typename T1
    , typename T2
    >
struct if_c
{
    typedef typename aux::if_impl< C_ >
        ::template result_<T1,T2>::type type;
};

// (almost) copy & paste in order to save one more 
// recursively nested template instantiation to user
template<
      typename DYN_DETAIL_BOOST_MPL_AUX_NA_PARAM(C_)
    , typename DYN_DETAIL_BOOST_MPL_AUX_NA_PARAM(T1)
    , typename DYN_DETAIL_BOOST_MPL_AUX_NA_PARAM(T2)
    >
struct if_
{
    enum { msvc_wknd_ = DYN_DETAIL_BOOST_MPL_AUX_MSVC_VALUE_WKND(C_)::value };

    typedef typename aux::if_impl< DYN_DETAIL_BOOST_MPL_AUX_STATIC_CAST(bool, msvc_wknd_) >
        ::template result_<T1,T2>::type type;

    DYN_DETAIL_BOOST_MPL_AUX_LAMBDA_SUPPORT(3,if_,(C_,T1,T2))
};

#endif // DYN_DETAIL_BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

DYN_DETAIL_BOOST_MPL_AUX_NA_SPEC(3, if_)

}}
}
#endif // DYN_DETAIL_BOOST_MPL_IF_HPP_INCLUDED
