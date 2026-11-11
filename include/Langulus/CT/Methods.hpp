///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../NameOf.hpp"


namespace Langulus::CTTI
{
   /// Can be used to reflect methods in two ways:                            
   /// 1. Specialize for T/concept                                            
   /// 2. Add a public `using CTTI_Methods = Methods<...>;` in T              
   template<class T>
   struct DefineMethods;
}

namespace Langulus::CT::Inner
{
   template<auto T>
   struct EmballageMethod {};

   /// Used for method reflections inside data types                          
   ///   @tparam HANDLE a pointer to a method                                 
   ///   @attention having just `auto HANDLE` as a template argument          
   ///      should generally be enough and works as it should on MSVC,        
   ///      however it doesn't make unique template instantiations on         
   ///      Clang and causes very nasty bugs. So, we're forced to add         
   ///      a couple more template parameters, to ensure proper templating.   
   ///      We compensate by using CTAD to deduce those parameters.           
   template<auto HANDLE, class OWNER, class RETURN, class...ARGUMENTS>
   struct MethodReflector {
      using Method = decltype(HANDLE);
      static_assert(::std::is_member_pointer_v<Method>,
         "Member must be a member pointer");
      using Owner     = OWNER;
      using Return    = RETURN;
      using Arguments = Types<ARGUMENTS...>;
      static constexpr RETURN (OWNER::*Handle)(ARGUMENTS...) = HANDLE;
      static constexpr auto Name = CppNameOf<HANDLE>();

      constexpr MethodReflector() = default;
      constexpr MethodReflector(EmballageMethod<HANDLE>, RETURN (OWNER::*)(ARGUMENTS...)) {}
   };

   /// This CTAD auto-completes the additional template arguments             
   template<auto HANDLE, class OWNER, class RETURN, class...ARGUMENTS>
   MethodReflector(EmballageMethod<HANDLE>, RETURN (OWNER::*)(ARGUMENTS...))
      -> MethodReflector<HANDLE, OWNER, RETURN, ARGUMENTS...>;

   /// Helper function to extract reflected methods                           
   template<class T>
   consteval auto GetMethods() {
      static_assert(not ::std::is_reference_v<T>,
         "Strip references first");

      if constexpr (Complete<CTTI::DefineMethods<T>>) {
         // Checked externally, T doesn't have to be complete           
         return typename CTTI::DefineMethods<T>::Type {};
      }
      else if constexpr (requires { typename T::CTTI_Methods; }) {
         // Checked internally, T has to be a complete type             
         return typename T::CTTI_Methods {};
      }
      else return NoTypes {};
   };
}

namespace Langulus
{
   /// Can be used to reflect named methods inside your T like so:            
   /// public: using CTTI_Methods = Methods<&T::Do, &T::Whatever>;            
   /// Or by specializing CTTI::DefineMethods<T>                              
   template<auto...M>
   struct Methods : Types<
      decltype(CT::Inner::MethodReflector(LglsFake(CT::Inner::EmballageMethod<M>), M))...
   > {};

   /// Get the reflected named methods                                        
   template<class T>
   using MethodsOf = decltype(CT::Inner::GetMethods<Decvq<Deref<T>>>());
}
