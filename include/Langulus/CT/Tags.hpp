///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "DefineTag.hpp"
#include "../Utils/Types.hpp"


namespace Langulus::CTTI
{
   /// Extends T with tag meta data at compile time. Examples:                
   /// 1) template<> struct Tags<MyData> : Tag1 {};                           
   /// 2) template<> struct Tags<MyData> : Types<Tag1, Tag2, etc...> {};      
   /// 3) struct MyData { using CTTI_Tags = Tag1; };                          
   /// 4) struct MyData { using CTTI_Tags = Types<Tag1, Tag2, etc...>; };     
   template<class>
   struct Tags;
}

namespace Langulus::CT::Inner
{
   /// Helper function to extract reflected tags                              
   template<class T>
   consteval auto GetTags() {
      static_assert(not ::std::is_reference_v<T>,
         "Strip references first");
      static_assert(not CT::Convoluted<T>,
         "Strip qualifiers first");

      using ctti = CTTI::Tags<T>;
      if constexpr (CT::Complete<ctti>) {
         // Checked externally, T doesn't have to be complete           
         if constexpr (CT::Void<ctti>)
            return NoTypes {};
         else {
            if constexpr (CT::DefineTag<ctti>) {
               // Defined as in example 1)                                 
               return Types<ctti> {}; //TODO could be problematic in the future - will produce a different cppname if reflected using that
            }
            else {
               // Defined as in examples 2)                                
               static_assert(CT::Typelist<ctti>,
                  "CTTI::Tags must be defined either as a single trait, "
                  "or as a Types<traits...>"
               );
               return ctti {};
            }
         }
      }
      else {
         // Checked internally, T has to be a complete type             
         static_assert(CT::Complete<T>,
            "Can't access `CTTI_Tags` inside incomplete type");

         if constexpr (requires { typename T::CTTI_Tags; }) {
            using inner = typename T::CTTI_Tags;
            if constexpr (CT::Void<inner>)
               return NoTypes {};
            else {
               if constexpr (CT::DefineTag<inner>) {
                  // Defined as in examples 3)                             
                  return Types<inner> {}; //TODO could be problematic in the future - will produce a different cppname if reflected using that
               }
               else {
                  // Defined as in examples 4)                             
                  static_assert(CT::Typelist<inner>,
                     "T::CTTI_Tags must be defined either as a single trait, "
                     "or as a Types<traits...>"
                  );
                  return inner {};
               }
            }
         }
         else return NoTypes {};
      }
   };
}

namespace Langulus
{
   /// Get the reflected tags, void if none                                   
   template<class T>
   using TagsOf = decltype(CT::Inner::GetTags<Decvq<Deref<T>>>());
}
