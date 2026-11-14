///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/CT/Destroyable.hpp>
#include <Langulus/Utils/Byte.hpp>

//TODO test extents


///                                                                           
/// CT::Destroyable                                                           
TEST_CASE_TEMPLATE("Testing CT::Destroyable types", T
   //, IncompleteType                     // shouldn't compile
   //, IncompleteType const               // shouldn't compile
   //, SheddableType<IncompleteType>      // shouldn't compile
   //, SheddableType<PureAbstract>        // shouldn't compile
   //, SheddableType<PureAbstract const>  // shouldn't compile
   //, SheddableType<PureAbstract const&> // shouldn't compile
   , Complex
   , ContainsComplex
   , PureAbstract
   , ImpureVirtual
   , VirtuallyDerived
) {
   static_assert(    CT::Destroyable<T>);
   static_assert(not CT::NotDestroyable<T>);
}

TEST_CASE_TEMPLATE("Testing CT::NotDestroyable types", T
   //, IncompleteType                // shouldn't compile
   //, IncompleteType const          // shouldn't compile
   //, SheddableType<IncompleteType> // shouldn't compile
   , EmptyType
   , NonDestructible
   , PrivatelyConstructible
   , NonIntentConstructible
   , AllIntentConstructible
   , AllIntentConstructibleAndAssignable
   , PartiallyIntentConstructible
   , DescribeConstructible
   , bool, uint32_t, double, char, wchar_t, char8_t, Langulus::Byte
   , Complex*, ContainsComplex*
   , ForcedAbstractExternally
   , ForcedAbstractInternally
) {
   static_assert(not CT::Destroyable<T>);
   static_assert(    CT::NotDestroyable<T>);
}

//static_assert(    CT::Destroyable<>); // shouldn't compile at all
static_assert(    CT::Destroyable<VirtuallyDerived, Complex, PureAbstract>);
static_assert(not CT::Destroyable<EmptyType, Complex, bool>);

//static_assert(    CT::NotDestroyable<>); // shouldn't compile at all
static_assert(    CT::NotDestroyable<uint32_t, NonDestructible, ForcedAbstractExternally>);
static_assert(not CT::NotDestroyable<VirtuallyDerived, NonDestructible, EmptyType>);
