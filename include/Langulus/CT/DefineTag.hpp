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


namespace Langulus
{
   /// A helper structure for reflecting a tag                                
   template<Literal TOKEN>
   struct NamedTag {
      using CTTI_ReflectAs = void;
      using ConsistentNamedTagTypeEvenIfInherited = NamedTag;
      static constexpr bool Enabled  = true;
      static constexpr auto Token    = TOKEN;
   };
}

namespace Langulus::CTTI
{
   /// Makes the given type a tag meta definition. Examples:                  
   /// 1) template<> struct DefineTag<X> : NamedTag<"x"> {};                  
   /// 2) struct X { using CTTI_DefineTag = NamedTag<"x">; };                 
   template<class T>
   struct DefineTag;
}

namespace Langulus::CT::Inner
{
   /// Get the definition of a tag at compile-time                            
   ///   @tparam T the tag to get the info of                                 
   ///   @return a NamedTag if tag was defined, or No otherwise               
   template<class T>
   consteval auto DefinitionOfTag() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip constness first");
      using ctti = CTTI::DefineTag<T>;

      if constexpr (CT::Complete<ctti>) {
         // Tag was defined externally                                  
         return typename ctti::ConsistentNamedTagTypeEvenIfInherited {};
      }
      else if constexpr (::std::is_class_v<T>) {
         // Tag was defined internally                                  
         static_assert(CT::Complete<T>,
            "Can't access CTTI_DefineTag in incomplete type");

         if constexpr (requires { typename T::CTTI_DefineTag; }) {
            using inner = typename T::CTTI_DefineTag;
            if constexpr (CT::Void<inner>)
               return No {};
            else 
               return inner {};
         }
         else return No {};
      }
      else return No {};
   }
   
   /// Get a custom token of a tag at compile-time, if such was defined       
   ///   @tparam T the tag to get the info of                                 
   ///   @return a compile-time string                                        
   template<class T>
   consteval auto CustomNameOfTag() {
      constexpr auto definition = DefinitionOfTag<T>();
      if constexpr (::std::is_same_v<decltype(definition), No const>)
         return Langulus::Literal {};
      else {
         constexpr auto c = definition.Token;
         static_assert(IsASCII(c), "Tag name must be ASCII");
         static_assert(c == "" or IsAlphabetical(c[0]),
            "Tag name must begin with an alphabetical symbol");
         return c;
      }
   }
}

namespace Langulus::CT
{
   /// Checks if all E are defined constants                                  
   template<class...T>
   concept DefineTag = ((not ::std::is_same_v<No, decltype(Inner::DefinitionOfTag<Decvq<Deref<T>>>())>) and ...);

   /// Checks if all E are not defined constants                              
   template<class...T>
   concept NotDefineTag = ((not DefineTag<T>) and ...);
}

namespace Langulus
{
   /// Get the name of a tag definition, if it exists                         
   template<class T>
   constexpr auto NameOfTag = CT::Inner::CustomNameOfTag<T>();
}