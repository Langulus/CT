///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <type_traits>


namespace Langulus
{
   /// Align a value to a given alignment                                     
   template<class T, class A>
   constexpr T Align(T valueToAlign, A alignment) {
      if constexpr (::std::is_pointer_v<T>) {
         const uintptr_t align = static_cast<uintptr_t>(alignment); 
         const uintptr_t as_bytes = reinterpret_cast<uintptr_t>(valueToAlign);
         const uintptr_t r = as_bytes % align;
         return reinterpret_cast<T>(r ? as_bytes + (align - r) : as_bytes);         
      }
      else {
         const T align = static_cast<T>(alignment); 
         const T r = valueToAlign % align;
         return r ? valueToAlign + (align - r) : valueToAlign;
      }
   }
}