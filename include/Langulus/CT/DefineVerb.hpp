///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../Utils/Literal.hpp"


namespace Langulus
{
   /// A helper structure for reflecting a verb                               
   template<
      Literal POSITIVE,
      Literal NEGATIVE = "",
      auto PRECEDENCE = 0,
      bool SHORTCIRCUITED = false
   >
   struct NamedVerb {
      using ConsistentNamedVerbTypeEvenIfInherited = NamedVerb;
      static constexpr auto  Positive      = POSITIVE;
      static constexpr auto  Negative      = NEGATIVE;
      static constexpr float Precedence    = static_cast<float>(PRECEDENCE);
      static constexpr bool  ShortCircuit  = SHORTCIRCUITED;
      static constexpr bool  Enabled       = true;
   };

   /// A helper structure for reflecting a verb operator                      
   template<
      Literal POSITIVE,
      Literal NEGATIVE = ""
   >
   struct NamedOperator {
      static constexpr auto  Positive      = POSITIVE;
      static constexpr auto  Negative      = NEGATIVE;
      static constexpr bool  Enabled       = true;
   };
}

namespace Langulus::CTTI
{
   /// Declares T is a verb definition. Examples:                             
   /// 1) template<> struct DefineVerb<Do> : NamedVerb<"Do", "Undo"> {};      
   /// 2) struct Do { using CTTI_DefineVerb = NamedVerb<"Do", "Undo">; };     
   template<class T>
   struct DefineVerb;

   /// Augments a verb definition with operator tokens. Examples:             
   /// 1) template<> struct DefineVerbOp<Do> : NamedOperator<"+", "-"> {};    
   /// 2) struct Do { using CTTI_DefineVerbOp = NamedOperator<"+", "-">; };   
   template<class T>
   struct DefineVerbOp;
}

namespace Langulus::CT::Inner
{
   /// Get the definition of a verb at compile-time                           
   ///   @tparam T the verb to get the info of                                
   ///   @return a NamedVerb if verb was defined, or No otherwise             
   template<class T>
   consteval auto DefinitionOfVerb() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip constness first");
      using ctti = CTTI::DefineVerb<T>;

      if constexpr (CT::Complete<ctti>) {
         // Verb was defined externally                                 
         return typename ctti::ConsistentNamedVerbTypeEvenIfInherited {};
      }
      else if constexpr (::std::is_class_v<T>) {
         // Verb was defined internally                                 
         static_assert(CT::Complete<T>,
            "Can't access CTTI_DefineVerb in incomplete type");

         if constexpr (requires { typename T::CTTI_DefineVerb; }) {
            using inner = typename T::CTTI_DefineVerb;
            if constexpr (CT::Void<inner>)
               return No {};
            else 
               return inner {};
         }
         else return No {};
      }
      else return No {};
   }
   
   /// Get the definition of a verb operator at compile-time                  
   ///   @tparam T the verb to get the info of                                
   ///   @return a NamedVerbOp if defined, or No otherwise                    
   template<class T>
   consteval auto DefinitionOfVerbOp() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip constness first");
      using ctti = CTTI::DefineVerbOp<T>;

      if constexpr (CT::Complete<ctti>) {
         // Verb was defined externally                                 
         return ctti {};
      }
      else if constexpr (::std::is_class_v<T>) {
         // Verb was defined internally                                 
         static_assert(CT::Complete<T>,
            "Can't access CTTI_DefineVerbOp in incomplete type");

         if constexpr (requires { typename T::CTTI_DefineVerb; }) {
            using inner = typename T::CTTI_DefineVerbOp;
            if constexpr (CT::Void<inner>)
               return No {};
            else 
               return inner {};
         }
         else return No {};
      }
      else return No {};
   }

   /// Get the name of NamedVerb::Positive at compile-time                    
   ///   @tparam T the verb to get the name of                                
   ///   @return the name                                                     
   template<class T>
   consteval auto PositiveNameOfVerb() {
      constexpr auto definition = DefinitionOfVerb<T>();
      if constexpr (::std::is_same_v<decltype(definition), No const>)
         return Langulus::Literal {};
      else {
         constexpr auto c = definition.Positive;
         static_assert(IsASCII(c), "Verb positive name must be ASCII");
         static_assert(c == "" or IsAlphabetical(c[0]),
            "Verb positive name must begin with an alphabetical symbol");
         return c;
      }
   }
   
   /// Get the name of NamedVerb::Negative at compile-time                    
   ///   @tparam T the verb to get the name of                                
   ///   @return the name                                                     
   template<class T>
   consteval auto NegativeNameOfVerb() {
      constexpr auto definition = DefinitionOfVerb<T>();
      if constexpr (::std::is_same_v<decltype(definition), No const>)
         return Langulus::Literal {};
      else {
         constexpr auto c = definition.Negative;
         static_assert(IsASCII(c), "Verb negative name must be ASCII");
         static_assert(c == "" or IsAlphabetical(c[0]),
            "Verb negative name must begin with an alphabetical symbol");
         return c;
      }
   }
   
   /// Get the name of NamedOperator::Positive at compile-time                
   ///   @tparam T the verb to get the name of                                
   ///   @return the name                                                     
   template<class T>
   consteval auto PositiveOperatorOfVerb() {
      constexpr auto definition = DefinitionOfVerbOp<T>();
      if constexpr (::std::is_same_v<decltype(definition), No const>)
         return Langulus::Literal {};
      else {
         constexpr auto c = definition.Positive;
         static_assert(IsASCII(c), "Verb positive operator must be ASCII");
         return c;
      }
   }
   
   /// Get the name of NamedOperator::Negative at compile-time                
   ///   @tparam T the verb to get the name of                                
   ///   @return the name                                                     
   template<class T>
   consteval auto NegativeOperatorOfVerb() {
      constexpr auto definition = DefinitionOfVerbOp<T>();
      if constexpr (::std::is_same_v<decltype(definition), No const>)
         return Langulus::Literal {};
      else {
         constexpr auto c = definition.Negative;
         static_assert(IsASCII(c), "Verb negative operator must be ASCII");
         return c;
      }
   }
}

namespace Langulus::CT
{
   /// Checks if all E are defined constants                                  
   template<class...T>
   concept DefineVerb = ((not ::std::is_same_v<No, decltype(Inner::DefinitionOfVerb<Decvq<Deref<T>>>())>) and ...);

   /// Checks if all E are not defined constants                              
   template<class...T>
   concept NotDefineVerb = ((not DefineVerb<T>) and ...);
}

namespace Langulus
{
   /// Get the positive name of a verb, if it exists                          
   template<class T>
   constexpr auto PositiveNameOfVerb = CT::Inner::PositiveNameOfVerb<T>();

   /// Get the negative name of a verb, if it exists                          
   template<class T>
   constexpr auto NegativeNameOfVerb = CT::Inner::NegativeNameOfVerb<T>();

   /// Get the positive operator of a verb, if it exists                      
   template<class T>
   constexpr auto PositiveOperatorOfVerb = CT::Inner::PositiveOperatorOfVerb<T>();
   
   /// Get the negative operator of a verb, if it exists                      
   template<class T>
   constexpr auto NegativeOperatorOfVerb = CT::Inner::NegativeOperatorOfVerb<T>();
}