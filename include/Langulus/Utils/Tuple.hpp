/// Optimal layout tuple                                                      
/// Written in 2012 by Martinho Fernandes <martinho.fernandes@gmail.com>      
/// Modified and modernized for C++23 in 2025 by Dimo Markov                  
/// <team@langulus.com>. Changes made:                                        
///  - making tuple fully constexpr                                           
///  - using 'requires' instead of 'std::enable_if' patterns                  
///  - using concepts for some require checks                                 
///                                                                           
/// SPDX-License-Identifier: CC0-1.0                                          
#pragma once
#include <Langulus/Core.hpp>
#include <type_traits>


namespace Langulus
{
   namespace Inner
   {
      struct absorb_another_tuple {};

      template<class...T>
      concept not_absorbing = ((not ::std::is_same_v<T, absorb_another_tuple>) and ...);
   }


   ///                                                                        
   /// We have tuple at home                                                  
   ///                                                                        
   /// STD implementation of tuple is crap, because:                          
   ///   1. it uses private bases which disables the use of tuples inside     
   ///      non-type template arguments (whyyy (╯°□°）╯︵ ┻━┻)                
   ///   2. default-initializes all contained data (╥﹏╥)                     
   template<class...T>
   struct Tuple;

   /// Empty tuple is empty                                                   
   template<>
   struct Tuple<> {
      using CTTI_Void  = Yup;
      using CTTI_Tuple = Yup;
      static constexpr size_t Size = 0;
   };

   /// Tuple with one element just contains it as a variable                  
   template<class T>
   struct Tuple<T> {
      using CTTI_Tuple = Yup;

      T first;

      static constexpr size_t Size = 1;

      constexpr Tuple() noexcept {}

      template<class K1> requires Inner::not_absorbing<K1>
      constexpr Tuple(K1&& arg1)
         : first {LglsFwd(arg1)} {}

      template<class K1>
      constexpr Tuple(Inner::absorb_another_tuple, Tuple<K1>&& tuple)
         : first {LglsFwd(tuple.first)} {}
   };

   /// Tuple with multiple elements incorporates each new element inside      
   /// nested bases. Some of the specializations have more types as an        
   /// optimization when accessing values further towards the end.            
   //TODO test if these actually affect build time and RAM usage (use percist)
   //TODO experiment with structured binding to reduce number of bases        
   //TODO - elements can be combined in the same struct                       
   /*template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class...TN>
   requires (sizeof...(TN) > 0)
   struct Tuple<T1, T2, T3, T4, T5, T6, T7, T8, TN...> : Tuple<T2, T3, T4, T5, T6, T7, T8, TN...> {
      using CTTI_Tuple = Yup;

      T1 first;

      static constexpr size_t Size = sizeof...(TN) + 8;
      using next8 = Tuple<TN...>;
      using next4 = Tuple<T5, T6, T7, T8, TN...>;
      using next2 = Tuple<T3, T4, T5, T6, T7, T8, TN...>;
      using next1 = Tuple<T2, T3, T4, T5, T6, T7, T8, TN...>;

      constexpr Tuple() noexcept {}

      template<class...K>
      constexpr Tuple(K&&...arguments)
         : Tuple<T2, T3, T4, T5, T6, T7, T8, TN...> {LglsFwd(arguments)...} {}

   };

   template<class T1, class T2, class T3, class T4, class...TN>
   requires (sizeof...(TN) > 0)
   struct Tuple<T1, T2, T3, T4, TN...> : Tuple<T2, T3, T4, TN...> {
      using CTTI_Tuple = Yup;

      T1 first;

      static constexpr size_t Size = sizeof...(TN) + 4;
      using next4 = Tuple<TN...>;
      using next2 = Tuple<T3, T4, TN...>;
      using next1 = Tuple<T2, T3, T4, TN...>;
   };

   template<class T1, class T2, class...TN>
   requires (sizeof...(TN) > 0)
   struct Tuple<T1, T2, TN...> : Tuple<T2, TN...> {
      using CTTI_Tuple = Yup;

      T1 first;

      static constexpr size_t Size = sizeof...(TN) + 2;
      using next2 = Tuple<TN...>;
      using next1 = Tuple<T2, TN...>;
   };*/

   template<class T1, class...TN> requires (sizeof...(TN) > 0)
   struct Tuple<T1, TN...> : Tuple<TN...> {
      using CTTI_Tuple = Yup;

      T1 first;

      static constexpr size_t Size = sizeof...(TN) + 1;
      using next1 = Tuple<TN...>;

      constexpr Tuple() noexcept {}

