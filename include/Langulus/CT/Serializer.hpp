///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../IntentOf.hpp"


namespace Langulus::CTTI
{
   /// Can be used to satisfy CT::Serializer<T>                               
   template<class T>
   struct Serializer;
}

namespace Langulus::CT
{
   namespace Inner
   {
      /// Helper function to extract reflected serialize                      
      template<class T>
      consteval auto GetSerializer() {
         static_assert(not ::std::is_reference_v<T>,
            "Strip references first");

         if constexpr (CT::Complete<CTTI::Serializer<T>>)
            return CTTI::Serializer<T> {};
      };
   }

   /// Check if all T are serializers                                         
   template<class...T>
   concept Serializer = PartialValidate<T...>
       and ((NotVoid<decltype(Inner::GetSerializer<Shed<T>>())>) and ...);
}

namespace Langulus::CTTI
{
   /// This can be specialized for custom serialization rules. If a morphism  
   /// doesn't have custom rules, a static_cast<S>(T) is done, and then       
   /// the result is concatenated to the back of S.                           
   template<CT::Serializer S, class T>
   struct SerializationRule;
}

namespace Langulus
{
   /// Serialize                                                              
   ///   @attention there is a major difference between conversion and        
   ///      serialization. For example, you can't convert Text -> Text, as    
   ///      the same type is never converter to itself. However, you can      
   ///      serialize Text ~> Text, which will wrap the contents in quotes,   
   ///      and produce a completely different string. In other words:        
   ///      serialization is an indirection on top of conversion.             
   ///   @return the number of elements written to 'to'                       
   template<class FROM, CT::Serializer TO> requires CT::NoIntent<FROM, TO>
   auto Serialize(FROM const& from, TO& to, typename CTTI::Serializer<TO>::Context* context = nullptr) -> size_t {
      using DFROM = DecvqAll<FROM>;
      using DTO   = DecvqAll<TO>;
      const auto initial = to.GetCount();
      
      if constexpr (not CT::Complete<Decay<DFROM>>) {
         // Some custom rules require the decayed type to be complete   
         // in order to check whether they are CT::Deep. This won't     
         // pick the correct rule, if Decayed<DFROM> is incomplete,     
         // because the CT::Deep check requires deeply complete types.  
         // That's why we handle pointers separately here using the     
         // appropriate converter.                                      
         static_assert(CT::Sparse<DFROM>, "If not complete, `FROM` needs to be sparse");
         (void) context;
         to += Convert<DTO, FROM>(from);
      }
      else if constexpr (requires { CTTI::SerializationRule<DTO, DFROM>{}; }) {
         // Custom rule exists                                          
         if constexpr (requires { to.GetDictionary(); }) {
            if (not context) {
               context = to.GetDictionary();
               if (not context) {
                  to.Reserve(1);
                  context = to.GetDictionary();
               }
            }
         }

         CTTI::SerializationRule<DTO, DFROM>::Serialize(from, to, context);
      }
      else {
         // No rule exists, just cast and concatenate, if possible      
         (void) context;
         to += Convert<DTO, FROM>(from);
      }
      return to.GetCount() - initial;
   }

   /// Deserialize                                                            
   ///   @return the number of elements read from 'from'                      
   template<CT::Serializer FROM, class TO> requires CT::NoIntent<FROM, TO>
   auto Deserialize(FROM const& from, TO& to, size_t progress, typename CTTI::Serializer<FROM>::Context const* context = nullptr) -> size_t {
      using DFROM = DecvqAll<FROM>;
      using DTO   = DecvqAll<TO>;
      
      if constexpr (requires { from.GetDictionary(); }) {
         if (not context)
            context = from.GetDictionary();
      }

      return CTTI::SerializationRule<DFROM, DTO>::Deserialize(from, progress, to, context);
   }
}

namespace Langulus::Serial
{
   enum class Operator {
      Noop            ,
      OpenScope       ,
      CloseScope      ,
      OpenScopeAlt    ,
      CloseScopeAlt   ,
      OpenCode        ,
      CloseCode       ,
      OpenComment     ,
      CloseComment    ,
      OpenLineComment ,
      CloseLineComment,
      OpenString      ,
      CloseString     ,
      OpenStringAlt   ,
      CloseStringAlt  ,
      OpenCharacter   ,
      CloseCharacter  ,
      OpenByte        ,
      CloseByte       ,
      SelectIdea      ,
      SelectThing     ,
      Future          ,
      Past            ,
      Null            ,
      Escape          ,
      Mass            ,
      Rate            ,
      Time            ,
      Priority        ,
      And             ,
      AndUnordered    ,
      Pair            ,
      Or              ,
      Last              
   };

   /// Helps to define an operator                                            
   template<Literal TOKEN, bool CHARGE = false>
   struct OperatorDefinition {
      static constexpr auto token     = TOKEN;
      static constexpr bool is_charge = CHARGE;
   };

   /// Built-in operator properties.                                          
   /// These are tuned for Langulus::Code specification, but you can          
   /// use your own in your custom CTTI::Serializer.                          
   constexpr auto OpenScope       = OperatorDefinition<"("      > {};
   constexpr auto CloseScope      = OperatorDefinition<")"      > {};
   constexpr auto OpenScopeAlt    = OperatorDefinition<"["      > {};
   constexpr auto CloseScopeAlt   = OperatorDefinition<"]"      > {};
   constexpr auto OpenCode        = OperatorDefinition<"{"      > {};
   constexpr auto CloseCode       = OperatorDefinition<"}"      > {};
   constexpr auto OpenComment     = OperatorDefinition<"/*"     > {};
   constexpr auto CloseComment    = OperatorDefinition<"*/"     > {};
   constexpr auto OpenLineComment = OperatorDefinition<"//"     > {};
   constexpr auto CloseLineComment= OperatorDefinition<"\n"     > {};
   constexpr auto OpenString      = OperatorDefinition<"\""     > {};
   constexpr auto CloseString     = OperatorDefinition<"\""     > {};
   constexpr auto OpenStringAlt   = OperatorDefinition<"`"      > {};
   constexpr auto CloseStringAlt  = OperatorDefinition<"`"      > {};
   constexpr auto OpenCharacter   = OperatorDefinition<"'"      > {};
   constexpr auto CloseCharacter  = OperatorDefinition<"'"      > {};
   constexpr auto OpenByte        = OperatorDefinition<"0x"     > {};
   constexpr auto CloseByte       = OperatorDefinition<" "      > {};
   constexpr auto SelectIdea      = OperatorDefinition<"##"     > {};
   constexpr auto SelectThing     = OperatorDefinition<"#"      > {};
   constexpr auto Future          = OperatorDefinition<"??"     > {};
   constexpr auto Past            = OperatorDefinition<"?"      > {};
   constexpr auto Null            = OperatorDefinition<"null"   > {};
   constexpr auto Escape          = OperatorDefinition<"\\"     > {};
   constexpr auto Mass            = OperatorDefinition<"*", true> {};
   constexpr auto Rate            = OperatorDefinition<"^", true> {};
   constexpr auto Time            = OperatorDefinition<"@", true> {};
   constexpr auto Priority        = OperatorDefinition<"!", true> {};
   constexpr auto And             = OperatorDefinition<", "     > {};
   constexpr auto AndUnordered    = OperatorDefinition<"; "     > {};
   constexpr auto Pair            = OperatorDefinition<" -> "   > {};
   constexpr auto Or              = OperatorDefinition<" or "   > {};
}
