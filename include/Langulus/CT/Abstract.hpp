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
   /// Affects CT::Abstract<T>                                                
   template<class T>
   struct Abstract;

   /// Built-in abstract types are always CT::Abstract                        
   template<class T> requires (::std::is_abstract_v<T>)
   struct Abstract<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Abstract);
