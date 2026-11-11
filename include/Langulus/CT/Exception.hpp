///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Typenav.hpp>
#include <Langulus/Except.hpp>


namespace Langulus::CTTI
{
   /// Affects CT::Exception<T>                                               
   template<class T>
   struct Exception;

   /// Built-in exception                                                     
   template<>
   struct Exception<Langulus::Exception> {};

   //TODO maybe add standard ones as well?
}

LANGULUS_CTTI_CONCEPT_DECVQ(Exception);
