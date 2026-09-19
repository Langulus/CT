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
   /// 1) template<> struct Abilities<MyData> : Verb1 {};                     
   /// 2) template<> struct Abilities<MyData> : Types<Verb1, etc...> {};      
   /// 3) struct MyData { using CTTI_Abilities = Verb1; };                    
   /// 4) struct MyData { using CTTI_Abilities = Types<Verb1, etc...>; };     
   template<class>
   struct Abilities;
}

namespace Langulus::CT::Inner
{
   /// Helper function to extract all associated abilities                    
   template<class T>
   consteval auto GetAllAbilities() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip qualifiers first");
      using ctti = CTTI::Abilities<T>;

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
            "Can't access `CTTI_Abilities` inside incomplete type");

         if constexpr (requires { typename T::CTTI_Abilities; }) {
            using inner = typename T::CTTI_Abilities;
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
}

namespace Langulus
{
   /// Get all associated abilities                                           
   template<class T>
   using AbilitiesOf = decltype(CT::Inner::GetAllAbilities<Decvq<Deref<T>>>());
}
