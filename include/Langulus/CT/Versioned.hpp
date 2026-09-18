///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"


namespace Langulus
{
   /// Useful for setting CTTI_Versioned                                      
   template<unsigned MAJOR = 0, unsigned MINOR = 0>
   struct Version {
      static constexpr unsigned Major   = MAJOR;
      static constexpr unsigned Minor   = MINOR;
      static constexpr bool     Enabled = true;
   };
}

namespace Langulus::CTTI
{
   /// Extends T with a version meta data at compile time                     
   /// Examples:                                                              
   /// 1) template<> struct Versioned<YourType> : Version<2, 5> {};           
   /// 2) struct YourType { using CTTI_Versioned = Version<2, 5>; };          
   template<class T>
   struct Versioned;

   /// Extends constant E with a version meta data at compile time            
   /// Examples:                                                              
   /// 1) template<> struct VersionedValue<E> : Version<2, 5> {};             
   template<auto E>
   struct VersionedValue;
}

namespace Langulus
{
   /// Get the version of a given type T                                      
   ///   @return No if version hasn't been defined                            
   template<class T>
   consteval auto VersionOf() {
      using DT = Shed<T>;
      using ctti = CTTI::Versioned<DT>;
      if constexpr (CT::Complete<ctti>) {
         if constexpr (ctti::Enabled) {
            static_assert(requires { ctti::Major; },
               "Wrongly specialized `CTTI::Versioned`");  
            return ctti {};
         }
         else return Version<> {};
      }
      else {
         static_assert(CT::Complete<DT>,
            "Can't access `CTTI_Versioned` inside incomplete type");

         if constexpr (requires { typename DT::CTTI_Versioned; }) {
            using inner = typename DT::CTTI_Versioned;
            if constexpr (inner::Enabled) {
               static_assert(requires { inner::Major; },
                  "Wrongly specialized `T::CTTI_Versioned`");  
               return inner {};
            }
            else return Version<> {};
         }
         else return Version<> {};
      }
   }

   /// Get the version of a given constant E                                  
   ///   @return No if version hasn't been defined                            
   template<auto E>
   consteval auto VersionOf() {
      using ctti = CTTI::VersionedValue<E>;
      if constexpr (CT::Complete<ctti>) {
         if constexpr (ctti::Enabled) {
            static_assert(requires { ctti::Major; },
               "Wrongly specialized `CTTI::VersionedValue`");  
            return ctti {};
         }
         else return Version<> {};
      }
      else return Version<> {};
   }
}
