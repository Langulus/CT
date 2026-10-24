///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Langulus/Typenav.hpp"
#include <Langulus/CT/Typed.hpp>
//#include <array>


namespace Langulus::CTTI
{
   /// Extends T by marking it as a vector. Examples:                         
   /// 1) template<> struct Vector<YourType> {};                              
   /// 2) struct YourType { using CTTI_Vector = Yup; };                       
   template<class T>
   struct Vector;

   /// Make all custom or standard arrays with extent > 1 be considered vector
   template<class T> requires (AllExtentsOf<T> > 1 and sizeof(T) == sizeof(TypeOf<T>) * AllExtentsOf<T>)
   struct Vector<T> {};

   /// Make ::std::array with extent > 1 be considered vector                 
   template<class T, size_t N> requires (N > 1 and sizeof(T)*N == sizeof(::std::array<T, N>))
   struct Vector<::std::array<T, N>> {};
}

LANGULUS_CTTI_CONCEPT_DECVQ(Vector);