///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../Utils/Literal.hpp"
#include "../Utils/ASCII.hpp"


namespace Langulus::CTTI
{
   /// Extends T with suffix meta data at compile time. Examples:             
   /// 1) template<> struct Suffix<Seconds> : Yes<"s"> {};                    
   /// 2) struct Seconds { using CTTI_Suffix = Yes<"s">; };                   
   template<class T>
   struct Suffix;
}

namespace Langulus::CTTI
{
   ///                                                                        
   /// Some built-in suffices                                                 
   ///                                                                        
   template<> struct Suffix<bool>     : Yes<"b"> {};
   template<> struct Suffix<int8_t>   : Yes<"i8"> {};
   template<> struct Suffix<int16_t>  : Yes<"i16"> {};
   template<> struct Suffix<int32_t>  : Yes<::std::is_same_v<int32_t, int> ? "i\0\0" : "i32"> {};
   template<> struct Suffix<int64_t>  : Yes<::std::is_same_v<int64_t, int> ? "i\0\0" : "i64"> {};
   template<> struct Suffix<uint8_t>  : Yes<"u8"> {};
   template<> struct Suffix<uint16_t> : Yes<"u16"> {};
   template<> struct Suffix<uint32_t> : Yes<::std::is_same_v<uint32_t, unsigned> ? "u\0\0" : "u32"> {};
   template<> struct Suffix<uint64_t> : Yes<::std::is_same_v<uint64_t, unsigned> ? "u\0\0" : "u64"> {};
   template<> struct Suffix<float>    : Yes<::std::is_same_v<float, Langulus::Real> ? "\0" : "f"> {};
   template<> struct Suffix<double>   : Yes<::std::is_same_v<double, Langulus::Real> ? "\0" : "d"> {};
}

namespace Langulus
{
   /// Get the suffix for a type at compile-time                              
   ///   @tparam T the type to get the info of                                
   ///   @return a compile-time string                                        
   //TODO check if tested, cuz i found some mistakes
   template<class T>
   consteval auto SuffixOf() {
      using DT = Decvq<Deref<T>>;
      constexpr auto suff = LANGULUS_CTTI_CHECK_EXTRACT(DT, Suffix, Literal {});
      static_assert(IsASCII(suff), "Suffix must be ASCII");
      static_assert(suff == "" or IsAlphabetical(suff[0]),
         "Suffix must begin with an alphabetical symbol");
      return suff;
   }
}