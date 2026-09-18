///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../Utils/Roof.hpp"


namespace Langulus::CTTI
{
   /// Extends T with a minimal allocation meta data at compile time          
   /// Examples:                                                              
   /// 1) template<> struct MinAlloc<YourType> : Yes<512> {};                 
   /// 2) struct YourType { using CTTI_MinAlloc = Yes<512>; };                
   template<class T>
   struct MinAlloc;
}

namespace Langulus
{
   /// Get the minimal allocation for a type at compile-time                  
   ///   @tparam T the type to get the info of                                
   ///   @return a compile-time value                                         
   template<class T>
   consteval auto MinAllocOf() {
      using ST = Shed<T>;
      constexpr size_t minalloc = Roof2(LANGULUS_CTTI_CHECK_EXTRACT(ST, MinAlloc, MinimalAllocation));
      static_assert(minalloc >= sizeof(ST),
         "MinAlloc can't be smaller than the size of T");
      static_assert(::std::has_single_bit(minalloc),
         "MinAlloc must be a power-of-two");
      return minalloc < MinimalAllocation ? MinimalAllocation : minalloc;
   }
}
