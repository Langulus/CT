///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../Utils/Types.hpp"


namespace Langulus::CTTI
{
   /// Extends T with tag meta data at compile time. Examples:                
   /// 1) template<> struct Tagged<MyData> : Tag1 {};                         
   /// 2) template<> struct Tagged<MyData> : Types<Tag1, Tag2, etc...> {};    
   /// 3) struct MyData { using CTTI_Tagged = Tag1; };                        
   /// 4) struct MyData { using CTTI_Tagged = Types<Tag1, Tag2, etc...>; };   
   ///   @attention this isn't the same as reflecting tags. It has more       
   ///      to do with reflecting tag IDs in statically-tagged containers.    
   template<class>
   struct Tagged;
}

namespace Langulus::CT::Inner
{
   /// Helper function to extract all associated tags                         
   template<class T>
   consteval auto GetAllTags() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip qualifiers first");
      using ctti = CTTI::Tagged<T>;

      if constexpr (CT::Complete<ctti>) {
         // Checked externally, T doesn't have to be complete           
         if constexpr (CT::Void<ctti>)
            return NoTypes {};
         else {
            if constexpr (CT::Typelist<ctti>) {
               // Defined as in examples 2)                             
               return ctti {};
            }
            else {
               // Defined as in example 1)                              
               return Types<typename ctti::ConsistentNamedTagTypeEvenIfInherited> {};
            }
         }
      }
      else {
         // Checked internally, T has to be a complete type             
         static_assert(CT::Complete<T>,
            "Can't access `CTTI_Tagged` inside incomplete type");

         if constexpr (requires { typename T::CTTI_Tagged; }) {
            using inner = typename T::CTTI_Tagged;
            if constexpr (CT::Void<inner>)
               return NoTypes {};
            else {
               if constexpr (CT::Typelist<inner>) {
                  // Defined as in examples 4)                          
                  return inner {};
               }
               else {
                  // Defined as in examples 3)                          
                  return Types<inner> {};
               }
            }
         }
         else return NoTypes {};
      }
   };

   /// Helper function to extract specific tag                                
   ///   @tparam T the type to inspect                                        
   ///   @tparam INDEX the tag you desire. Will scream a compile-time error   
   ///      at you if you go out of the defined range.                        
   ///   @return the NamedTag definition                                      
   template<class T, size_t INDEX>
   consteval auto GetSpecificTag() {
      return typename decltype(GetAllTags<T>())::template At<INDEX> {};
   };
}

namespace Langulus
{
   /// Get all associated tags                                                
   template<class T>
   using TagsOf = decltype(CT::Inner::GetAllTags<Decvq<Deref<T>>>());

   /// Get a specific tag                                                     
   template<class T, size_t INDEX = 0>
   using TagOf = decltype(CT::Inner::GetSpecificTag<Decvq<Deref<T>>, INDEX>());
}
