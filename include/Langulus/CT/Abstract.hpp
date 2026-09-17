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
   struct Abstract {
      static constexpr bool Default = true;
      static constexpr bool Enabled = ::std::is_abstract_v<T>;
   };
}

LANGULUS_CTTI_CONCEPT_DECVQ(Abstract);
