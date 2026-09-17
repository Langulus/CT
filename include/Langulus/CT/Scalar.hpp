///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Character.hpp"
#include "Number.hpp"

namespace Langulus
{
   struct Byte;
}

namespace Langulus::CT
{
   /// Scalar concept - any fundamental or custom number type, regardless     
   /// if wrapped inside an intent. Bounded arrays of those with              
   /// ExtentOf == 1 are also considered scalars.                             
   template<class...T>
   concept Scalar = ((AllExtentsOf<T> == 1 and (
            ::std::is_same_v<Langulus::Byte, Decvq<DeextAll<T>>>
         or Number<DeextAll<T>>
         or Character<DeextAll<T>>
      )) and ...);
}