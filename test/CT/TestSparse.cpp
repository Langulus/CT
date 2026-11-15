///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/CT/Abstract.hpp>


///                                                                           
/// MARK: Sparse/Dense                                                        
TEST_CASE_TEMPLATE("Testing sparse types", TestType
   , SheddableType<PointerType>
   , PointerType
   , CustomPointerType<int>
   , CustomPointerType<int>**
   , IncompleteType*
   , void**
   , nullptr_t
) {
   static_assert(    CT::Sparse<TestType>);
   static_assert(not CT::Dense<TestType>);
}

TEST_CASE_TEMPLATE("Testing dense types", TestType
   , IncompleteType
   , SheddableType<EmptyType>
   , TypedType<CustomPointerType<int>>
   , TypedType<int*>
   , EmptyType
   , int, int&, void
   , int(&)[5], int[5], bool[5][5]
   , Types<int*>
) {
   static_assert(not CT::Sparse<TestType>);
   static_assert(    CT::Dense<TestType>);
}

//static_assert(CT::Sparse<>); // shouldn't compile at all
static_assert(    CT::Sparse<SheddableType<PointerType>, PointerType, CustomPointerType<int>>);
static_assert(not CT::Sparse<SheddableType<PointerType>, PointerType, int>);

//static_assert(CT::Dense<>); // shouldn't compile at all
static_assert(    CT::Dense<SheddableType<EmptyType>, EmptyType, int>);
static_assert(not CT::Dense<SheddableType<EmptyType>, EmptyType, int*>);


///                                                                           
/// MARK: CustomPointer                                                       
TEST_CASE_TEMPLATE("Testing custom pointer types", TestType
   , SheddableType<CustomPointerType<int>>
   , CustomPointerType<int>
) {
   static_assert(CT::CustomPointer<TestType>);
}

TEST_CASE_TEMPLATE("Testing not custom pointer types", TestType
   , SheddableType<PointerType>
   , PointerType
   , CustomPointerType<int>**
   , IncompleteType*
   , void**
   , nullptr_t
) {
   static_assert(not CT::CustomPointer<TestType>);
}

//static_assert(    CT::CustomPointer<>); // shouldn't compile at all
static_assert(    CT::CustomPointer<SheddableType<CustomPointerType<int>>, CustomPointerType<int>, CustomPointerType<int>>);
static_assert(not CT::CustomPointer<SheddableType<CustomPointerType<int>>, CustomPointerType<int>, CustomPointerType<int>*>);


///                                                                           
/// MARK: Deptr                                                               
static_assert(::std::same_as<Deptr<SheddableType<int**>>, int*>);
static_assert(::std::same_as<Deptr<SheddableType<int*>>, int>);
static_assert(::std::same_as<Deptr<SheddableType<int>*>, SheddableType<int>>);
static_assert(::std::same_as<Deptr<SheddableType<CustomPointerType<int>>>, int>);
static_assert(::std::same_as<Deptr<CustomPointerType<int>>, int>);

static_assert(::std::same_as<Deptr<int>,         int>);
static_assert(::std::same_as<Deptr<int&>,        int>);
static_assert(::std::same_as<Deptr<int&&>,       int>);
static_assert(::std::same_as<Deptr<CustomPointerType<int>&&>, int>);
static_assert(::std::same_as<Deptr<int const&>,  int const>);
static_assert(::std::same_as<Deptr<int const&&>, int const>);
static_assert(::std::same_as<Deptr<int(&)[15]>,  int[15]>);
static_assert(::std::same_as<Deptr<CustomPointerType<int>(&)[15]>,  CustomPointerType<int>[15]>);

