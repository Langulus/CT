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
   /// Makes the given type a tag. Examples:                                  
   /// 1) template<> struct DefineTag<X> : Yes<"x"> {};                       
   /// 2) struct X { using CTTI_DefineTag = Yes<"x">; };                      
   template<class T>
   struct DefineTag;
}

namespace Langulus::RTTI
{
   /// Get the name of a tag definition at compile-time                       
   ///   @tparam T the type to get the info of                                
   ///   @return a compile-time string                                        
   template<class T>
   consteval auto NameOfTag() {
      constexpr auto tag = LANGULUS_CTTI_CHECK_EXTRACT(T, DefineTag, Literal {});
      static_assert(IsASCII(tag), "Tag name must be ASCII");
      static_assert(tag == "" or IsAlphabetical(tag[0]),
         "Tag name must begin with an alphabetical symbol");
      return tag;
   }
}

namespace Langulus::CT
{
   /// Checks if Ts are tag definitions                                       
   template<class...T>
   concept DefineTag = ((RTTI::NameOfTag<T>() != "") and ...);

   /// Checks if Ts are not tag definitions                                   
   template<class...T>
   concept NotDefineTag = ((not DefineTag<T>) and ...);
}