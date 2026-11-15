///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/CT/Retype.hpp>
#include <concepts>


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

#define DEFINE_RETYPE_TYPE_TEST(WHAT, RESULT) \
   static_assert(::std::same_as<Retype<WHAT, float>, RESULT>);

///                                                                           
/// MARK: Retype                                                              
SCENARIO("Retype") {
   DEFINE_RETYPE_TYPE_TEST(void, float);
   DEFINE_RETYPE_TYPE_TEST(TypedNotRetypable, TypedNotRetypable);
   DEFINE_RETYPE_TYPE_TEST(TypedRetypable, float);
   DEFINE_RETYPE_TYPE_TEST(Retypable<int>, Retypable<float>);
   DEFINE_RETYPE_TYPE_TEST(RetypableCustom<int>, Retypable2<float>);

   DEFINE_RETYPE_TYPE_TEST(int, float);
   DEFINE_RETYPE_TYPE_TEST(int[2], float[2]);
   DEFINE_RETYPE_TYPE_TEST(int&, float&);
   DEFINE_RETYPE_TYPE_TEST(int&&, float&&);
   DEFINE_RETYPE_TYPE_TEST(volatile int, volatile float);
   DEFINE_RETYPE_TYPE_TEST(int*, float*);
   DEFINE_RETYPE_TYPE_TEST(int***, float***);
   DEFINE_RETYPE_TYPE_TEST(int***[5], float***[5]);
   DEFINE_RETYPE_TYPE_TEST(const int***[5], const float***[5]);
   DEFINE_RETYPE_TYPE_TEST(int**const*const, float**const*const);
   DEFINE_RETYPE_TYPE_TEST(int const**const*const&, float const**const*const&);

   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int>,          CustomPointerType<float>)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int>&,         CustomPointerType<float>&)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int> const,    CustomPointerType<float> const)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int> const*,   CustomPointerType<float> const*)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int>,          CustomPointerType<float>)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int>&,         CustomPointerType<float>&)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int> const,    CustomPointerType<float> const)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int>*,         CustomPointerType<float>*)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int> const*,   CustomPointerType<float> const*)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int> const* const* const,   CustomPointerType<float> const* const* const)
   DEFINE_RETYPE_TYPE_TEST(CustomPointerType<int> const*&&, CustomPointerType<float> const*&&)

}
