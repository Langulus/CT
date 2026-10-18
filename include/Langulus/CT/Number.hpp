///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Integer.hpp"
#include "Real.hpp"


namespace Langulus::CTTI
{
   /// Extends T by marking it as a number. Examples:                         
   /// 1) template<> struct Number<YourType> {};                              
   /// 2) struct YourType { using CTTI_Number = Yup; };                       
   /// @attention bool types are not considered numbers                       
   template<class T>
   struct Number;

   /// All custom/built-in integers and real numbers are CT::Number           
   template<class T> requires (CT::Integer<T> or CT::Real<T>)
   struct Number<T> {};

   /// Extends T by marking it as a custom number. CT::CustomNumber is proper 
   /// subset of CT::Number. Examples:                                        
   /// 1) template<> struct CustomNumber<YourType> {};                        
   /// 2) struct YourType { using CTTI_CustomNumber = Yup; };                 
   template<class T>
   struct CustomNumber;
}

LANGULUS_CTTI_CONCEPT_DECVQ(Number);
LANGULUS_CTTI_CONCEPT_DECVQ(CustomNumber);

namespace Langulus::CT
{
   /// C++ is notorious with its ambiguity between int8_t and char. You can   
   /// never derive clear intent from using those, because they are declared  
   /// the same way: are we talking about number semantics, or character      
   /// semantics? Who knows! No idea who decided that's a good ontological    
   /// commitment, but we bear that load on our shoulders since the dawn of   
   /// time. So you can use this concept to exclude `char` from the set of    
   /// number types.                                                          
   template<class...T>
   concept NumberUnambiguously = ((Number<T>
       and not ::std::is_same_v<ShedDeref<T>, char>) and ...);

   /// Built-in number                                                        
   template<class...T>
   concept BuiltinNumber = ((Number<T> and not CustomNumber<T>) and ...);

   /// Unambiguous built-in number                                            
   template<class...T>
   concept BuiltinNumberUnambiguously = ((NumberUnambiguously<T> and not CustomNumber<T>) and ...);
}