      template<class K1, class...KN> requires Inner::not_absorbing<K1, KN...>
      constexpr Tuple(K1&& arg1, KN&&...argn)
         : Tuple<TN...> {LglsFwd(argn)...}
         , first        {LglsFwd(arg1)   } {}

      template<class K1, class...KN>
      constexpr Tuple(Inner::absorb_another_tuple, Tuple<K1, KN...>&& tuple)
         : Tuple<TN...> {Inner::absorb_another_tuple{}, ::std::forward<Tuple<KN...>>(tuple)}
         , first        {LglsFwd(tuple.first)} {}
   };

   template<class...T>
   Tuple(T&&...) -> Tuple<T&&...>;


   /// Get the value at a specific index inside the tuple                     
   ///   @tparam PURE_TYPE true can be used to extract the type with which    
   ///      tuple was declared with. false returns a reference to the member, 
   ///      as it is accessed from the provided instance.                     
   template<size_t I, bool PURE_TYPE = false, class T>
   requires requires { typename ::std::decay_t<T>::CTTI_Tuple; }
   constexpr decltype(auto) TupleGet(T&& tuple) noexcept {
      constexpr bool constant = ::std::is_const_v<::std::remove_reference_t<T>>;
      using DT = ::std::decay_t<T>;
      static_assert(I < DT::Size, "Index is out of tuple range");
      /*if constexpr (I > 8) {
         using next = ::std::conditional_t<constant, typename DT::next8 const,
                                                     typename DT::next8>;
         return TupleGet<I - 8, PURE_TYPE>(::std::forward<next>(tuple));
      }
      else if constexpr (I > 4) {
         using next = ::std::conditional_t<constant, typename DT::next4 const,
                                                     typename DT::next4>;
         return TupleGet<I - 4, PURE_TYPE>(::std::forward<next>(tuple));
      }
      else if constexpr (I > 2) {
         using next = ::std::conditional_t<constant, typename DT::next2 const,
                                                     typename DT::next2>;
         return TupleGet<I - 2, PURE_TYPE>(::std::forward<next>(tuple));
      }
      else*/ if constexpr (I > 0) {
         using next = ::std::conditional_t<constant, typename DT::next1 const,
                                                     typename DT::next1>;
         return TupleGet<I - 1, PURE_TYPE>(::std::forward<next>(tuple));
      }
      else if constexpr (PURE_TYPE)
         return tuple.first;     // Access the declared type            
      else
         return (tuple.first);   // Access the value                    
   }

   /// Get the type of the value at a specific index inside the tuple         
   template<size_t I, class T>
   using TupleTypeAt = decltype(TupleGet<I, true>(::std::declval<::std::decay_t<T>>()));

   /// Shuffle a tuple by a new index sequence                                
   template<class T, size_t...I>
   using ShuffleTuple = Tuple<TupleTypeAt<I, T>...>;
}

#include <array>

namespace Langulus::Inner
{
   template<class T>
   consteval size_t alignof_nonref() {
      return ::std::is_reference_v<T> ? alignof(void*)
                                      : alignof(T);
   }

   /// Compile-time max alignment function                                    
   ///   @attention references get aligned to void*                           
   template<size_t MAX, class T1, class...TN>
   consteval size_t find_max_alignment() {
      constexpr size_t t1_alignment = alignof_nonref<T1>();
      if constexpr (sizeof...(TN) == 0)
         return MAX > t1_alignment ? MAX : t1_alignment;
      else if constexpr (MAX > t1_alignment)
         return find_max_alignment<MAX, TN...>();
      else
         return find_max_alignment<t1_alignment, TN...>();
   }
   
   /// Sort a tuple by alignment                                              
   /// Elements with the largest alignment go to the front of the tuple.      
   /// Smallest elements go to the back of the tuple (most inner base, so     
   /// that their members appear earliest in final layout).                   
   template<class...T>
   consteval auto SortTupleByAlignmentInner() {
      std::array<size_t, sizeof...(T)> indices;
      size_t writer = 0;
      size_t counter = 0;
      size_t alignment = find_max_alignment<1, T...>();
      const auto search = [&]<class TYPE> {
         if (alignof_nonref<TYPE>() == alignment)
            indices[writer++] = counter++;
         else
            ++counter;
      };
      while (writer < sizeof...(T)) {
         (search.template operator()<T>(), ...);
         counter = 0;
         alignment /= 2;
      }
      return indices;
   }

