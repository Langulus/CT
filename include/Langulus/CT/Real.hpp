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
   /// Affects CT::Real<T>                                                    
   template<class T>
   struct Real;

   /// By default, all T that satisfy std::is_floating_point_v are real       
   template<class T> requires (::std::is_floating_point_v<T>)
   struct Real<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Real);