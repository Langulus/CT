///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing.hpp>
#include <Langulus/CT/Retype.hpp>
#include <concepts>

using namespace Langulus;


namespace
{
   struct TypedNotRetypable {
      using CTTI_Typed = int;
      template<class>
      using Retype = TypedNotRetypable;
   };
   
   struct TypedRetypable {
      using CTTI_Typed = int;
      template<class T>
      using Retype = T;
   };

   template<class T>
   struct Retypable {
      T data;

      template<class ALT>
      using Retype = Retypable<ALT>;
   };
   
   template<class T>
   struct Retypable2 {
      T data;
   };
   
   template<class T>
   struct RetypableCustom {
      T data;
      template<class T2>
      using Retype = Retypable2<T2>;
   };
}

///                                                                           
/// MARK: Retype                                                              
SCENARIO("Retype") {
   static_assert(::std::same_as<Retype<void, float>, float>);
   static_assert(::std::same_as<Retype<TypedNotRetypable, float>, TypedNotRetypable>);
   static_assert(::std::same_as<Retype<TypedRetypable, float>, float>);
   static_assert(::std::same_as<Retype<Retypable<int>, float>, Retypable<float>>);
   static_assert(::std::same_as<Retype<RetypableCustom<int>, float>, Retypable2<float>>);

   static_assert(::std::same_as<Retype<int, float>, float>);
   static_assert(::std::same_as<Retype<int[2], float>, float[2]>);
   static_assert(::std::same_as<Retype<int&, float>, float&>);
   static_assert(::std::same_as<Retype<int&&, float>, float&&>);
   static_assert(::std::same_as<Retype<volatile int, float>, volatile float>);
   static_assert(::std::same_as<Retype<int*, float>, float*>);
   static_assert(::std::same_as<Retype<int***, float>, float***>);
   static_assert(::std::same_as<Retype<int***[5], float>, float***[5]>);
   static_assert(::std::same_as<Retype<const int***[5], float>, const float***[5]>);
   static_assert(::std::same_as<Retype<int**const*const, float>, float**const*const>);
   static_assert(::std::same_as<Retype<int const**const*const&, float>, float const**const*const&>);
}
