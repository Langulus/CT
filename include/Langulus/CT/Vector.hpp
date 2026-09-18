///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/TypeOf.hpp>
#include <Langulus/IntentOf.hpp>
#include <Langulus/Utils/Byte.hpp>


namespace Langulus::CT
{
   /// Check if all the provided types are considered Vector types.           
   /// Any type that is Typed and has extent that is at least 2, and          
   /// the T's size is exactly equal to sizeof(TypeOf<T>) * AllExtentsOf<T>.  
   /// Additionally, all bounded arrays with more than a single element are   
   /// also considered Vector.                                                
   template<class...T>
   concept Vector = (((Typed<Deint<T>>
            and AllExtentsOf<Deint<T>> > 1
            and sizeof(Deint<T>) == sizeof(TypeOf<Deint<T>>) * AllExtentsOf<Deint<T>>
         ) or (::std::extent_v<Deint<T>> > 1)
      ) and ...);
}