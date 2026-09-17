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
   /// Affects CT::Character<T>                                               
   template<class T>
   struct Character;
   
   /// Built-in character types always satisfy CT::Character                  
   template<> struct Character<char>{};
   template<> struct Character<wchar_t>{};
   template<> struct Character<char8_t>{};
   template<> struct Character<char16_t>{};
   template<> struct Character<char32_t>{};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Character);
