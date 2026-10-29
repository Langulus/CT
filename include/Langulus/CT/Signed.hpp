///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Number.hpp"


namespace Langulus::CTTI
{
   namespace Inner
   {
      /// Test if T is initializable with a signed custom/built-in number     
      ///   @attention CT::Number check is required, because many container   
      ///      possess constructors compatible with F {-1}, and will attempt  
      ///      to construct themselves at compile-time, resulting in errors   
      ///      and false positives.                                           
      template<class T, class F>
      consteval bool SignedInner() {
         if constexpr (CT::Number<T, F>) {
            if constexpr (requires { T {F {-1}} < T {F {0}}; })
               return T {F {-1}} < T {F {0}};
            else
               return false;
         }
         else return false;
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

   template<class T>
   concept SignedInteger8  = Signed<T> and Integer<T> and sizeof(T) == 1;
   template<class T>
   concept SignedInteger16 = Signed<T> and Integer<T> and sizeof(T) == 2;
   template<class T>
   concept SignedInteger32 = Signed<T> and Integer<T> and sizeof(T) == 4;
   template<class T>
   concept SignedInteger64 = Signed<T> and Integer<T> and sizeof(T) == 8;

   template<class T>
   concept UnsignedInteger8  = Unsigned<T> and Integer<T> and sizeof(T) == 1;
   template<class T>
   concept UnsignedInteger16 = Unsigned<T> and Integer<T> and sizeof(T) == 2;
   template<class T>
   concept UnsignedInteger32 = Unsigned<T> and Integer<T> and sizeof(T) == 4;
   template<class T>
   concept UnsignedInteger64 = Unsigned<T> and Integer<T> and sizeof(T) == 8;
}