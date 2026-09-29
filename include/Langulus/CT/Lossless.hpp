///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Typenav.hpp"
#include "../IntentOf.hpp"
#include "Fundamental.hpp"
#include "Scalar.hpp"
#include "Typed.hpp"
#include "Signed.hpp"


namespace Langulus::CT
{
   template<class T>
   concept SignedInteger8  = Signed<T> and Integer<T> and sizeof(T) == 1;
   template<class T>
   concept SignedInteger16 = Signed<T> and Integer<T> and sizeof(T) == 2;
   template<class T>
   concept SignedInteger32 = Signed<T> and Integer<T> and sizeof(T) == 4;
   template<class T>
   concept SignedInteger64 = Signed<T> and Integer<T> and sizeof(T) == 8;

   template<class T>
   concept UnsignedInteger8  = Unsigned<T> and Integer<T> and sizeof(T) == 1;
   template<class T>
   concept UnsignedInteger16 = Unsigned<T> and Integer<T> and sizeof(T) == 2;
   template<class T>
   concept UnsignedInteger32 = Unsigned<T> and Integer<T> and sizeof(T) == 4;
   template<class T>
   concept UnsignedInteger64 = Unsigned<T> and Integer<T> and sizeof(T) == 8;

   template<class T>
   concept Integer8  = Integer<T> and sizeof(T) == 1;
   template<class T>
   concept Integer16 = Integer<T> and sizeof(T) == 2;
   template<class T>
   concept Integer32 = Integer<T> and sizeof(T) == 4;
   template<class T>
   concept Integer64 = Integer<T> and sizeof(T) == 8;

   template<class T>
   concept Real16 = Real<T> and sizeof(T) == 2;
   template<class T>
   concept Real32 = Real<T> and sizeof(T) == 4;
   template<class T>
   concept Real64 = Real<T> and sizeof(T) == 8;
}

namespace Langulus
{
   /// Casts a scalar to its underlying fundamental type.                     
   /// Acts like a nested TypedCast().                                        
   ///   @param a the scalar to cast                                          
   ///   @return a reference to the underlying type                           
   template<CT::Scalar T> LANGULUS(ALWAYS_INLINED)
   constexpr decltype(auto) FundamentalCast(T&& a) noexcept {
      if constexpr (CT::Typed<T> and requires { TypedCast(DeintCast(a)); }) {
         // Explicitly cast to a reference of the contained type, and   
         // nest down to the fundamentals                               
         return FundamentalCast(TypedCast(DeintCast(a)));
      }
      else return DeintCast(a);
   }

   namespace Inner
   {
      /// Returns the extent overlap of two arrays/non arrays                 
      ///   @return the smaller extent, if two arrays are provided;           
      ///           the bigger extent, if one of the arguments isn't an array;
      ///           1 if both arguments are not arrays;                       
      template<class LHS, class RHS>
      consteval size_t OverlapCounts() noexcept {
         static_assert(CT::NotSheddable<LHS, RHS>, "Shed all sheddables first");
         static_assert(CT::NotConvoluted<LHS, RHS>, "Shed all qualifiers first");
         constexpr auto lhs = AllExtentsOf<LHS>;
         constexpr auto rhs = AllExtentsOf<RHS>;

         if constexpr (lhs > 1 and rhs > 1)
            return lhs < rhs ? lhs : rhs;
         else if constexpr (lhs > 1)
            return lhs;
         else if constexpr (rhs > 1)
            return rhs;
         else
            return 1;
      }

      /*#define OVERLAP_EXTENTS(l,r) OverlapExtents<decltype(l), decltype(r)>()
      
      /// Returns the count overlap of two vectors/scalars                       
      /// This is used to decide the output array size, for containing the       
      /// result of an arithmetic operation                                      
      ///   @tparam LHS - left type                                              
      ///   @tparam RHS - right type                                             
      ///   @return the overlapping count:                                       
      ///           the smaller extent, if two arrays are provided;              
      ///           the bigger extent, if one of the arguments isn't a vector    
      ///           1 if both arguments are not arrays                           
      template<class LHS, class RHS>
      consteval Count OverlapCounts() noexcept {
         constexpr auto lhs = CountOf<Deint<LHS>>;
         constexpr auto rhs = CountOf<Deint<RHS>>;

         if constexpr (lhs > 1 and rhs > 1)
            return lhs < rhs ? lhs : rhs;
         else if constexpr (lhs > 1)
            return lhs;
         else if constexpr (rhs > 1)
            return rhs;
         else
            return 1;
      }*/
      
