///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"


namespace Langulus::Annies
{
   /// Predeclaration of type-erased function container                       
   template<class> 
   struct TVerb;
   using Verb = TVerb<void>;
}

namespace Langulus::CTTI
{
   /// Extends T by marking it as executable. Examples:                       
   /// 1) template<> struct Executable<YourType> {};                          
   /// 2) struct YourType { using CTTI_Executable = Yup; };                   
   template<class T>
   struct Executable;

   /// All C++ function signatures are marked executable                      
   template<class T> requires ::std::is_function_v<T>
   struct Executable<T> {};

   /// All verbs are marked executable                                        
   template<class T>
   struct Executable<::Langulus::Annies::TVerb<T>> {};
}

LANGULUS_CTTI_CONCEPT_DECAY(Executable)