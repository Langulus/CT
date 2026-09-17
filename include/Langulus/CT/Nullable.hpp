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
   /// Affects CT::Nullable<T>                                                
   template<class T>
   struct Nullable;

   /// Pointers and fundamentals are nullable by default                      
   template<class T> requires (::std::is_pointer_v<T> or ::std::is_fundamental_v<T>)
   struct Nullable<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Nullable);
