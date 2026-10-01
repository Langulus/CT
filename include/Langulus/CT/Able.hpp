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

      struct MakeConceptualAbility {
         static constexpr bool Conceptual = true;
      };   
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
      static_assert(::std::is_same_v<DecvqAll<T>, T>,
         "Strip all decorations on all indirections first");
      static_assert((::std::is_same_v<DecvqAll<PREV>, PREV> and ...),
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
      static_assert(::std::is_same_v<DecvqAll<OF>, OF>,
         "Strip all decorations on all indirections first in OF");
      static_assert(::std::is_same_v<DecvqAll<VERB>, VERB>,
         "Strip all decorations on all indirections first in VERB");

      using M = CTTI::Ability<OF, PROGRESS>;
      if constexpr (requires { M{}; }) {
         if constexpr (CT::Typelist<typename M::Can>) {
            constexpr typename M::Verbs verbs;
            if constexpr (verbs.template Contains<VERB>) {
               // Prioritize concrete specializations over concept ones 
               if constexpr (requires { typename M::Conceptual; }) {//TODO i initially forgot to add this to the implementation, but tests still passed - make sure we add more tests and evaluate if this is necessary at all
                  constexpr int concrete = FindAbility<OF, VERB, PROGRESS + 1, UNIQUE>();
                  if constexpr (concrete == -1)
                     return PROGRESS;
                  else
                     return concrete;
               }
               else return PROGRESS;
            }
            else return FindAbility<OF, VERB, PROGRESS + 1, UNIQUE>();
         }
         else if constexpr (::std::is_same_v<typename M::Can, VERB>) {
            // Prioritize concrete specializations over concept ones    
            if constexpr (requires { typename M::Conceptual; }) {//TODO i initially forgot to add this to the implementation, but tests still passed - make sure we add more tests and evaluate if this is necessary at all
               constexpr int concrete = FindAbility<OF, VERB, PROGRESS + 1, UNIQUE>();
               if constexpr (concrete == -1)
                  return PROGRESS;
               else
                  return concrete;
            }
            else return PROGRESS;
         }
         else return FindAbility<OF, VERB, PROGRESS + 1, UNIQUE>();
      }
      else return -1;
   }
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

   /// Invoke an ability by finding it at compile-time                        
   //TODO this can also potentially be used to run flows at compile-time      
   template<class OF, class ABILITY>
   bool InvokeAbility(OF& context, ABILITY& verb) {
      constexpr int ability = CT::Inner::FindAbility<OF, ABILITY>();
      static_assert(ability >= 0, "OF lacks ABILITY");
      using IMPLEMENTATION = CTTI::Ability<OF, ability>;
      return IMPLEMENTATION::Default(context, verb);
   }
}

#include "../Utils/StaticSet.hpp"

/// Extent OF with the abilities provided by you. Example in which allow for  
/// integers to be added to other integers:                                   
///  LglsImplementAbilitiesFor(int) {                                         
///     using Can = Verbs::Add;                                               
///     static bool Default(int& lhs, Verb& verb) {                           
///         Many const& rhs = verb.GetArgument();                             
///         rhs.ForEach([&lhs](int const& i) { lhs += i; });                  
///         return true;                                                      
///     }                                                                     
///     static bool Default(int const& lhs, Verb& verb) {                     
///         Many const& rhs = verb.GetArgument();                             
///         int result = lhs;                                                 
///         rhs.ForEach([&result](int const& i) { result += i; });            
///         verb << result;                                                   
///         return true;                                                      
///     }                                                                     
///  };                                                                       
#define LglsImplementAbilitiesFor(OF) \
   template<int UNIQUE> requires (UNIQUE == GetStaticSetIndex<Inner::AbilitySet<OF>, HERE()>()) \
   struct Ability<OF, UNIQUE>
   
/// Same as above, but uses a concept to group types. Here's a more elegant   
/// solution to the above, that applies to all number types:                  
///  LglsImplementAbilitiesForConcept(CT::Number, OF) {                       
///     using Can = Verbs::Add;                                               
///     static bool Default(OF& lhs, Verb& verb) {                            
///         Many const& rhs = verb.GetArgument();                             
///         rhs.ForEach([&lhs](OF const& i) { lhs += i; });                   
///         return true;                                                      
///     }                                                                     
///     static bool Default(OF const& lhs, Verb& verb) {                      
///         Many const& rhs = verb.GetArgument();                             
///         OF result = lhs;                                                  
///         rhs.ForEach([&result](OF const& i) { result += i; });             
///         verb << result;                                                   
///         return true;                                                      
///     }                                                                     
///  };                                                                       
#define LglsImplementAbilitiesForConcept(CONCEPT, OF) \
   template<CONCEPT OF, int UNIQUE> requires (UNIQUE == GetStaticSetIndex<Inner::AbilitySet<OF>, HERE()>()) \
   struct Ability<OF, UNIQUE> : Inner::MakeConceptualAbility