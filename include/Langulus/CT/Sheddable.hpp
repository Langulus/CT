///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Void.hpp"


namespace Langulus::CTTI
{
   /// Affects CT::Sheddable                                                  
   template<class>
   struct Sheddable;
}

namespace Langulus::CT::Inner
{
   /// Extracts the inner type if T is marked as sheddable                    
   ///   @attention doesn't strip any references                              
   template<class T>
   consteval auto GetSheddedType() {
      if constexpr (Void<T>) {
         // Void types are never sheddable                              
         return ::std::type_identity<T> {};
      }
      else {
         using TI = CTTI::Sheddable<T>;
         if constexpr (Complete<TI>) {
            // External test                                            
            static_assert(requires { typename TI::Type; },
               "CTTI::Sheddable lacks Type");

            using InnerT = typename TI::Type;
            if constexpr (Void<InnerT>)
               return ::std::type_identity<T> {};
            else {
               static_assert(not requires { InnerT::Enabled; },
                  "Pick a type to shed to, or void, instead of using Yes/Yup/No"
                  "for CTTI::Sheddable::Type");
               return ::std::type_identity<InnerT> {};
            }
         }
         else {
            // Internal check                                           
            using DT = ::std::remove_cvref_t<T>;
            static_assert(Complete<DT>,
               "Can't access `CTTI_Sheddable` inside incomplete type");

            if constexpr (requires { typename DT::CTTI_Sheddable; }) {
               using InnerT = typename DT::CTTI_Sheddable;
               if constexpr (Void<InnerT>)
                  return ::std::type_identity<T> {};
               else {
                  static_assert(not requires { InnerT::Enabled; },
                     "Pick a type to shed to, or void, instead of using Yes/Yup/No"
                     "for T::CTTI_Sheddable");
                  return ::std::type_identity<InnerT> {};
               }
            }
            else return ::std::type_identity<T> {};
         }
      }
   };

   /// Extracts the inner type if T is marked as sheddable.                   
   /// If T is not sheddable, just returns T as the type.                     
   ///   @attention strips _all_ sheddables                                   
   template<class T>
   consteval auto ShedInner() {
      using ST = typename decltype(GetSheddedType<T>())::type;
      if constexpr (::std::is_same_v<T, ST>)
         return ::std::type_identity<T> {};
      else
         return ShedInner<ST>();
   };
}

namespace Langulus::CT
{
   /// Check if all T are sheddable types (like intents), that serve only     
   /// to wrap data for tag dispatching and semantics. Sheddable types        
   /// don't carry any real data, and often just contain a reference          
   /// to the real data.                                                      
   /// They should be aggressively optimized out from the final binary.       
   /// Marking types as sheddable means that they don't interfere with most   
   /// other CT concepts - most will act as if sheddable types don't          
   /// exist at all.                                                          
   template<class...T>
   concept Sheddable = ((not ::std::is_same_v<::std::type_identity<T>, decltype(Inner::GetSheddedType<T>())>) and ...);

   template<class...T>
   concept NotSheddable = ((not Sheddable<T>) and ...);
}

namespace Langulus
{
   /// Sheds all sheddable types                                              
   template<class T>
   using Shed = typename decltype(CT::Inner::ShedInner<T>())::type;

   /// Sheds and then dereferences all sheddable types                        
   template<class T>
   using ShedDeref = ::std::remove_reference_t<Shed<T>>;
}