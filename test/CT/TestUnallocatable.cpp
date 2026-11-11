///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/NameOf.hpp>
#include <Langulus/CT/Unallocatable.hpp>

namespace
{
   struct AllocatableExternally {};
   struct AllocatableExternallyDisabled {};
   struct AllocatableInternally { using CTTI_Unallocatable = No; };
   struct InheritedAllocatable : AllocatableInternally {};
   struct InheritedAllocatableDisabled : AllocatableInternally { using CTTI_Unallocatable = Yup; };
   struct InheritedAllocatableButPrivate : private AllocatableInternally {};
   struct InheritedAllocatableExternally : AllocatableExternally {};
}

namespace Langulus::CTTI
{
   template<>
   struct Unallocatable<AllocatableExternally> : No {};

   template<>
   struct Unallocatable<AllocatableExternallyDisabled> {};
}

//TODO test extents

///                                                                           
/// CT::Unallocatable                                                         
TEST_CASE_TEMPLATE("Testing CT::Unallocatable types", TestType
   //, IncompleteType                  // shouldn't compile
   //, IncompleteType const            // shouldn't compile
   //, IncompleteType const&           // shouldn't compile
   //, SheddableType<IncompleteType>   // shouldn't compile
   //, SheddableType<IncompleteType>&  // shouldn't compile
   , AllocatableExternally
   , AllocatableExternally const
   , AllocatableExternally&
   , AllocatableInternally
   , AllocatableInternally const
   , AllocatableInternally&
   , SheddableType<AllocatableInternally*>
   , SheddableType<AllocatableInternally* const>
   , SheddableType<AllocatableInternally* const&>
   , InheritedAllocatable
   , InheritedAllocatable const
   , InheritedAllocatable&
   , InheritedAllocatableButPrivate
   , InheritedAllocatableExternally
   , void*
   , int, int const, int const&, int&
   , Types<void*>
   , SheddableType<InheritedAllocatableDisabled*>
   , SheddableType<InheritedAllocatableDisabled* const>
   , SheddableType<InheritedAllocatableDisabled* const&>
   , IncompleteType*
) {
   static_assert(    CT::NotUnallocatable<TestType>);
   static_assert(not CT::Unallocatable<TestType>);
}

TEST_CASE_TEMPLATE("Testing CT::NotUnallocatable types", TestType
   //, IncompleteType                    // shouldn't compile
   //, IncompleteType const              // shouldn't compile
   //, IncompleteType const&             // shouldn't compile
   //, SheddableType<IncompleteType>     // shouldn't compile
   //, SheddableType<IncompleteType>&    // shouldn't compile
   , void
   , SheddableType<InheritedAllocatableDisabled>
   , SheddableType<InheritedAllocatableDisabled const>
   , SheddableType<InheritedAllocatableDisabled const&>
   , InheritedAllocatableDisabled
   , AllocatableExternallyDisabled
) {
   static_assert(not CT::NotUnallocatable<TestType>,
      CppNameOf<TestType>() + " shouldn't be allocatable but is");
   static_assert(    CT::Unallocatable<TestType>,
      CppNameOf<TestType>() + " shouldn't be allocatable but is");
}

//static_assert(    CT::Allocatable<>); // shouldn't compile at all
static_assert(    CT::NotUnallocatable<AllocatableExternally, AllocatableInternally, int>);
static_assert(not CT::NotUnallocatable<AllocatableExternally, AllocatableInternally, InheritedAllocatableDisabled>);

//static_assert(    CT::NotAllocatable<>); // shouldn't compile at all
static_assert(    CT::Unallocatable<void, InheritedAllocatableDisabled, AllocatableExternallyDisabled>);
static_assert(not CT::Unallocatable<void, InheritedAllocatableDisabled, int>);
