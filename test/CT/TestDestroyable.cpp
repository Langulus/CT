///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/CT/Destroyable.hpp>

//TODO test extents


///                                                                           
/// CT::Destroyable                                                           
TEST_CASE_TEMPLATE("Testing CT::Destroyable types", TestType
   , IncompleteType                     // shouldn't compile
   , IncompleteType const               // shouldn't compile
   , SheddableType<IncompleteType>      // shouldn't compile
   , SheddableType<PureAbstract>        // shouldn't compile
   , SheddableType<PureAbstract const>  // shouldn't compile
   , SheddableType<PureAbstract const&>
   , Destructible
   , Complex
   , ContainsComplex
   , PureVirtual
   , ImpureVirtual
   , VirtuallyDerive
) {
   static_assert(    CT::Destroyable<TestType>);
   static_assert(not CT::NotDestroyable<TestType>);
}

TEST_CASE_TEMPLATE("Testing CT::NotDestroyable types", TestType
   , IncompleteType                // shouldn't compile
   , IncompleteType const          // shouldn't compile
   , SheddableType<IncompleteType> // shouldn't compile
   , EmptyType
   , NonDestructible
   , PrivatelyConstructible
   , NonIntentConstructible
   , AllIntentConstructible
   , AllIntentConstructibleAndAssignable
   , PartiallyIntentConstructible
   , DescriptorConstructible
   , IncompleteType
   , bool, uint32_t, double, char, wchar_t, char8_t, Langulus::Byte
   , Complex*, ContainsComplex*
   , ForcedAbstract
) {
   static_assert(not CT::Destroyable<TestType>);
   static_assert(    CT::NotDestroyable<TestType>);
}

static_assert(    CT::Destroyable<>); // shouldn't compile at all
static_assert(    CT::Destroyable<Destructible, Complex, PureVirtual>);
static_assert(not CT::Destroyable<Destructible, Complex, bool>);

static_assert(    CT::NotDestroyable<>); // shouldn't compile at all
static_assert(    CT::NotDestroyable<uint32_t, NonDestructible, ForcedAbstract>);
static_assert(not CT::NotDestroyable<uint32_t, NonDestructible, Destructible>);
