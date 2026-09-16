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
   /// By default, all T that satisfy std::is_floating_point_v are real       
   template<class T>
   struct Real {
      static constexpr bool Default = true;
      static constexpr bool Enabled = ::std::is_floating_point_v<T>;
   };

   template<>
   struct Real<float> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Real);

static_assert(Langulus::CT::Real<float>);
static_assert(Langulus::CT::Real<double>);