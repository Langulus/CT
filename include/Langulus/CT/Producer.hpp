///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../Utils/Types.hpp"


namespace Langulus::CTTI
{
   /// Extends T with compile-time meta data about who can produce T at       
   /// runtime. Examples:                                                     
   /// 1) template<> struct Producer<MyData> : Types<T1, T2, etc...> {};      
   /// 2) struct MyData { using CTTI_Producer = T1; };                        
   /// 3) struct MyData { using CTTI_Producer = Types<T1, T2, etc...>; };     
   template<class>
   struct Producer;
}

namespace Langulus::CT::Inner
{
   /// Helper function to extract all associated producers                    
   template<class T>
   consteval auto GetAllProducers() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip qualifiers first");
      using ctti = CTTI::Producer<T>;

      if constexpr (CT::Complete<ctti>) {
         // Checked externally, T doesn't have to be complete           
         if constexpr (CT::Void<ctti>)
            return NoTypes {};
         else
            return ctti {};
      }
      else {
         // Checked internally, T has to be a complete type             
         static_assert(CT::Complete<T>,
            "Can't access `CTTI_Producer` inside incomplete type");

         if constexpr (requires { typename T::CTTI_Producer; }) {
            using inner = typename T::CTTI_Producer;
            if constexpr (CT::Void<inner>)
               return NoTypes {};
            else {
               if constexpr (CT::Typelist<inner>) {
                  // Defined as in examples 3)                          
                  return inner {};
               }
               else {
                  // Defined as in examples 2)                          
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
   /// Get all associated producers                                           
   template<class T>
   using ProducersOf = decltype(CT::Inner::GetAllProducers<Decvq<Deref<T>>>());
}

namespace Langulus::CT
{
   /// Check if all types are producible from other                           
   template<class...T>
   concept Producible = ((not ProducersOf<T>::Empty) and ...);

   /// Check if all types are not producible from other                       
   template<class...T>
   concept NotProducible = ((not Producible<T>) and ...);
}