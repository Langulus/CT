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
   /// Extends T by marking it as a meta at compile-time. Used for DMeta,     
   /// TMeta, CMeta, and VMeta. Examples:                                     
   /// 1) template<> struct Meta<YourType> {};                                
   /// 2) struct YourType { using CTTI_Meta = Yup; };                         
   template<class T>
   struct Meta;
}

LANGULUS_CTTI_CONCEPT_DECVQ(Meta);
