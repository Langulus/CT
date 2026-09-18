///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Core.hpp>


namespace Langulus
{
   using ::std::size_t;

   template<size_t...IDX>
   using ExpandedSequence = ::std::integer_sequence<size_t, IDX...>;

   ///                                                                        
   ///   Compile-time integer sequences                                       
   ///                                                                        
   template<size_t END>
   struct Sequence {
   protected:
      template<class LAMBDA>
      static consteval bool Noexcept() {
         return noexcept(Fake<LAMBDA&&>().template operator() <0> ());
      }

   public:
      static constexpr size_t Size = END;
      static constexpr bool Empty  = END == 0;
      static constexpr auto Expand = ::std::make_integer_sequence<size_t, END> {};

      /// Iterate through each index in the sequence using generator pattern  
      ///   @param generator - a templated lambda function                    
      /// Example use:                                                        
      ///   Sequence<Ret::Columns>::ForEach([&]<size_t COL>() noexcept {      
      ///      auto& lc = lhs.template GetColumn<COL>();                      
      ///      Sequence<Ret::Rows>::ForEach([&]<size_t ROW>() noexcept {      
      ///         *(r++) = (lc * rhs.template GetRow<ROW>()).HSum();          
      ///      });                                                            
      ///   });                                                               
      template<class LAMBDA> LANGULUS(INLINED)
      static constexpr void ForEach(LAMBDA&& generator) noexcept(Noexcept<LAMBDA>()) {
         [&]<size_t...IDX>(ExpandedSequence<IDX...>) noexcept(Noexcept<LAMBDA>()) {
            (generator.template operator() <IDX> (), ...);
         }(Expand);
      }
   };

} // namespace Langulus
