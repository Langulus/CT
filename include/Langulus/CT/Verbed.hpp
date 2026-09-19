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
   /// Extends T with verb meta data at compile time. Examples:               
   /// 1) template<> struct Verbed<MyData> : Verb1 {};                        
   /// 2) template<> struct Verbed<MyData> : Types<Verb1, Verb2, etc...> {};  
   /// 3) struct MyData { using CTTI_Verbed = Verb1; };                       
   /// 4) struct MyData { using CTTI_Verbed = Types<Verb1, Verb2, etc...>; }; 
   ///   @attention this isn't the same as reflecting abilities. It has more  
   ///      to do with reflecting verb IDs in statically-verbed containers.   
   template<class>
   struct Verbed;
}

namespace Langulus::CT::Inner
{
   /// Helper function to extract all associated verbs                        
   template<class T>
   consteval auto GetAllVerbs() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip qualifiers first");
      using ctti = CTTI::Verbed<T>;

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
               return Types<typename ctti::ConsistentNamedVerbTypeEvenIfInherited> {};
            }
         }
      }
      else {
         // Checked internally, T has to be a complete type             
         static_assert(CT::Complete<T>,
            "Can't access `CTTI_Verbed` inside incomplete type");

         if constexpr (requires { typename T::CTTI_Verbed; }) {
            using inner = typename T::CTTI_Verbed;
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

   /// Helper function to extract specific verb                               
   ///   @tparam T the type to inspect                                        
   ///   @tparam INDEX the verb you desire. Will scream a compile-time error  
   ///      at you if you go out of the defined range.                        
   ///   @return the NamedVerb definition                                     
   template<class T, size_t INDEX>
   consteval auto GetSpecificVerb() {
      return typename decltype(GetAllVerbs<T>())::template At<INDEX> {};
   };
}

namespace Langulus
{
   /// Get all associated verbs                                               
   template<class T>
   using VerbsOf = decltype(CT::Inner::GetAllVerbs<Decvq<Deref<T>>>());

   /// Get a specific verb                                                    
   template<class T, size_t INDEX = 0>
   using VerbOf = decltype(CT::Inner::GetSpecificVerb<Decvq<Deref<T>>, INDEX>());
}
