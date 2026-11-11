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
   /// Extends T by marking it as pinnable at compile-time. Examples:         
   /// 1) template<> struct Pinnable<YourType> {};                            
   /// 2) struct YourType { using CTTI_Pinnable = Yup; };                     
   template<class T>
   struct Pinnable;
}

LANGULUS_CTTI_CONCEPT_DECVQ(Pinnable);
