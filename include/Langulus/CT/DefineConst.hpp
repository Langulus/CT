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
#include "../Utils/Types.hpp"
#include "../Utils/Values.hpp"


namespace Langulus
{
   /// A helper structure for reflecting a constant, with an optional custom  
   /// token and info string                                                  
   template<auto E, Literal TOKEN = "", Literal INFO = "">
   struct NamedValue {
      using CTTI_ReflectAs = void;
      static constexpr auto Constant = E;
      static constexpr bool Enabled  = true;
      static constexpr auto Token    = TOKEN;
      static constexpr auto Info     = INFO;
   };
}

namespace Langulus::CTTI
{
   /// Extends T with named values at compile-time. Examples:                 
   /// 1) template<> struct DefineConstant<real> : NamedValue<3.14, "Pi"> {}; 
   /// 2) template<> struct DefineConstant<real> : Types<                     
   ///       NamedValue<3.14,       "Pi">,                                    
   ///       NamedValue<3.14/2, "HalfPi">                                     
   ///    > {};                                                               
   /// 3) template<> struct DefineConstant<std::endian> : Values<             
   ///       std::endian::big,                                                
   ///       std::endian::little                                              
   ///    > {};                                                               
   /// 4) struct X {                                                          
   ///      enum { One };                                                     
   ///      using CTTI_Values = NamedValue<One>;                              
   ///    };                                                                  
   /// 5) struct X {                                                          
   ///      enum { One, Two, Three };                                         
   ///      using CTTI_Values = Values<One, Two, Three>; };                   
   ///    };                                                                  
   /// 6) struct X {                                                          
   ///      enum { One, Two, Three };                                         
   ///      using CTTI_Values = Types<                                        
   ///         NamedValue<One,    "1", "A custom number one"  >,              
   ///         NamedValue<Two,    "2", "A custom number two"  >,              
   ///         NamedValue<Three,  "3", "A custom number three">,              
   ///      >;                                                                
   ///    };                                                                  
   template<class T>
   struct DefineConstant;

   /// Extend bool, by associating couple of constants with it                
   template<>
   struct DefineConstant<bool> : Types<
      NamedValue<true, "yes", "The canonical true boolean value">,
      NamedValue<false, "no", "The canonical false boolean value">
   > {};
}

namespace Langulus::CT::Inner
{
   /// Get the list of constants associated with a type                       
   ///   @tparam T the type to get named values of                            
   ///   @return a typelist of NamedValue(s)                                  
   template<class T>
   consteval auto GetNamedValueList() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip constness first");
      using ctti = CTTI::DefineConstant<T>;

      if constexpr (CT::Complete<ctti>) {
         //                                                             
         // Constants were defined externally                           
         if constexpr (CT::Typelist<ctti>) {
            // Constants are defined as a sequence of NamedValue(s)     
            return ctti {};
         }
         else if constexpr (CT::Valuelist<ctti>) {
            // Constants are defined as a sequence of values            
            return ctti::Expand([]<auto...E> {
               return Types<NamedValue<E>...> {};
            });
         }
         else if constexpr (requires { ctti::Constant; }) {
            // Constant should be a single NamedValue                   
            return Types<ctti> {};
         }
         else return NoTypes {};
      }
      else if constexpr (::std::is_class_v<T>) {
         //                                                             
         // Constants were defined internally.                          
         static_assert(CT::Complete<T>,
            "T must be complete in order to extract named values from it");

         if constexpr (requires { typename T::CTTI_Values; }) {
            using inner = typename T::CTTI_Values;
            if constexpr (CT::Void<inner>) {
               // Named values are explicitly disabled for T            
               return NoTypes {};
            }
            else if constexpr (CT::Typelist<inner>) {
               // Constants are defined as a sequence of NamedValue(s)  
               return inner {};
            }
            else if constexpr (CT::Valuelist<inner>) {
               // Constants are defined as a sequence of values         
               return ctti::Expand([]<auto...E> {
                  return Types<NamedValue<E>...> {};
               });
            }
            else if constexpr (requires { inner::Constant; }) {
               // Constant should be a single NamedValue                
               return Types<inner> {};
            }
            else return NoTypes {};
         }
         else return NoTypes {};
      }
      else return NoTypes {};
   }

   /// Get the definition of a constant at compile-time                       
   ///   @tparam E the value to get the info of                               
   ///   @return a NamedValue if constant was defined, or No otherwise        
   /*template<auto E>
   consteval auto DefinitionOfConstant() {
      using T    = decltype(E);
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_const_v<T>, "Strip constness first");
      using ctti = CTTI::DefineConstant<T>;

      if constexpr (CT::Complete<ctti>) {
         //                                                             
         // Constants were defined externally                           
         if constexpr (CT::Typelist<ctti>) {
            // Constants are defined as a sequence of NamedValue(s)     
            return ctti::ForEachConstOr([]<class V> {
               if constexpr (V::Constant == E)
                  return V {};
               else
                  return No {};
            });
         }
         else if constexpr (CT::Valuelist<ctti>) {
            // Constants are defined as a sequence of values            
            if constexpr (ctti::template Contains<E>)
               return NamedValue<E> {};
            else
               return No {};
         }
         else if constexpr (ctti::Constant == E) {
            // Constant should be a single NamedValue                   
            return ctti {};
         }
         else return No {};
      }
      else if constexpr (::std::is_class_v<T>) {
         //                                                             
         // Constants were defined internally.                          
         // We have a guarantee T is complete. E is here, after all.    
         using inner = typename T::CTTI_Values;
         if constexpr (CT::Void<inner>) {
            // Named values are explicitly disabled for T               
            return No {};
         }
         else if constexpr (CT::Typelist<inner>) {
            // Constants are defined as a sequence of NamedValue(s)     
            return inner::ForEachConstOr([]<class V> {
               if constexpr (V::Constant == E)
                  return V {};
               else
                  return No {};
            });
         }
         else if constexpr (CT::Valuelist<inner>) {
            // Constants are defined as a sequence of values            
            if constexpr (inner::template Contains<E>)
               return NamedValue<E> {};
            else
               return No {};
         }
         else if constexpr (inner::Constant == E) {
            // Constant should be a single NamedValue                   
            return inner {};
         }
         else return No {};
      }
      else return No {};
   }
   
   /// Get a custom token of a constant at compile-time, if such was defined  
   ///   @tparam E the constant to get the token of                           
   ///   @return a compile-time string                                        
   template<auto E>
   consteval auto CustomNameOfConstant() {
      constexpr auto definition = DefinitionOfConstant<E>();
      if constexpr (::std::is_same_v<decltype(definition), No const>)
         return Langulus::Literal {};
      else {
         constexpr auto c = definition.Token;
         static_assert(IsASCII(c), "Constant name must be ASCII");
         static_assert(c == "" or IsAlphabetical(c[0]),
            "Constant name must begin with an alphabetical symbol");
         return c;
      }
   }*/
}

/*namespace Langulus::CT
{
   /// Checks if all E are defined constants                                  
   template<auto...E>
   concept DefineConstant = ((not ::std::is_same_v<No, decltype(Inner::DefinitionOfConstant<E>())>) and ...);

   /// Checks if all E are not defined constants                              
   template<auto...E>
   concept NotDefineConstant = ((not DefineConstant<E>) and ...);
}*/

namespace Langulus
{
   /// Get the reflected named values associated with type T                  
   template<class T>
   using NamedValuesOf = decltype(CT::Inner::GetNamedValueList<Decvq<Deref<T>>>());
}
