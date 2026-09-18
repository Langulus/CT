///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
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

   /// All T satisfying std::integer are CT::Integer by default, with the     
   /// exception of custom and built-in boolean types.                        
   template<class T> requires (::std::is_integral_v<T> and not CT::Bool<T>)
   struct Integer<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Integer);
