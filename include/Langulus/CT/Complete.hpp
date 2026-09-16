///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <type_traits>


namespace Langulus::CT
{
   template<class T, bool PROBE = requires(T) { sizeof(T); }>
   constexpr bool Complete = PROBE;

   namespace Inner
   {
      template<class...T>
      consteval bool ValidateInner() {
         static_assert(sizeof...(T) > 0, "No arguments provided");
         static_assert(((Complete<T> or ::std::is_void_v<T>) and ...),
            "Incomplete type in CT check");
         return true;
      }

      template<class...T>
      consteval bool PartialValidateInner() {
         static_assert(sizeof...(T) > 0, "No arguments provided");
         return true;
      }
   }

   /// Makes sure an error is reported if a CT concept is tested without      
   /// any arguments, or if any argument is an incomplete type, so that       
   /// failures aren't silent.                                                
   ///   @attention 'void' is not considered incomplete in this context       
   template<class...T>
   concept Validate = Inner::ValidateInner<T...>();

   /// Makes sure an error is reported if a CT concept is tested without      
   /// any arguments, so failures aren't silent. This variation allows for    
   /// incomplete types.                                                      
   template<class...T>
   concept PartialValidate = Inner::PartialValidateInner<T...>();
}