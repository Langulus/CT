///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../Utils/Literal.hpp"


namespace Langulus::CTTI
{
   /// Extends T with an info meta data at compile time                       
   /// Examples:                                                              
   /// 1) template<> struct Info<YourType> : Yes<"Some info string"> {};      
   /// 2) struct YourType { using CTTI_Info = Yes<"Some info string">; };     
   template<class T>
   struct Info;

   /// Extends constant E with a info meta data at compile time               
   /// Examples:                                                              
   /// 1) template<> struct InfoValue<E> : Yes<"Some info string"> {};        
   template<auto E>
   struct InfoValue;
}

namespace Langulus
{
   /// Get the info for a type at compile-time                                
   ///   @tparam T the type to get the info of                                
   ///   @return a compile-time string                                        
   //TODO check if tested, cuz i found some mistakes
   template<class T>
   consteval auto InfoOf() {
      using DT = Decvq<Deref<T>>;
      return LANGULUS_CTTI_CHECK_EXTRACT(DT, Info, Literal {});
   }
   
   /// Get the info for a constant at compile-time                            
   ///   @tparam E the constant to get the info of                            
   ///   @return a compile-time string                                        
   //TODO check if tested, cuz i found some mistakes
   template<auto E>
   consteval auto InfoOf() {
      using ctti = CTTI::InfoValue<E>;
      if constexpr (CT::Complete<ctti>) {
         LANGULUS_CTTI_DELVE_IN_EXTRACT(ctti, Literal {});
      }
      else return Literal {};
   }
}
