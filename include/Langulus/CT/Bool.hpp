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
   /// Affects CT::Bool<T>                                                    
   template<class T>
   struct Bool;

   /// Built-in bool always satisfies CT::Bool                                
   template<>
   struct Bool<bool> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Bool);
