///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Complete.hpp"


namespace Langulus::CTTI
{
   /// Affects CT::Void<T>                                                    
   template<class>
   struct Void;

   /// Make sure no one interferes with true void type                        
   template<>
   struct Void<void> {};

   /// Void type identity is still considered void                            
   template<>
   struct Void<::std::type_identity<void>> {};

   /// True/false helper types are considered void                            
   template<>
   struct Void<Yup> {};
   template<bool CONDITION>
   struct Void<Maybe<CONDITION>> {};
   template<>
   struct Void<No> {};
   template<>
   struct Void<::std::true_type> {};
   template<>
   struct Void<::std::false_type> {};
}

namespace Langulus::CT::Inner
{
   template<class T>
   consteval bool IsVoidInner() {
      using DT = ::std::remove_cvref_t<T>;
      if constexpr (Complete<CTTI::Void<DT>>) {
         // External check                                              
         return true;
      }
      else {
         // Internal check                                              
         static_assert(Complete<DT>,
            "Can't check if an incomplete type is void");

         if constexpr (requires { DT::CTTI_Void::Enabled; })
            return DT::CTTI_Void::Enabled;
         else
            return false;
      }
   }
}

namespace Langulus::CT
{
   /// Check if all T are marked void                                         
   template<class...T>
   concept Void = (Inner::IsVoidInner<T>() and ...);

   template<class...T>
   concept NotVoid = ((not Void<T>) and ...);
}