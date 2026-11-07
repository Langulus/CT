///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Complete.hpp"


namespace Langulus::Inner
{
   /// You can customize this call by:                                        
   /// `template<class...WITH> using Retype = <your way of retyping>;`        
   /// inside your decayed T.                                                 
   ///   @tparam T type we're retyping                                        
   ///   @tparam WITH what are we retyping with                               
   ///   @return type identity with the retyped T, all references, qualifiers 
   ///      and indirections preserved.                                       
   template<class T, class...WITH>
   consteval auto RetypeNested() {
      if constexpr (::std::is_const_v<T>)
         return ::std::type_identity<typename decltype(RetypeNested<::std::remove_const_t<T>, WITH...>())::type const> {};
      else if constexpr (::std::is_volatile_v<T>)
         return ::std::type_identity<typename decltype(RetypeNested<::std::remove_volatile_t<T>, WITH...>())::type volatile> {};
      else if constexpr (::std::is_rvalue_reference_v<T>)
         return ::std::type_identity<typename decltype(RetypeNested<::std::remove_reference_t<T>, WITH...>())::type&&> {};
      else if constexpr (::std::is_lvalue_reference_v<T>)
         return ::std::type_identity<typename decltype(RetypeNested<::std::remove_reference_t<T>, WITH...>())::type&> {};
      else if constexpr (::std::is_pointer_v<T>)
         return ::std::type_identity<typename decltype(RetypeNested<::std::remove_pointer_t<T>, WITH...>())::type*> {};
      else if constexpr (::std::is_bounded_array_v<T>)
         return ::std::type_identity<typename decltype(RetypeNested<::std::remove_extent_t<T>, WITH...>())::type [::std::extent_v<T>]> {};
      else if constexpr (CT::Complete<T>) {
         if constexpr (requires { typename T::template Retype<WITH...>; })
            return ::std::type_identity<typename T::template Retype<WITH...>> {};
         else
            return ::std::type_identity<WITH...> {};
      }
      else return ::std::type_identity<WITH...> {};
   }
}

namespace Langulus
{
   /// Change the inner type of a CT::Typed, pointer, array, etc.             
   template<class WHAT, class...WITH>
   using Retype = typename decltype(Inner::RetypeNested<WHAT, WITH...>())::type;
}
