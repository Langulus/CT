///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Complete.hpp"


namespace CT
{
   namespace Inner
   {
      /// Extracts the inner type if T is marked as sheddable.                
      /// Otherwise results in an empty type list.                            
      template<class T>
      consteval auto GetSheddedType() {
         using DT = ::std::remove_cvref_t<T>;

         if constexpr (Complete<DT> and ::std::is_class_v<DT>) {
            if constexpr (requires { typename DT::CTTI_Sheddable; }) {
               using InnerT = typename DT::CTTI_Sheddable;
               if constexpr (::std::same_as<InnerT, No>
               or ::std::is_void_v<InnerT>)
                  return NoTypes {};
               else {
                  static_assert(not ::std::same_as<InnerT, Yes<>>,
                     "Instead of Yes<> pick a type to shed to "
                     "for CTTI_Sheddable");
                  return Types<InnerT> {};
               }
            }
            else return NoTypes {};
         }
         else return NoTypes {};
      };

      /// Extracts the inner type if T is marked as sheddable.                
      /// If T is not sheddable, just returns T as the type.                  
      ///   @attention strips _all_ sheddables                                
      template<class T>
      consteval auto ShedInner() {
         using ST = decltype(GetSheddedType<T>());
         if constexpr (ST::Empty)
            return Types<T> {};
         else
            return ShedInner<typename ST::First>();
      };
   }
   
   /// Check if all T are sheddable types (like intents), that serve only     
   /// to wrap data for tag dispatching and semantics. Sheddable types        
   /// don't carry any real data, and often just contain a reference          
   /// to the real data.                                                      
   /// They should be aggressively optimized out from the final binary.       
   /// Marking types as sheddable means that they don't interfere with most   
   /// other CT concepts - these will act as if sheddable types don't         
   /// exist at all, with the exception of CT::Sheddable itself.              
   ///   @attention sheddable types can only be defined through a member      
   ///      called CTTI_Sheddable, and are always assumed complete,           
   ///      otherwise this check will return false                            
   template<class...T>
   concept Sheddable = PartialValidate<T...>
         and ((not decltype(Inner::GetSheddedType<T>())::Empty) and ...);

   template<class...T>
   concept NotSheddable = PartialValidate<T...>
         and ((not Sheddable<T>) and ...);
}