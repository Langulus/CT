///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Abstract.hpp"
#include "DefineTag.hpp"


namespace Langulus
{
   ///                                                                        
   /// Descriptor intermediate type, used in constructors and assignment      
   /// operators to enable describe-construction/assignment. The inner type   
   /// is always a reference to a type-erased container.                      
   /// You should #include <Langulus/Annies/Many.hpp>                         
   ///        and #include <Langulus/CT/Describable.hpp>                      
   ///        in order to use Describe semantics                              
   struct Describe;
}

namespace Langulus::CT
{
   /// Check if all T are describe-constructible.                             
   /// It has to have the T (Describe&&) constructor in order to be so.       
   template<class...T>
   concept DescribeConstructible = not Abstract<T...>
       and not Enum<T...> and not Aggregate<T...>
       and requires (Describe a) { (T {a}, ...); };
   
   /// Check if all T are describe-assignable.                                
   /// It has to have the T::operator = (Describe&&) constructor.             
   template<class...T>
   concept DescribeAssignable = not Abstract<T...>
       and not Enum<T...> and not Aggregate<T...>
       and requires (T&...lhs, Describe rhs) { ((lhs = rhs), ...); };
}
