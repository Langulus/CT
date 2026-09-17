///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"


namespace Langulus::CTTI
{
   namespace Inner
   {
      /// Test if T is initializable with a signed fundamental                
      template<class T, class F>
      consteval bool SignedInner() {
         if constexpr (requires { T {F {-1}} < T {F {0}}; })
            return T {F {-1}} < T {F {0}};
         else
            return false;
      }
   }

   /// Affects CT::Signed<T>                                                  
   template<class>
   struct Signed;

   /// std::is_signed_v is crap, because it assumes that all types are        
   /// int-initializable. This one is better, because it allows tests for     
   /// float- and double-initializables as well                               
   template<class T> requires (Inner::SignedInner<T, int>()
                            or Inner::SignedInner<T, float>()
                            or Inner::SignedInner<T, double>())
   struct Signed<T> {};
}

namespace Langulus::CT
{
   template<class...T>
   concept Signed = PartialValidate<T...>
       and (LANGULUS_CTTI_CHECK(Decvq<ShedDeref<T>>, Signed) and ...);

   template<class...T>
   concept Unsigned = PartialValidate<T...>
       and ((not Signed<T>) and ...);
}