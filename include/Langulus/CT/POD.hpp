///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Abstract.hpp"
#include "Fundamental.hpp"
#include "Array.hpp"
#include <ranges>


namespace Langulus::CTTI
{
   /// Affects CT::POD<T>                                                     
   template<class T>
   struct POD;

   namespace Inner
   {
      /// Some types can be auto-detected as POD                              
      ///   @note is_trivially_destructible_v is required to strenghten the   
      ///      is_trivial_v check on GCC/Clang due to compiler differences;   
      ///   @note std::array will be considered POD if containing POD elements
      ///      so we make sure that ranges are never considered POD by default
      ///      otherwise an array containing one hash will result in a rehash 
      ///      instead of a reuse.                                            
      ///   @note extents are ignored by default                              
      template<class T>
      consteval bool CanItBeConsideredPOD() {
         using DT = ::std::remove_all_extents_t<T>;
         if constexpr (CT::Abstract<DT>)
            return false;
         else {
            return CT::Sparse<DT> or CT::Fundamental<DT> or (
                   ::std::is_trivial_v<DT>
               and ::std::is_standard_layout_v<DT>
               and ::std::is_trivially_destructible_v<DT>
               and not ::std::ranges::range<DT>
            );
         }
      }
   }

   template<class T> requires (Inner::CanItBeConsideredPOD<T>())
   struct POD<T> {};
}

LANGULUS_CTTI_CONCEPT_DECVQE(POD);