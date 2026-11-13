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
/// MARK: ShedCast                                                            
TEST_CASE_TEMPLATE("Testing ShedCast", TestType
   , int
   , const int&
   , SheddableTypeCastableExplicit<int>
   , SheddableTypeCastableExplicit<int&&>
   , SheddableTypeCastableExplicit<const int&>
   , SheddableTypeCastableImplicit<int>
   , SheddableTypeCastableImplicit<int&&>
   , SheddableTypeCastableImplicit<const int&>
   , const SheddableTypeCastableExplicit<int>
   , const SheddableTypeCastableExplicit<int&&>
   , const SheddableTypeCastableExplicit<const int&>
   , const SheddableTypeCastableImplicit<int>
   , const SheddableTypeCastableImplicit<int&&>
   , const SheddableTypeCastableImplicit<const int&>
) {
   int value = 656;
   TestType i {value};

   static_assert(not CT::Sheddable<decltype(ShedCast(i))>);
}

TEST_CASE_TEMPLATE("Testing nested ShedCast", TestType
   , SheddableTypeCastableExplicit<SheddableTypeCastableExplicit<int>>
   , SheddableTypeCastableExplicit<SheddableTypeCastableExplicit<const int> const> const
) {
   using ST = typename decltype(CT::Inner::GetSheddedType<TestType>())::type;
   int value = 656;
   ST inner {value};
   TestType i {inner};

   static_assert(    CT::Sheddable<ST>);
   static_assert(not CT::Sheddable<decltype(ShedCast(i))>);
}