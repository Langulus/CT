///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include <ranges>


namespace Langulus::CTTI
{
   /// Affects CT::Contiguous<T>                                              
   template<class T>
   struct Contiguous;
   
   /// All T that satisfy std::ranges::contiguous_range are covered by default
   template<::std::ranges::contiguous_range T>
   struct Contiguous<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Contiguous);
