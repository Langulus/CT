///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../Utils/Literal.hpp"


namespace Langulus::CTTI
{
   /// Used to define an alternative token for the data definition, as        
   /// opposed to the C++ one.                                                
   /// Types defined with the same token will use the same definition, unless 
   /// they're binary incompatible, in which case a runtime error will occur  
   /// when reflected (if MANAGED_REFLECTION is enabled).                     
   /// Can be used in two ways to satisfy CT::Named<T>:                       
   /// 1. Specialize for T/concept                                            
   /// 2. Add a public `using CTTI_Named = Yes<"DataID">;` in T               
   template<class T>
   struct Named;
}

LANGULUS_CTTI_CONCEPT_DECVQ(Named);