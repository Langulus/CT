///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"


namespace Langulus::CTTI
{
   /// Affects CT::Unallocatable<T>                                           
   template<class T>
   struct Unallocatable;

   /// Void types are non-allocatable by default                              
   template<CT::Void T>
   struct Unallocatable<T> : Yup {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Unallocatable);
