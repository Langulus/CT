///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/CT/Defaultable.hpp>

//TODO test extents

///                                                                           
/// CT::Defaultable                                                           
TEST_CASE_TEMPLATE("Testing CT::Defaultable types", TestType
   , IncompleteType                     // shouldn't compile
   , IncompleteType const               // shouldn't compile
   , SheddableType<IncompleteType>      // shouldn't compile
   , SheddableType<PureAbstract>        // shouldn't compile
   , SheddableType<PureAbstract const>  // shouldn't compile
   , SheddableType<PureAbstract const&>
   , bool, uint32_t, double, char, wchar_t, char8_t, Langulus::Byte
   , EmptyType
   , Destructible
   , AggregateType
   , Complex*, ContainsComplex*
) {
   static_assert(    CT::Defaultable<TestType>);
   static_assert(not CT::NotDefaultable<TestType>);
}

TEST_CASE_TEMPLATE("Testing CT::NotDefaultable types", TestType
   , IncompleteType                // shouldn't compile
   , IncompleteType const          // shouldn't compile
   , SheddableType<IncompleteType> // shouldn't compile
   , NonDestructible
   , PrivatelyConstructible
   , NonIntentConstructible
   , AllIntentConstructible
   , AllIntentConstructibleAndAssignable
   , PartiallyIntentConstructible
   , DescriptorConstructible
   , Complex
   , ContainsComplex
   , IncompleteType
   , ForcefullyPod
   , PureVirtual
   , ForcedAbstract
) {
   static_assert(not CT::Defaultable<TestType>);
   static_assert(    CT::NotDefaultable<TestType>);
}

static_assert(    CT::Defaultable<>); // shouldn't compile at all
static_assert(    CT::Defaultable<bool, AggregateType, EmptyType>);
static_assert(not CT::Defaultable<bool, AggregateType, Complex>);

static_assert(    CT::NotDefaultable<>); // shouldn't compile at all
static_assert(    CT::NotDefaultable<PrivatelyConstructible, Complex, ForcedAbstract>);
static_assert(not CT::NotDefaultable<PrivatelyConstructible, Complex, bool>);
