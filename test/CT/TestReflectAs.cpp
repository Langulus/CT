///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/CT/ReflectAs.hpp>


///                                                                           
/// MARK: Reflectable                                                         
TEST_CASE_TEMPLATE("Testing CT::Reflectable", TestType
   , ReflectableIntern
   , ReflectableIntern const
   , ReflectableIntern&
   , ReflectableIntern*
   , ReflectableExtern
   , ReflectableExtern const
   , ReflectableExtern&
   , ReflectableExtern*
   , ReflectableAsSelf
   , ReflectableAsSelf const
   , ReflectableAsSelf&
   , ReflectableAsSelf*
   , int
   , int const
   , int const&, int&
   , void*
   , IncompleteType*
   , IncompleteType
   , IncompleteType const
   , NotReflectableExtern*
   , NotReflectableIntern*
   , NotReflectable*
   , Types<void*>*
) {
   static_assert(CT::Reflectable<TestType>);
}

TEST_CASE_TEMPLATE("Testing not CT::Reflectable", TestType
   //, IncompleteType&                    // shouldn't compile at all
   //, IncompleteType const&              // shouldn't compile at all
   //, SheddableType<IncompleteType>      // shouldn't compile at all
   //, SheddableType<IncompleteType>&     // shouldn't compile at all
   //, SheddableType<IncompleteType>*     // shouldn't compile at all
   , void
   , nullptr_t
   , Types<void*>
   //, SheddableType<int>                 // shouldn't compile at all
   //, SheddableType<int*>                // shouldn't compile at all
   //, SheddableType<int* const>          // shouldn't compile at all
   //, SheddableType<int* const&>         // shouldn't compile at all
   //, SheddableType<int>*                // shouldn't compile at all
   //, SheddableType<int*>*               // shouldn't compile at all
   //, SheddableType<int* const>*         // shouldn't compile at all
   //, SheddableType<int* const&>*        // shouldn't compile at all
   //, SheddableType<NotReflectable>      // shouldn't compile at all
   , NotReflectable
   //, NotReflectable&                    // shouldn't compile at all
   //, SheddableType<NotReflectableIntern>// shouldn't compile at all
   , NotReflectableIntern
   //, NotReflectableIntern&              // shouldn't compile at all
   //, SheddableType<NotReflectableExtern>// shouldn't compile at all
   , NotReflectableExtern
   //, NotReflectableExtern&              // shouldn't compile at all
) {
   static_assert(not CT::Reflectable<TestType>);
}

//static_assert(    CT::Reflectable<>); // shouldn't compile at all
static_assert(    CT::Reflectable<ReflectableIntern, ReflectableExtern, bool>);
static_assert(not CT::Reflectable<ReflectableIntern, ReflectableExtern, NotReflectableExtern>);


#define DEFINE_REFLECTAS_TYPE_TEST(WHAT, RESULT) \
   static_assert(::std::same_as<CT::ReflectedAs<WHAT>, RESULT>);

///                                                                           
/// CT::ReflectedAs                                                           
SCENARIO("CT::ReflectedAs") {
   DEFINE_REFLECTAS_TYPE_TEST(void,                void)
   DEFINE_REFLECTAS_TYPE_TEST(void*,               void*)
   DEFINE_REFLECTAS_TYPE_TEST(void const*,         void const*)
   DEFINE_REFLECTAS_TYPE_TEST(void const* const,   void const* const)
   DEFINE_REFLECTAS_TYPE_TEST(nullptr_t,           void)
   DEFINE_REFLECTAS_TYPE_TEST(IncompleteType,      IncompleteType)
   DEFINE_REFLECTAS_TYPE_TEST(int32_t(&)[5],       int32_t(&)[5])
   DEFINE_REFLECTAS_TYPE_TEST(int32_t[5],          int32_t[5])

   DEFINE_REFLECTAS_TYPE_TEST(int,           int)
   DEFINE_REFLECTAS_TYPE_TEST(int&,          int&)
   DEFINE_REFLECTAS_TYPE_TEST(int const,     int const)
   DEFINE_REFLECTAS_TYPE_TEST(int const*,    int const*)
   DEFINE_REFLECTAS_TYPE_TEST(int,           int)
   DEFINE_REFLECTAS_TYPE_TEST(int&,          int&)
   DEFINE_REFLECTAS_TYPE_TEST(int const,     int const)
   DEFINE_REFLECTAS_TYPE_TEST(int*,          int*)
   DEFINE_REFLECTAS_TYPE_TEST(int const*,    int const*)
   DEFINE_REFLECTAS_TYPE_TEST(int const* const* const,    int const* const* const)
   DEFINE_REFLECTAS_TYPE_TEST(int const*&&,  int const*&&)

   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable,          void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable&,         void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable const,    void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable const*,   NotReflectable const*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable,          void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable&,         void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable const,    void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable*,         NotReflectable*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable const*,   NotReflectable const*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectable const*&&, NotReflectable const*&&)
   
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern,          void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern&,         void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern const,    void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern const*,   NotReflectableIntern const*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern,          void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern&,         void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern const,    void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern*,         NotReflectableIntern*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern const*,   NotReflectableIntern const*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern const*&&, NotReflectableIntern const*&&)
   
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern,          void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern&,         void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern const,    void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern const*,   NotReflectableExtern const*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern,          void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern&,         void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern const,    void)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern*,         NotReflectableExtern*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableExtern const*,   NotReflectableExtern const*)
   DEFINE_REFLECTAS_TYPE_TEST(NotReflectableIntern const*&&, NotReflectableIntern const*&&)
   
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern,          char)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern&,         char&)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern const,    char const)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern const*,   char const*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern,          char)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern&,         char&)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern const,    char const)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern*,         char*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern const*,   char const*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableIntern const*&&, char const*&&)

   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern,          char)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern const&,   char const&)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern const,    char const)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern const*,   char const*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern,          char)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern&,         char&)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern const,    char const)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern*,         char*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern const*,   char const*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern const* const* const,   char const* const* const)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableExtern const*&&, char const*&&)

   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf,          ReflectableAsSelf)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf&,         ReflectableAsSelf&)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf const,    ReflectableAsSelf const)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf const*,   ReflectableAsSelf const*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf,          ReflectableAsSelf)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf&,         ReflectableAsSelf&)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf const,    ReflectableAsSelf const)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf*,         ReflectableAsSelf*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf const*,   ReflectableAsSelf const*)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf const* const* const,   ReflectableAsSelf const* const* const)
   DEFINE_REFLECTAS_TYPE_TEST(ReflectableAsSelf const*&&, ReflectableAsSelf const*&&)

   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int>,          CustomPointerType<int>)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int>&,         CustomPointerType<int>&)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int> const,    CustomPointerType<int> const)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int> const*,   CustomPointerType<int> const*)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int>,          CustomPointerType<int>)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int>&,         CustomPointerType<int>&)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int> const,    CustomPointerType<int> const)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int>*,         CustomPointerType<int>*)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int> const*,   CustomPointerType<int> const*)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int> const* const* const,   CustomPointerType<int> const* const* const)
   DEFINE_REFLECTAS_TYPE_TEST(CustomPointerType<int> const*&&, CustomPointerType<int> const*&&)
}
