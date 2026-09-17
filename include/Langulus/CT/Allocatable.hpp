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
   /// Affects CT::Allocatable<T>                                             
   template<class T>
   struct Allocatable;

   /// Non-void types are allocatable by default                              
   template<CT::NotVoid T>
   struct Allocatable<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Allocatable);