   template<class...T, std::array INDICES = SortTupleByAlignmentInner<T...>()>
   consteval auto SortTupleByAlignment(Tuple<T...>&&) {
      using TUPLE = Tuple<T...>;
      return [&]<size_t...I>(::std::index_sequence<I...>) {
         return ::std::type_identity<Tuple<TupleTypeAt<INDICES[I], TUPLE>...>> {};
      } (::std::make_index_sequence<sizeof...(T)>());
   }

   template<std::array A>
   consteval auto FlipArray() {
      std::array result = A;
      for (size_t i = 0; i < A.size(); ++i)
         result[A[i]] = i;
      return result;
   }

   template<std::array INDICES, class...T>
   constexpr auto ShuffleTupleAndForward(T&&...arg) {
      Tuple<T&&...> temp {LglsFwd(arg)...};
      return [&temp]<size_t...I>(::std::index_sequence<I...>) {
         return Tuple {LglsFwd(TupleGet<INDICES[I]>(temp))...};
      } (::std::make_index_sequence<sizeof...(T)>());
   }
}

namespace Langulus
{
   template<class T>
   using OptimizedTuple = typename decltype(Inner::SortTupleByAlignment(::std::declval<T>()))::type;

   template<class...T>
   struct CompactTuple {
      using CTTI_IndirectTuple = Yup;
      OptimizedTuple<Tuple<T...>> storage;
      static constexpr std::array to_compact = Inner::SortTupleByAlignmentInner<T...>();
      static constexpr std::array to_simple = Inner::FlipArray<to_compact>();

      constexpr CompactTuple() noexcept {}

      template<class...K>
      constexpr CompactTuple(K&&...arguments)
         : storage {Inner::absorb_another_tuple{}, Inner::ShuffleTupleAndForward<to_compact>(LglsFwd(arguments)...)} {}
   };

   /// Get the value at a specific index inside the compact tuple             
   ///   @tparam PURE_TYPE true can be used to extract the type with which    
   ///      tuple was declared with. false returns a reference to the member, 
   ///      as it is accessed from the provided instance.                     
   template<size_t I, bool PURE_TYPE = false, class T>
   requires requires { typename ::std::decay_t<T>::CTTI_IndirectTuple; }
   constexpr decltype(auto) TupleGet(T&& tuple) noexcept {
      using DT = ::std::decay_t<T>;
      return TupleGet<DT::to_simple[I], PURE_TYPE>(tuple.storage);
   }


   //TODO move these to tests
   static_assert(::std::is_same_v<
      OptimizedTuple<Tuple<size_t&, char, void*, size_t, char>>,
                     Tuple<size_t&, void*, size_t, char, char>
   >);

   // Ordinary tuple
   static_assert(::std::is_same_v<
      TupleTypeAt<0, Tuple<size_t&, char, void*, size_t, char>>,
      size_t&
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<1, Tuple<size_t&, char, void*, size_t, char>>,
      char
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<2, Tuple<size_t&, char, void*, size_t, char>>,
      void*
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<3, Tuple<size_t&, char, void*, size_t, char>>,
      size_t
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<4, Tuple<size_t&, char, void*, size_t, char>>,
      char
   >);

   // Ordinary tuple (const)
   static_assert(::std::is_same_v<
      TupleTypeAt<0, const Tuple<size_t&, char, void*, size_t, char>>,
      size_t&
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<1, const Tuple<size_t&, char, void*, size_t, char>>,
      char
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<2, const Tuple<size_t&, char, void*, size_t, char>>,
      void*
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<3, const Tuple<size_t&, char, void*, size_t, char>>,
      size_t
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<4, const Tuple<size_t&, char, void*, size_t, char>>,
      char
   >);

   // Compact tuple
   static_assert(::std::is_same_v<
      TupleTypeAt<0, CompactTuple<size_t&, char, void*, size_t, char>>,
      size_t&
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<1, CompactTuple<size_t&, char, void*, size_t, char>>,
      char
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<2, CompactTuple<size_t&, char, void*, size_t, char>>,
      void*
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<3, CompactTuple<size_t&, char, void*, size_t, char>>,
      size_t
   >);
   static_assert(::std::is_same_v<
      TupleTypeAt<4, CompactTuple<size_t&, char, void*, size_t, char>>,
      char
   >);

   static_assert(sizeof(OptimizedTuple<Tuple<size_t&, char, void*, size_t, char>>) == sizeof(size_t) * 4);
   static_assert(sizeof(        CompactTuple<size_t&, char, void*, size_t, char> ) == sizeof(size_t) * 4);
   static_assert(sizeof(               Tuple<size_t&, char, void*, size_t, char> ) == sizeof(size_t) * 5);
}