static_assert(::std::same_as<Deptr<int*>,        int>);
static_assert(::std::same_as<Deptr<int const*>,  int const>);
static_assert(::std::same_as<Deptr<CustomPointerType<int> const*>,  CustomPointerType<int> const>);
static_assert(::std::same_as<Deptr<int*&>,       int>);
static_assert(::std::same_as<Deptr<int*&&>,      int>);
static_assert(::std::same_as<Deptr<int const*&>, int const>);
static_assert(::std::same_as<Deptr<int const*&&>,int const>);
static_assert(::std::same_as<Deptr<int(*)[15]>,  int[15]>);
static_assert(::std::same_as<Deptr<int*(*)[15]>, int*[15]>);
static_assert(::std::same_as<Deptr<int*[15]>,    int*[15]>);
static_assert(::std::same_as<Deptr<CustomPointerType<int>*[15]>,    CustomPointerType<int>*[15]>);

static_assert(::std::same_as<Deptr<int**>,   int*>);
static_assert(::std::same_as<Deptr<int**&>,  int*>);
static_assert(::std::same_as<Deptr<int**&&>, int*>);
static_assert(::std::same_as<Deptr<int const**&>,  int const*>);
static_assert(::std::same_as<Deptr<int const**&&>, int const*>);
static_assert(::std::same_as<Deptr<int const* const*&>,  int const* const>);
static_assert(::std::same_as<Deptr<int const* const*&&>, int const* const>);

static_assert(::std::same_as<Deptr<SheddableType<int**>, 2>, int>);
static_assert(::std::same_as<Deptr<SheddableType<int*>, 2>, int>);
static_assert(::std::same_as<Deptr<SheddableType<int>*, 2>, SheddableType<int>>);
static_assert(::std::same_as<Deptr<SheddableType<CustomPointerType<int>>*, 2>, SheddableType<CustomPointerType<int>>>);

static_assert(::std::same_as<Deptr<int, 2>,   int>);
static_assert(::std::same_as<Deptr<int&, 2>,  int>);
static_assert(::std::same_as<Deptr<int&&, 2>, int>);
static_assert(::std::same_as<Deptr<CustomPointerType<int>&&, 2>, int>);
static_assert(::std::same_as<Deptr<int const&, 2>,  int const>);
static_assert(::std::same_as<Deptr<int const&&, 2>, int const>);
static_assert(::std::same_as<Deptr<int(&)[15], 2>,  int[15]>);

static_assert(::std::same_as<Deptr<int*, 2>,         int>);
static_assert(::std::same_as<Deptr<int*&, 2>,        int>);
static_assert(::std::same_as<Deptr<int*&&, 2>,       int>);
static_assert(::std::same_as<Deptr<CustomPointerType<int>*&&, 2>,       int>);
static_assert(::std::same_as<Deptr<int const*&, 2>,  int const>);
static_assert(::std::same_as<Deptr<int const*&&, 2>, int const>);
static_assert(::std::same_as<Deptr<int(*)[15], 2>,   int[15]>);
static_assert(::std::same_as<Deptr<int*(*)[15], 2>,  int*[15]>);
static_assert(::std::same_as<Deptr<int*[15], 2>,     int*[15]>);
static_assert(::std::same_as<Deptr<CustomPointerType<int>*[15], 2>,     CustomPointerType<int>*[15]>);

static_assert(::std::same_as<Deptr<int**, 2>,   int>);
static_assert(::std::same_as<Deptr<int**&, 2>,  int>);
static_assert(::std::same_as<Deptr<int**&&, 2>, int>);
static_assert(::std::same_as<Deptr<CustomPointerType<int>**&&, 2>, CustomPointerType<int>>);
static_assert(::std::same_as<Deptr<int const**&, 2>,  int const>);
static_assert(::std::same_as<Deptr<int const**&&, 2>, int const>);
static_assert(::std::same_as<Deptr<int const* const*&, 2>,  int const>);
static_assert(::std::same_as<Deptr<int const* const*&&, 2>, int const>);


