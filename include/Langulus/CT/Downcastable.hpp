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
   struct Any;
}

namespace Langulus::CTTI
{
   /// Can be used in two ways to satisfy CT::Downcastable<T>:                
   /// 1. Specialize for T/concept                                            
   /// 2. Add a public `using CTTI_Downcastable = Yup;` in T                  
   /// Either way, the type also has to have the required interface for this  
   /// to work, so just inherit from Langulus::Downcastable. By default, all  
   /// types are not CT::Downcastable.                                        
   template<class T>
   struct Downcastable;
}

LANGULUS_CTTI_CONCEPT_DECVQ(Downcastable);
