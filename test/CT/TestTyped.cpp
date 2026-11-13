///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/CT/Typed.hpp>
#include <string_view>
#include <array>
#include <vector>


///                                                                           
/// MARK: Typed/Untyped                                                       
TEST_CASE_TEMPLATE("Testing typed type", TestType
   , std::vector<bool>
   , std::string_view
   , std::array<double, 5>
   , TypedEnum
   , TypedEnumClass
   , CustomTypedType
   , CustomTypedTypeDerived
   , SheddableType<TypedEnum>
) {
   static_assert(    CT::Typed<TestType>);
   static_assert(not CT::Untyped<TestType>);
}

TEST_CASE_TEMPLATE("Testing untyped type", TestType
   , CustomUntypedType
   , SheddableType<int>
   //, IncompleteType // shouldn't compile
   , void, int
) {
   static_assert(not CT::Typed<TestType>);
   static_assert(    CT::Untyped<TestType>);
}

//static_assert(CT::Typed<>); // shouldn't compile at all
static_assert(    CT::Typed<std::vector<bool>, CustomTypedType, TypedEnum>);
static_assert(not CT::Typed<std::vector<bool>, CustomTypedType, int>);

//static_assert(CT::Untyped<>); // shouldn't compile at all
static_assert(    CT::Untyped<CustomUntypedType, void, int>);
static_assert(not CT::Untyped<CustomUntypedType, void, TypedEnum>);


///                                                                           
/// MARK: TypeOf                                                              
TEST_CASE("Testing TypeOf") {
   static_assert(::std::same_as<TypeOf<SheddableType<int>>, void>);
   static_assert(::std::same_as<TypeOf<SheddableType<int&>>, void>);
   static_assert(::std::same_as<TypeOf<SheddableType<int const* const>>, void>);
   static_assert(::std::same_as<TypeOf<CustomTypedType>, int>);
   static_assert(::std::same_as<TypeOf<int>, void>);
   static_assert(::std::same_as<TypeOf<void>, void>);
   static_assert(::std::same_as<TypeOf<::std::nullptr_t>, void>);
   static_assert(::std::same_as<TypeOf<int&>, void>);
   static_assert(::std::same_as<TypeOf<volatile int const* const&>, void>);
   static_assert(::std::same_as<TypeOf<TypedEnum>, int64_t>);
   static_assert(::std::same_as<TypeOf<CustomUntypedType>, void>);
   static_assert(::std::same_as<TypeOf<std::vector<bool>>, bool>);
   static_assert(::std::same_as<TypeOf<std::string_view>, char>);
   static_assert(::std::same_as<TypeOf<std::array<double, 5>>, double>);
   //static_assert(::std::same_as<TypeOf<IncompleteType>, void>); // shouldn't compile
}


///                                                                           
/// MARK: TypedCast                                                           
TEST_CASE_TEMPLATE("Testing TypedCast", TestType //TODO test with sheddable, make sure they shed
   , int
   , int&&
   , const int&
   , TypedTypeCastableExplicit<int>
   , TypedTypeCastableExplicit<int&&>
   , TypedTypeCastableExplicit<const int&>
   , TypedTypeCastableImplicit<int>
   , TypedTypeCastableImplicit<int&&>
   , TypedTypeCastableImplicit<const int&>
   , TypedTypeCastableUsingMethod<const int&>
   , const TypedTypeCastableExplicit<int>
   , const TypedTypeCastableExplicit<int&&>
   , const TypedTypeCastableExplicit<const int&>
   , const TypedTypeCastableImplicit<int>
   , const TypedTypeCastableImplicit<int&&>
   , const TypedTypeCastableImplicit<const int&>
   , const TypedTypeCastableUsingMethod<const int&>
) {
   int value = 656;
   TestType i {::std::move(value)};

   if constexpr (CT::Typed<TestType>)
      static_assert(::std::same_as<decltype(TypedCast(i)), TypeOf<TestType>>);
   else
      static_assert(::std::same_as<decltype(TypedCast(i)), TestType&>);
}