///                                                                           
/// MARK: DeptrAll                                                            
static_assert(::std::same_as<DeptrAll<SheddableType<int**>>, int>);
static_assert(::std::same_as<DeptrAll<SheddableType<int*>>, int>);
static_assert(::std::same_as<DeptrAll<SheddableType<int>*>, SheddableType<int>>);
static_assert(::std::same_as<DeptrAll<SheddableType<CustomPointerType<int>>>, int>);
static_assert(::std::same_as<DeptrAll<CustomPointerType<int>>, int>);

static_assert(::std::same_as<DeptrAll<int>,         int>);
static_assert(::std::same_as<DeptrAll<int&>,        int>);
static_assert(::std::same_as<DeptrAll<int&&>,       int>);
static_assert(::std::same_as<DeptrAll<CustomPointerType<int>&&>, int>);
static_assert(::std::same_as<DeptrAll<int const&>,  int const>);
static_assert(::std::same_as<DeptrAll<int const&&>, int const>);
static_assert(::std::same_as<DeptrAll<int(&)[15]>,  int[15]>);
static_assert(::std::same_as<DeptrAll<CustomPointerType<int>(&)[15]>,  CustomPointerType<int>[15]>);

static_assert(::std::same_as<DeptrAll<int*>,        int>);
static_assert(::std::same_as<DeptrAll<int const*>,  int const>);
static_assert(::std::same_as<DeptrAll<CustomPointerType<int> const*>,  int>);
static_assert(::std::same_as<DeptrAll<int*&>,       int>);
static_assert(::std::same_as<DeptrAll<int*&&>,      int>);
static_assert(::std::same_as<DeptrAll<int const*&>, int const>);
static_assert(::std::same_as<DeptrAll<int const*&&>,int const>);
static_assert(::std::same_as<DeptrAll<int(*)[15]>,  int[15]>);
static_assert(::std::same_as<DeptrAll<int*(*)[15]>, int*[15]>);
static_assert(::std::same_as<DeptrAll<int*[15]>,    int*[15]>);
static_assert(::std::same_as<DeptrAll<CustomPointerType<int>*[15]>,    CustomPointerType<int>*[15]>);

static_assert(::std::same_as<DeptrAll<int**>,   int>);
static_assert(::std::same_as<DeptrAll<int**&>,  int>);
static_assert(::std::same_as<DeptrAll<int**&&>, int>);
static_assert(::std::same_as<DeptrAll<int const**&>,  int const>);
static_assert(::std::same_as<DeptrAll<int const**&&>, int const>);
static_assert(::std::same_as<DeptrAll<int const* const*&>,  int const>);
static_assert(::std::same_as<DeptrAll<int const* const*&&>, int const>);


///                                                                           
/// MARK: IndirectsOf                                                         
static_assert(IndirectsOf<SheddableType<int>> == 0);
static_assert(IndirectsOf<SheddableType<int* const>> == 1);
static_assert(IndirectsOf<SheddableType<int>* const volatile> == 1);
static_assert(IndirectsOf<SheddableType<int*>* const volatile> == 2);
static_assert(IndirectsOf<SheddableType<CustomPointerType<int>>* const volatile> == 2);
static_assert(IndirectsOf<SheddableType<CustomPointerType<int>*>* const volatile> == 3);

static_assert(IndirectsOf<int> == 0);
static_assert(IndirectsOf<int&> == 0);
static_assert(IndirectsOf<int&&> == 0);
static_assert(IndirectsOf<CustomPointerType<int>&&> == 1);
static_assert(IndirectsOf<int(&)[15]> == 0);
static_assert(IndirectsOf<int[15]> == 0);
static_assert(IndirectsOf<CustomPointerType<int>[15]> == 0);

static_assert(IndirectsOf<int*> == 1);
static_assert(IndirectsOf<int**> == 2);
static_assert(IndirectsOf<int***> == 3);
static_assert(IndirectsOf<int***&> == 3);
static_assert(IndirectsOf<int***&&> == 3);
static_assert(IndirectsOf<CustomPointerType<int>***&&> == 4);
static_assert(IndirectsOf<int const* const* const* const> == 3);

//TODO ForEachIndirection