      /// When given two types, choose the one that is most lossless in terms 
      /// of behavior, and capacity                                           
      ///  - if T1 or T2 is an array, an array of OverlapCount size will be   
      ///    given back. The array will be of the lossless decayed type       
      ///  - if both types are reals, the bigger real will be returned        
      ///  - if one of the type is a real, and the other an integer, the real 
      ///    will be returned                                                 
      ///  - if both types are integers with different signs, always returns  
      ///    the signed equivalent of the bigger integer                      
      ///  - if one of the types is not CT::Fundamental type, it will always  
      ///    be preferred, as it may have custom behavior                     
      ///  - if both types are not CT::Fundamental, the first type is always  
      ///    preferred as a deterministic fallback                            
      template<class T1, class T2>
      consteval auto PickLossless() {
         static_assert(CT::NotSheddable<T1, T2>, "Shed all sheddables first");
         static_assert(CT::NotConvoluted<T1, T2>, "Shed all qualifiers first");
         constexpr auto size = OverlapCounts<T1, T2>();

         if constexpr (CT::Fundamental<DeextAll<T1>, DeextAll<T2>>) {
            // Both types are fundamental                               
            using LHS = Decay<T1>;
            using RHS = Decay<T2>;
   
            if constexpr (CT::Real<LHS, RHS>) {
               // Always prefer the bigger real number                  
               if constexpr (sizeof(LHS) >= sizeof(RHS))
                  return ::std::array<LHS, size> {};
               else
                  return ::std::array<RHS, size> {};
            }
            else if constexpr (CT::Real<LHS> and not CT::Real<RHS>) {
               // Always prefer real numbers                            
               return ::std::array<LHS, size> {};
            }
            else if constexpr (CT::Real<RHS> and not CT::Real<LHS>) {
               // Always prefer real numbers                            
               return ::std::array<RHS, size> {};
            }
            else if constexpr (CT::Signed<LHS> == CT::Signed<RHS>) {
               // Both are signed integers, so pick the bigger one      
               if constexpr (sizeof(LHS) >= sizeof(RHS))
                  return ::std::array<LHS, size> {};
               else
                  return ::std::array<RHS, size> {};
            }
            else if constexpr (CT::Signed<LHS>) {
               // LHS is signed, but RHS is not, so pick the signed one,
               // but also guarantee that size remains the bigger one   
               if constexpr (sizeof(LHS) >= sizeof(RHS))
                  return ::std::array<LHS, size> {};
               else
                  return ::std::array<::std::make_signed_t<RHS>, size> {};
            }
            else {
               // RHS is signed, but LHS is not, so pick the signed one,
               // but also guarantee that size remains the bigger one   
               if constexpr (sizeof(RHS) >= sizeof(LHS))
                  return ::std::array<RHS, size> {};
               else
                  return ::std::array<::std::make_signed_t<LHS>, size> {};
            }
         }
         else if constexpr (CT::Fundamental<DeextAll<T1>>) {
            // RHS isn't fundamental, so always prefer it               
            if constexpr (CT::Typed<T2>)
               return ::std::array<Decay<TypeOf<T2>>, size> {};
            else
               return ::std::array<Decay<T2>, size> {};
         }
         else {
            // Either both types aren't fundamental, or the RHS one is. 
            // Just fallback to LHS.                                    
            if constexpr (CT::Typed<T1>)
               return ::std::array<Decay<TypeOf<T1>>, size> {};
            else
               return ::std::array<Decay<T1>, size> {};
         }
      }

      /// Nest the above function for all types in a variadic template        
      template<class T1, class T2, class...TN>
      consteval auto LosslessNestedInner() {
         static_assert(CT::NotSheddable<T1, T2, TN...>, "Shed all sheddables first");
         static_assert(CT::NotConvoluted<T1, T2, TN...>, "Shed all qualifiers first");
         using T1T2 = decltype(PickLossless<T1, T2>());

         if constexpr (sizeof...(TN))
            return LosslessNestedInner<T1T2, TN...>();
         else
            return T1T2 {};
      }

      /// Nest the above function for all types in a variadic template        
      ///   @attention this strips all sheddables and qualifiers              
      template<class T1, class...TN>
      consteval auto LosslessNested() {
         if constexpr (sizeof...(TN) == 0) {
            using T = DecvqAll<ShedDeref<T1>>;
            if constexpr (CT::Typed<T>)
               return ::std::array<Decay<TypeOf<T>>, AllExtentsOf<T>> {};
            else
               return ::std::array<Decay<T>, AllExtentsOf<T>> {};
         }
         else return LosslessNestedInner<DecvqAll<ShedDeref<T1>>, DecvqAll<ShedDeref<TN>>...>();
      }
   }

