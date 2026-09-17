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
   /// Can be used in two ways to satisfy CT::ReflectAs<T>:                   
   /// 1. Specialize for T/concept, with the desired Type                     
   /// 2. Add a public `using CTTI_ReflectAs = <DesiredType>;` in T           
   /// Use void to disable reflection for the type                            
   template<class T>
   struct ReflectAs;

   /// nullptr_t is not reflectable                                           
   template<>
   struct ReflectAs<nullptr_t> {
      using Type = void;
   };
}

namespace Langulus::CT::Inner
{
   /// Convenience function that checks if ReflectAs is not void, which       
   /// would mean that the type is not reflectable at all. It also makes      
   /// sure that if ReflectAs is specified, then the involved types are       
   /// binary-compatible.                                                     
   template<class T>
   consteval auto IsReflectable() {
      static_assert(not ::std::is_reference_v<T>, "Strip references first");
      static_assert(not ::std::is_bounded_array_v<T>, "Strip extents first");

      if constexpr (not Complete<T>) {
         // Incomplete types are never reflectable                      
         return ::std::type_identity<void> {};
      }
      else if constexpr (Void<T>) {
         // Void types are never reflectable                            
         return ::std::type_identity<void> {};
      }
      else if constexpr (Complete<CTTI::ReflectAs<T>>) {
         // Substitution through external template.                     
         // Despite this, all participating types must be complete      
         // because their `sizeof` and `alignof` are checked.           
         // You can't reflect as a binary-incompatible type.            
         using AS = typename CTTI::ReflectAs<T>::Type;
         if constexpr (Void<AS>)
            return ::std::type_identity<void> {};
         else {
            static_assert(sizeof(T) == sizeof(AS),
               "Provided ReflectAs type must be of the same size");
            static_assert(alignof(T) == alignof(AS),
               "Provided ReflectAs type must be of the same alignment");
            return ::std::type_identity<AS> {};
         }
      }
      else if constexpr (::std::is_const_v<T>) {
         // Let's make sure constness is preserved                      
         using AS = decltype(IsReflectable<::std::remove_reference_t<T>>());
         if constexpr (Void<AS>)
            return ::std::type_identity<void> {};
         else
            return ::std::type_identity<typename AS::type const> {};
      }
      else if constexpr (::std::is_pointer_v<T>) {
         // Let's make sure indirections are preserved                  
         if constexpr (not Complete<::std::remove_pointer_t<T>>)
            return ::std::type_identity<T> {};
         else {
            using AS = decltype(IsReflectable<::std::remove_pointer_t<T>>());
            if constexpr (Void<AS>)
               return ::std::type_identity<void>{};
            else
               return ::std::type_identity<typename AS::type*> {};
         }
      }
      else if constexpr (requires { typename T::CTTI_ReflectAs; }) {
         // Substitution through internal type                          
         using AS = typename T::CTTI_ReflectAs;
         if constexpr (Void<AS>)
            return ::std::type_identity<void> {};
         else {
            static_assert(sizeof(T) == sizeof(AS),
               "Provided ReflectAs type must be of the same size");
            static_assert(alignof(T) == alignof(AS),
               "Provided ReflectAs type must be of the same alignment");
            return ::std::type_identity<AS> {};
         }
      }
      else return ::std::type_identity<T> {};
   }
}

namespace Langulus::CT
{
   /// Check if all T are reflectable                                         
   template<class...T>
   concept Reflectable = PartialValidate<T...>
       and (CT::NotVoid<decltype(Inner::IsReflectable<T>())> and ...);

   /// Get the type a given T is reflected as. This is very useful as a       
   /// a build-time optimization, because many type-erased containers are     
   /// binary-compatible with their templated equivalents, and the use of     
   /// CTTI_ReflectAs can drastically lower build time for RTTI generation,   
   /// by reducing unnessesary template reflections of redundant types.       
   ///   @attention this is designed only for affecting the reflection of     
   ///      data types, not tag, verb, or constant definitions                
   ///   @attention this will strip all references, but will preserve CV      
   ///      qualifiers and indirections!                                      
   template<class T>
   using ReflectedAs = typename decltype(Inner::IsReflectable<T>())::type;
}