///                                                                           
/// MARK: SparseCast                                                          
TEST_CASE_TEMPLATE("Testing SparseCast", TestType //TODO test with typed, make sure they don't shed
   , int
   , int&&
   , const int&
   , const int* const&
   , const int* const* const&
   , const int* const* const* const&
   , const int* const* const*
   //, SheddableTypeCastableExplicit<int>    // shouldn't compile, can't get a pointer out of rvalue
   //, SheddableTypeCastableExplicit<int&&>  // shouldn't compile, can't get a pointer out of rvalue
   , SheddableTypeCastableExplicit<const int&>
   , SheddableTypeCastableExplicit<const int* const&>
   , SheddableTypeCastableExplicit<const int* const* const&>
   , SheddableTypeCastableExplicit<const int* const* const* const&>
   , SheddableTypeCastableExplicit<const int* const* const*>
   //, const SheddableTypeCastableExplicit<int>    // shouldn't compile, because operator returns temporary
   //, const SheddableTypeCastableExplicit<int&&>  // shouldn't compile, can't get a pointer out of rvalue
   , const SheddableTypeCastableExplicit<const int&>
   , const SheddableTypeCastableExplicit<const int* const&>
   , const SheddableTypeCastableExplicit<const int* const* const&>
   , const SheddableTypeCastableExplicit<const int* const* const* const&>
   , const SheddableTypeCastableExplicit<const int* const* const*>
) {
   int value = 656;

   if constexpr (CT::Dense<TestType>) {
      TestType i {::std::move(value)};

      static_assert(    CT::Sparse   <decltype(SparseCast(i))>);
      static_assert(not CT::Sheddable<decltype(SparseCast(i))>);
   }
   else if constexpr (IndirectsOf<TestType> == 1) {
      TestType i {&value};

      static_assert(    CT::Sparse   <decltype(SparseCast(i))>);
      static_assert(not CT::Sheddable<decltype(SparseCast(i))>);
   }
   else if constexpr (IndirectsOf<TestType> == 2) {
      int* vp = &value;
      TestType i {&vp};

      static_assert(    CT::Sparse   <decltype(SparseCast(i))>);
      static_assert(not CT::Sheddable<decltype(SparseCast(i))>);
   }
   else if constexpr (IndirectsOf<TestType> == 3) {
      int* vp = &value;
      int** vpp = &vp;
      int*** vppp = &vpp;
      TestType i {const_cast<Shed<TestType>&>(vppp)};

      static_assert(    CT::Sparse   <decltype(SparseCast(i))>);
      static_assert(not CT::Sheddable<decltype(SparseCast(i))>);
   }
   else static_assert(false, "Unhandled case");
}


///                                                                           
/// MARK: DenseCast                                                           
TEST_CASE_TEMPLATE("Testing DenseCast", TestType //TODO test with typed, make sure they don't shed
   , int
   //, int&&
   , const int&
   , const int* const&
   , const int* const* const&
   , const int* const* const* const&
   , const int* const* const*
   , SheddableTypeCastableExplicit<int>
   , SheddableTypeCastableExplicit<int&&>
   , SheddableTypeCastableExplicit<const int&>
   , SheddableTypeCastableExplicit<const int* const&>
   , SheddableTypeCastableExplicit<const int* const* const&>
   , SheddableTypeCastableExplicit<const int* const* const* const&>
   , SheddableTypeCastableExplicit<const int* const* const*>
   , const SheddableTypeCastableExplicit<int>
   , const SheddableTypeCastableExplicit<int&&>
   , const SheddableTypeCastableExplicit<const int&>
   , const SheddableTypeCastableExplicit<const int* const&>
   , const SheddableTypeCastableExplicit<const int* const* const&>
   , const SheddableTypeCastableExplicit<const int* const* const* const&>
   , const SheddableTypeCastableExplicit<const int* const* const*>
) {
   int value = 656;

   if constexpr (CT::Dense<TestType>) {
      TestType i {value};

      static_assert(    CT::Dense    <decltype(DenseCast(i))>);
      static_assert(not CT::Sheddable<decltype(DenseCast(i))>);
   }
   else if constexpr (IndirectsOf<TestType> == 1) {
      TestType i {&value};

      static_assert(    CT::Dense    <decltype(DenseCast(i))>);
      static_assert(not CT::Sheddable<decltype(DenseCast(i))>);
   }
   else if constexpr (IndirectsOf<TestType> == 2) {
      int* vp = &value;
      TestType i {&vp};

      static_assert(    CT::Dense    <decltype(DenseCast(i))>);
      static_assert(not CT::Sheddable<decltype(DenseCast(i))>);
   }
   else if constexpr (IndirectsOf<TestType> == 3) {
      int* vp = &value;
      int** vpp = &vp;
      int*** vppp = &vpp;
      TestType i {const_cast<Shed<TestType>&>(vppp)};

      static_assert(    CT::Dense    <decltype(DenseCast(i))>);
      static_assert(not CT::Sheddable<decltype(DenseCast(i))>);
   }
   else static_assert(false, "Unhandled case");
}
