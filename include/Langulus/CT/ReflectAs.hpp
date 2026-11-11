///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Typenav.hpp>
#include <Langulus/CT/Retype.hpp>


namespace Langulus::CTTI
{
   /// Can be used in two ways to satisfy CT::ReflectAs<T>:                   
   /// 1. Specialize for T/concept, with the desired Type                     
   /// 2. Add a public `using CTTI_ReflectAs = <DesiredType>;` in T           
   /// Use void to disable reflection for the type                            
   template<class T> requires (not ::std::is_reference_v<T>
                           and not ::std::is_bounded_array_v<T>
                           and not ::std::is_const_v<T>
                           and not ::std::is_volatile_v<T>
                           and not ::std::is_pointer_v<T>)
   struct ReflectAs;

   /// void is not reflectable                                                
   template<>
   struct ReflectAs<void> { using Type = void; };

   /// nullptr_t is not reflectable                                           
   template<>
   struct ReflectAs<nullptr_t> { using Type = void; };
}

namespace Langulus::CT::Inner
{
   /// Convenience function that checks if ReflectAs is not void, which       
   /// would mean that the type is not reflectable at all. It also makes      
   /// sure that if ReflectAs is specified, then the involved types are       
   /// binary-compatible.                                                     
   /// If you provide a decorated/pointer type, all decorations will be       
   /// preserved, and only the decayed type will be replaced by the reflected.
   ///   @return type_identity containing the reflected type                  
   template<class T>
   consteval auto FormReflectedType() {
      if constexpr (not CT::Decayed<T>) {
         using DT = Decay<T>;
         using AS = typename decltype(FormReflectedType<DT>())::type;
         if constexpr (::std::is_void_v<AS> and not ::std::is_pointer_v<T>) {
            // Not reflectable only if resulting in a dense void.       
            return ::std::type_identity<void> {};
         }
         else {
            // Pointers are always reflectable, even if pointing to     
            // unreflectable types. Also preserved all decorations.     
            if constexpr (::std::is_void_v<AS>)
               return ::std::type_identity<Retype<T, DT>> {};
            else
               return ::std::type_identity<Retype<T, AS>> {};
         }
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
      else {
         // If incomplete, just return it as it is.                     
         // Reflectable until proven otherwise.                         
         return ::std::type_identity<T> {};
      }
   }
}

namespace Langulus::CT
{
   /// Check if all T are reflectable                                         
   template<class...T>
   concept Reflectable = PartialValidate<T...>
       and (CT::NotVoid<decltype(Inner::FormReflectedType<T>())> and ...);

   /// Get the type a given T is reflected as. This is very useful as a       
   /// a build-time optimization, because many type-erased containers are     
   /// binary-compatible with their templated equivalents, and the use of     
   /// CTTI_ReflectAs can drastically lower build time for RTTI generation,   
   /// by reducing unnessesary template reflections of redundant types.       
   ///   @attention this is designed only for affecting the reflection of     
   ///      data types, not tag, verb, or constant definitions                
   ///   @attention this will preserve all CV qualifiers, references and      
   ///      indirections!                                                     
   template<class T>
   using ReflectedAs = typename decltype(Inner::FormReflectedType<T>())::type;
}
