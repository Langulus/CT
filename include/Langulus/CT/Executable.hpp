///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"


namespace Langulus::Flow
{
   /// Predeclaration of type-erased function container                       
   struct Verb;
}

namespace Langulus::CTTI
{
   /// Extends T by marking it as executable. Examples:                       
   /// 1) template<> struct Executable<YourType> {};                          
   /// 2) struct YourType { using CTTI_Executable = Yup; };                   
   template<class T>
   struct Executable;

   /// Type-erased verbs are always marked executable                         
   template<>
   struct Executable<::Langulus::Flow::Verb> {};
}

namespace Langulus::CT
{
   /// Checks whether all decayed T are marked as executable                  
   template<class...T>
   concept Executable = Validate<Decay<T>...>
       and (LANGULUS_CTTI_CHECK(Decay<T>, Executable) and ...);

  /// Checks whether all decayed T are not marked as executable               
   template<class...T>
   concept NotExecutable = Validate<Decay<T>...>
       and ((not Executable<T>) and ...);
}