///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Bool.hpp"


namespace Langulus::CTTI
{
   /// Affects CT::Integer<T>                                                 
   template<class T>
   struct Integer;

   /// All T satisfying std::is_integral_v are CT::Integer by default, with   
   /// the exception of custom and built-in boolean types.                    
   template<class T> requires (::std::is_integral_v<T> and not CT::Bool<T>)
   struct Integer<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Integer);

namespace Langulus::CT
{
   template<class T>
   concept Integer8  = Integer<T> and sizeof(T) == 1;
   template<class T>
   concept Integer16 = Integer<T> and sizeof(T) == 2;
   template<class T>
   concept Integer32 = Integer<T> and sizeof(T) == 4;
   template<class T>
   concept Integer64 = Integer<T> and sizeof(T) == 8;
}