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
   /// 1) template<> struct Ability<MyData> : Verb1 {};                       
   /// 2) template<> struct Ability<MyData> : Types<Verb1, etc...> {};        
   /// 3) struct MyData { using CTTI_Ability = Verb1; };                      
   /// 4) struct MyData { using CTTI_Ability = Types<Verb1, etc...>; };       
   template<class OF, int UNIQUE>
   struct Ability;

   namespace Inner
   {
      template<class>
      struct AbilitySet;
   }
}

namespace Langulus::CT::Inner
{
   /// Gather all defined CTTI::Ability of type T                             
   ///   @return a type list containing all declared verbs                    
   template<class T, int PROGRESS = 0, class...PREV>
   constexpr auto GetAbilitiesOf(Types<PREV...>&& prev) {
      static_assert(NotConvoluted<T>, "Strip qualifiers first");
      static_assert(NotReference<T>,  "Strip references first");
      static_assert(NotSheddable<T>,  "Strip sheddables first");
      static_assert(Exact<DecvqAll<T>, T>,
         "Strip all decorations on all indirections first");
      static_assert((Exact<DecvqAll<PREV>, PREV> and ...),
         "Strip all decorations on all indirections first");
      
      using M = CTTI::Ability<T, PROGRESS>;
      if constexpr (requires { M{}; }) {
         if constexpr (CT::Typelist<typename M::Can>) {
            constexpr typename M::Can verbs;
            ForEach(verbs, []<class TO> {
               static_assert(NotConvoluted<TO>, "Strip qualifiers first");
               static_assert(NotReference<TO>,  "Strip references first");
               static_assert(NotSheddable<TO>,  "Strip sheddables first");
               static_assert(not Types<PREV...>::template Contains<TO>,
                  "Verb redefinition");
            });
            return GetAbilitiesOf<T, PROGRESS + 1>(prev + verbs);
         }
         else {
            static_assert(not Types<PREV...>::template Contains<typename M::Can>,
               "Verb redefinition");
            return GetAbilitiesOf<T, PROGRESS + 1>(prev + Types<typename M::Can>{});
         }
      }
      else return prev;
   }

   /// Find the CTTI::Ability declaration that implements the desired verb    
   ///   @attention each call to this function is a uniquely defined one,     
   ///      and the result might change depending on the include-chain at     
   ///      the point of instantiation.                                       
   template<class OF, class VERB, int PROGRESS = 0, auto UNIQUE = []{}>
   consteval int FindAbility() {
      static_assert(NotConvoluted<OF, VERB>, "Strip qualifiers first");
      static_assert(NotReference<OF, VERB>,  "Strip references first");
      static_assert(NotSheddable<OF, VERB>,  "Strip sheddables first");
      static_assert(Exact<DecvqAll<OF>, OF>,
         "Strip all decorations on all indirections first in OF");
      static_assert(Exact<DecvqAll<VERB>, VERB>,
         "Strip all decorations on all indirections first in VERB");

      using M = CTTI::Ability<OF, PROGRESS>;
      if constexpr (requires { M{}; }) {
         if constexpr (CT::Typelist<typename M::Can>) {
            constexpr typename M::Verbs verbs;
            if constexpr (verbs.template Contains<VERB>) {
               // Prioritize concrete specializations over concept ones 
               constexpr int concrete = FindAbility<OF, VERB, PROGRESS + 1, UNIQUE>();
               if constexpr (concrete == -1)
                  return PROGRESS;
               else
                  return concrete;
            }
            else return FindAbility<OF, VERB, PROGRESS + 1, UNIQUE>();
         }
         else if constexpr (::std::is_same_v<typename M::Can, VERB>) {
            // Prioritize concrete specializations over concept ones    
            constexpr int concrete = FindAbility<OF, VERB, PROGRESS + 1, UNIQUE>();
            if constexpr (concrete == -1)
               return PROGRESS;
            else
               return concrete;
         }
         else return FindAbility<OF, VERB, PROGRESS + 1, UNIQUE>();
      }
      else return -1;
   }
   
   /// Helper function to extract all associated abilities                    
   /*template<class T>
   consteval auto GetAllAbilities() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip qualifiers first");
      using ctti = CTTI::Ability<T>;

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
            "Can't access `CTTI_Ability` inside incomplete type");

         if constexpr (requires { typename T::CTTI_Ability; }) {
            using inner = typename T::CTTI_Ability;
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
   };*/
}

namespace Langulus::CT
{
   /// Check if 'T' had the ability to do all 'VERB'                          
   ///   @attention this concept might change at compile-time! This should be 
   ///      detected by the compiler, so don't worry! If it happens, you can  
   ///      fix it by including the same headers everywhere it is used.       
   template<class T, class...VERB>
   concept Able = PartialValidate<VERB...> and (
      (Inner::FindAbility<DecvqAll<ShedDeref<T>>,
                          DecvqAll<ShedDeref<VERB>>, 0>() >= 0
      ) and ...);
}

namespace Langulus
{
   /// Get the reflected abilities from T to other types                      
   template<class T>
   using GatherAbilitiesOf = decltype(
      CT::Inner::GetAbilitiesOf<DecvqAll<Deref<T>>>(Types<>{})
   );
}

#include "../Utils/StaticSet.hpp"

#define LglsImplementAbilitiesFor(OF) \
   template<int UNIQUE> requires (UNIQUE == GetStaticSetIndex<Inner::AbilitySet<OF>, HERE()>()) \
   struct Ability<OF, UNIQUE>