   /// Given any number of types, choose the one that is most lossless        
   /// after an arithmetic operation is performed between them. If any type   
   /// is an array, an array of OverlapCount size will be given back.         
   ///   @attention this will discard any sheddables and qualifiers           
   ///   @attention this never does integer promotions - only the involved    
   ///      types are considered!                                             
   template<class T1, class...TN>
   using Lossless = ::std::conditional_t<
         AllExtentsOf<decltype(::Langulus::Inner::LosslessNested<T1, TN...>())> == 1,
               TypeOf<decltype(::Langulus::Inner::LosslessNested<T1, TN...>())>,
               TypeOf<decltype(::Langulus::Inner::LosslessNested<T1, TN...>())>
                   [AllExtentsOf<decltype(::Langulus::Inner::LosslessNested<T1, TN...>())>]
      >;

   namespace Inner
   {
      template<class T, bool FORCE_SIGNED = false>
      consteval auto WiderInner() {
         if constexpr (CT::SignedInteger8<T>)
            return ::std::type_identity<int16_t> {};
         else if constexpr (CT::UnsignedInteger8<T>) {
            if constexpr (FORCE_SIGNED)
               return ::std::type_identity<int16_t> {};
            else
               return ::std::type_identity<uint16_t> {};
         }
         else if constexpr (CT::SignedInteger16<T>)
            return ::std::type_identity<int32_t> {};
         else if constexpr (CT::UnsignedInteger16<T>) {
            if constexpr (FORCE_SIGNED)
               return ::std::type_identity<int32_t> {};
            else
               return ::std::type_identity<uint32_t> {};
         }
         else if constexpr (CT::SignedInteger32<T>)
            return ::std::type_identity<int64_t> {};
         else if constexpr (CT::UnsignedInteger32<T>) {
            if constexpr (FORCE_SIGNED)
               return ::std::type_identity<int64_t> {};
            else
               return ::std::type_identity<uint64_t> {};
         }
         else if constexpr (CT::Integer64<T>)
            return ::std::type_identity<T> {};
         else if constexpr (CT::Real32<T>)
            return ::std::type_identity<double> {};
         else if constexpr (CT::Real64<T>)
            return ::std::type_identity<double> {};
         else
           static_assert(false, "Can't find a wider type");
      }

      template<class T>
      consteval auto NarrowerInner() {
         if constexpr (CT::Integer8<T>)
            return ::std::type_identity<T> {};
         else if constexpr (CT::SignedInteger16<T>)
            return ::std::type_identity<int8_t> {};
         else if constexpr (CT::UnsignedInteger16<T>)
            return ::std::type_identity<uint8_t> {};
         else if constexpr (CT::SignedInteger32<T>)
            return ::std::type_identity<int16_t> {};
         else if constexpr (CT::UnsignedInteger32<T>)
            return ::std::type_identity<uint16_t> {};
         else if constexpr (CT::SignedInteger64<T>)
            return ::std::type_identity<int32_t> {};
         else if constexpr (CT::UnsignedInteger64<T>)
            return ::std::type_identity<uint32_t> {};
         else if constexpr (CT::Real32<T>)
            return ::std::type_identity<T> {};
         else if constexpr (CT::Real64<T>)
            return ::std::type_identity<float> {};
         else
           static_assert(false, "Can't find a narrower type");
      }
   }

   /// Get a wider fundamental type, if possible                              
   /// uint32_t -> uint64_t                                                   
   template<class T1, class...TN>
   using Wider = typename decltype(
         ::Langulus::Inner::WiderInner<Lossless<T1, TN...>>()
      )::type;

   /// Get a signed wider fundamental type, if possible                       
   /// uint32_t -> int64_t                                                    
   template<class T1, class...TN>
   using WiderSigned = typename decltype(
         ::Langulus::Inner::WiderInner<Lossless<T1, TN...>, true>()
      )::type;

   /// Get a smaller fundamental type, if possible                            
   /// uint32_t -> uint16_t                                                   
   template<class T1, class...TN>
   using Narrower = typename decltype(
         ::Langulus::Inner::NarrowerInner<Lossless<T1, TN...>>()
      )::type;
}