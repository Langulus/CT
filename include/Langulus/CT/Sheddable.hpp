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
   template<class T>
   consteval auto GetSheddedType() {
      using DT = ::std::remove_cvref_t<T>;
      using TI = CTTI::Sheddable<DT>;

      if constexpr (Complete<TI>) {
         // External test                                               
         static_assert(requires { typename TI::Type; },
            "CTTI::Sheddable lacks Type");

         using InnerT = typename TI::Type;
         if constexpr (Void<InnerT>)
            return ::std::type_identity<void> {};
         else {
            static_assert(not requires { InnerT::Enabled; },
               "Pick a type to shed to, or void, instead of using Yes/Yup/No"
               "for CTTI::Sheddable::Type");
            return ::std::type_identity<InnerT> {};
         }
      }
      else {
         // Internal check                                              
         static_assert(Complete<DT>,
            "Can't access `CTTI_Sheddable` inside incomplete type");

         if constexpr (requires { typename DT::CTTI_Sheddable; }) {
            using InnerT = typename DT::CTTI_Sheddable;
            if constexpr (Void<InnerT>)
               return ::std::type_identity<void> {};
            else {
               static_assert(not requires { InnerT::Enabled; },
                  "Pick a type to shed to, or void, instead of using Yes/Yup/No"
                  "for CTTI_Sheddable");
               return ::std::type_identity<InnerT> {};
            }
         }
         else return ::std::type_identity<void> {};
      }
   };

   /// Extracts the inner type if T is marked as sheddable.                   
   /// If T is not sheddable, just returns T as the type.                     
   ///   @attention strips _all_ sheddables                                   
   template<class T>
   consteval auto ShedInner() {
      using ST = decltype(GetSheddedType<T>());
      if constexpr (Void<ST>)
         return ::std::type_identity<void> {};
      else
         return ShedInner<typename ST::type>();
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
   concept Sheddable = ((not Void<decltype(Inner::GetSheddedType<T>())>) and ...);

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