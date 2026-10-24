///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "Character.hpp"
#include "Number.hpp"


namespace Langulus
{
   struct Byte;
}

namespace Langulus::CTTI
{
   /// Extends T by marking it as scalar. Scalar is a more permissive number  
   /// type, that also includes bytes and other custom numbers. Examples:     
   /// 1) template<> struct Scalar<YourType> {};                              
   /// 2) struct YourType { using CTTI_Scalar = Yup; };                       
   template<class T>
   struct Scalar;

   /// Any fundamental or custom number type which has AllExtentsOf == 1 is   
   /// considered scalar by default.                                          
   template<class T> requires ((AllExtentsOf<T> == 1 and (//TODO dubious. maybe just not CT::Vector?
      ::std::is_same_v<Langulus::Byte, Decvq<DeextAll<T>>>
      or CT::Number<DeextAll<T>> or CT::Character<DeextAll<T>>
   )))
   struct Scalar<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Scalar);

namespace Langulus::CT
{
   /// Integer scalar                                                         
   template<class...T>
   concept ScalarInt = ((Scalar<T> and Integer<DeextAll<T>>) and ...);
}
