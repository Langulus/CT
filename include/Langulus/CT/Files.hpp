///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../Utils/Literal.hpp"


namespace Langulus::CTTI
{
   /// Extends T with a file extension meta data at compile time              
   /// Examples:                                                              
   /// 1) template<> struct Files<YourType> : Yes<"txt,rtf,etc"> {};          
   /// 2) struct YourType { using CTTI_Files = Yes<"txt,rtf,etc">; };         
   template<class T>
   struct Files;
}

namespace Langulus
{
   /// Get the file extensions for a type at compile-time                     
   ///   @tparam T the type to get the info of                                
   ///   @return a compile-time string                                        
   //TODO check if tested, cuz i found some mistakes
   template<class T>
   consteval auto FilesOf() {
      using DT = Decvq<Deref<T>>;
      constexpr auto files = LANGULUS_CTTI_CHECK_EXTRACT(DT, Files, Literal {});
      static_assert(IsASCII(files), "File extensions must be ASCII");
      return files;
   }
}
