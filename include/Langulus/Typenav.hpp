///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "CT/Sheddable.hpp"
#include "CT/Sparse.hpp"
#include "CT/Macros.hpp"


///                                                                           
///   A namespace for defining compile-time type information tags.            
///                                                                           
///   Specializing <type_traits> is generally undefined behavior, but here    
/// we have alternatives that are more flexible, using type_traits as the     
/// ground truth and building concepts on top of them in Langulus::CT.        
/// Read more: https://stackoverflow.com/questions/25345486                   
///   Each of the structures in this namespace corresponds to a concept in    
/// Langulus::CT. These concepts can be affected in two ways (unless          
/// specified otherwise):                                                     
///   1. Specialize the appropriate CTTI::<name> struct for a type/concept.   
///      Some CTTI_<Name> tags might require specific way of specialization,  
///      check the CT/<Name>.hpp file for documentation.                      
///   2. Add a public `using CTTI_<Name> = Yup/Yes<CONST>/Maybe/No;` in the   
///      desired type.                                                        
namespace Langulus::CTTI
{
   /// MARK: CTTI                                                             
   /// Affects CT::Null<T>:                                                   
   template<class T>
   struct Null {
      static constexpr bool Default = true;
      static constexpr bool Enabled = ::std::is_null_pointer_v<T>;
   };
   
   /// Affects CT::Enum<T>:                                                   
   template<class T>
   struct Enum {
      static constexpr bool Default = true;
      static constexpr bool Enabled = ::std::is_enum_v<T>;
   };
   
   /// Affects CT::Aggregate<T>:                                              
   template<class T>
   struct Aggregate {
      static constexpr bool Default = true;
      static constexpr bool Enabled = ::std::is_aggregate_v<T>;
   };
}

namespace Langulus
{
   /// Remove a reference from type                                           
   template<class T>
   using Deref = ::std::remove_reference_t<T>;

   /// Remove a const/volatile from a type                                    
   template<class T>
   using Decvq = ::std::remove_cv_t<T>;

   /// Remove a const from a type                                             
   template<class T>
   using Decq = ::std::remove_const_t<T>;

   /// Remove a volatile from a type                                          
   template<class T>
   using Devq = ::std::remove_volatile_t<T>;

   namespace Inner
   {
      /// Nest-strip any qualifiers, extents, references, sheddables, and     
      /// indirections (including custom pointers).                           
      ///   @return a pointer to the stripped T                               
      ///   @attention if an incomplete type is reached, the nesting ceases,  
      ///      and the decayed incomplete type is returned.                   
      template<class T>
      consteval auto NestedDecay() {
         using Stripped = Decvq<Deref<Deptr<T>>>;
         if constexpr (::std::is_same_v<T, Stripped>)
            return static_cast<Stripped*>(nullptr);
         else
            return NestedDecay<Stripped>();
      }
   }

   /// Strip a typename to its origin, removing qualifiers, indirections      
   /// (even custom ones), references, and sheddables. Unlike std::decay_t,   
   /// this strongly guarantees, that it strips _everything_, including       
   /// nested pointers, sheddables, and extents.                              
   template<class T>
   using Decay = ::std::remove_pointer_t<decltype(Inner::NestedDecay<T>())>;
   

   /// MARK: CT                                                               
   namespace CT
   {
      /// Check if all T are volatile-qualified                               
      template<class...T>
      concept Volatile = PartialValidate<T...>
          and (::std::is_volatile_v<ShedDeref<T>> and ...);

      /// Check if all T are constant-qualified                               
      template<class...T>
      concept Constant = PartialValidate<T...>
          and (::std::is_const_v<ShedDeref<T>> and ...);

      /// Check if all T are not constant-qualified                           
      template<class...T>
      concept Mutable = PartialValidate<T...>
         and ((not Constant<T>) and ...);

      /// Check if all T are either const- and/or volatile-qualified          
      template<class...T>
      concept Convoluted = PartialValidate<T...>
          and ((::std::is_const_v<ShedDeref<T>>
             or ::std::is_volatile_v<ShedDeref<T>>
          ) and ...);

      /// Check if none of T are const- and/or volatile-qualified             
      template<class...T>
      concept NotConvoluted = PartialValidate<T...>
          and ((not Convoluted<T>) and ...);

      /// Check if all T are reference types after shedding, which requires   
      /// all T to be complete. You can always just use std::is_reference_v.  
      template<class...T>
      concept Reference = PartialValidate<T...>
          and (::std::is_reference_v<Shed<T>> and ...);

      /// Check if all T aren't reference types after shedding, which requires
      /// all T to be complete. You can always just use std::is_reference_v.  
      template<class...T>
      concept NotReference = PartialValidate<T...>
          and ((not ::std::is_reference_v<Shed<T>>) and ...);

      /// Check if all types have no reference/pointer/extent/qualifiers.     
      /// Includes support for custom pointers.                               
      ///   @attention this doesn't shed or remove references before check    
      template<class...T>
      concept Decayed = PartialValidate<T...> and [] {
         if constexpr (((
            ::std::is_bounded_array_v<T>
         or ::std::is_reference_v<T>
         or ::std::is_const_v<T>
         or ::std::is_volatile_v<T>) or ...))
            return false;
         else
            return CT::Dense<T...>;
      } ();
   
      /// Check if types have reference/pointer/extent/const/volatile         
      ///   @attention this doesn't shed or remove references before check    
      template<class...T>
      concept NotDecayed = PartialValidate<T...> and ((not Decayed<T>) and ...);

      /// True if T is not a pointer (even a custom one), has no extent       
      /// with [] and isn't a reference.                                      
      ///   @attention still allowed to be cv-qualified                       
      template<class...T>
      concept Slab = PartialValidate<T...> and [] {//TODO is this ever used?
         if constexpr (((::std::is_reference_v<T> or ::std::is_array_v<T>) or ...))
            return false;
         else
            return CT::Dense<T...>;
      } ();
         
      namespace Inner
      {
         /// Checks for const/volatile qualifiers in all indirections/refs.   
         template<class T>
         consteval bool NestedCheckCVQ() {
            if constexpr (CT::Convoluted<T>)
               return true;
            else if constexpr (::std::is_reference_v<T>)
               return NestedCheckCVQ<Deref<T>>();
            else if constexpr (CT::Sparse<T>)
               return NestedCheckCVQ<Deptr<T>>();
            else if constexpr (::std::is_bounded_array_v<T>)
               return NestedCheckCVQ<::std::remove_extent_t<T>>();
            else
               return false;
         }

         /// Checks if all indirections/refs are constant.                    
         template<class T>
         consteval bool NestedConstantEverywhere() {
            if constexpr (::std::is_reference_v<T>)
               return CT::Constant<Deref<T>> and NestedConstantEverywhere<Deref<T>>();
            else if constexpr (CT::Sparse<T>)
               return CT::Constant<Deptr<T>> and NestedConstantEverywhere<Deptr<T>>();
            else if constexpr (::std::is_bounded_array_v<T>)
               return CT::Constant<::std::remove_extent_t<T>> and NestedConstantEverywhere<::std::remove_extent_t<T>>();
            else
               return CT::Constant<T>;
         }
      }
      
      /// Check if all T are either const- and/or volatile-qualified on any   
      /// level of indirection.                                               
      template<class...T>
      concept ConvolutedAnywhere = PartialValidate<T...>
          and (Inner::NestedCheckCVQ<T>() and ...);

      /// Check if none of T are const- and/or volatile-qualified on any      
      /// level of indirection.                                               
      template<class...T>
      concept NotConvolutedAnywhere = PartialValidate<T...>
          and ((not ConvolutedAnywhere<T>) and ...);

      /// Check if all T are either const- and/or volatile-qualified on any   
      /// level of indirection.                                               
      template<class...T>
      concept ConstantEverywhere = PartialValidate<T...>
          and (Inner::NestedConstantEverywhere<T>() and ...);

      /// Check if none of T are const- and/or volatile-qualified on any      
      /// level of indirection.                                               
      template<class...T>
      concept NotConstantEverywhere = PartialValidate<T...>
          and ((not ConstantEverywhere<T>) and ...);
   }

   namespace Inner
   {
      /// Removes all const/volatile qualifiers from all indirections.        
      /// Supports custom pointers. Preserves references.                     
      template<class T>
      consteval auto NestedDecvq() {
         if constexpr (::std::is_rvalue_reference_v<T>)
            return ::std::type_identity<typename decltype(NestedDecvq<Deref<T>>())::type&&> {};
         else if constexpr (::std::is_lvalue_reference_v<T>)
            return ::std::type_identity<typename decltype(NestedDecvq<Deref<T>>())::type&> {};
         else if constexpr (::std::is_pointer_v<T>)
            return ::std::type_identity<typename decltype(NestedDecvq<::std::remove_pointer_t<T>>())::type*> {};
         else if constexpr (::std::is_bounded_array_v<T>)
            return ::std::type_identity<typename decltype(NestedDecvq<::std::remove_extent_t<T>>())::type [::std::extent_v<T>]> {};
         else if constexpr (CT::Complete<T>) {
            if constexpr (CT::CustomPointer<T>)
               return ::std::type_identity<typename T::MakeDecvqAll> {};
            else
               return ::std::type_identity<::std::remove_cv_t<T>> {};
         }
         else return ::std::type_identity<::std::remove_cv_t<T>> {};
      }

      /// Adds const qualifier to all levels of indirection except the top.   
      /// Supports custom pointers. Preserves references.                     
      template<class T>
      consteval auto NestedConst() {
         if constexpr (::std::is_rvalue_reference_v<T>)
            return ::std::type_identity<typename decltype(NestedConst<Deref<T>>())::type const&&> {};
         else if constexpr (::std::is_lvalue_reference_v<T>)
            return ::std::type_identity<typename decltype(NestedConst<Deref<T>>())::type const&> {};
         else if constexpr (::std::is_pointer_v<T>)
            return ::std::type_identity<typename decltype(NestedConst<::std::remove_pointer_t<T>>())::type const*> {};
         else if constexpr (::std::is_bounded_array_v<T>)
            return ::std::type_identity<typename decltype(NestedConst<::std::remove_extent_t<T>>())::type const [::std::extent_v<T>]> {};
         else if constexpr (CT::Complete<T>) {
            if constexpr (CT::CustomPointer<T>)
               return ::std::type_identity<typename T::MakeConstAll> {};
            else
               return ::std::type_identity<T> {};
         }
         else return ::std::type_identity<T> {};
      }
   }

   /// Strip all qualifiers on all levels of indirection of a type.           
   /// Preserves references, makes them mutable.                              
   /// For example: `void const volatile* const* const` becomes `void**`.     
   ///              `void const volatile* const&` becomes `void*&`.           
   template<class T>
   using DecvqAll = typename decltype(Inner::NestedDecvq<T>())::type;

   /// Adds const qualifiers to all levels of indirection of a type, except   
   /// the top one. You can always do `const ConstAll<T>` to fix that.        
   /// Preserves references, makes them constant.                             
   /// For example: `void**` becomes `void const* const*`.                    
   ///              `void*&` becomes `void const* const&`.                    
   template<class T>
   using ConstAll = typename decltype(Inner::NestedConst<T>())::type;

   /// Strips all cv-qualifiers from the provided argument                    
   ///   @attention this will return pointers for bounded array arguments     
   template<class T> requires (not ::std::is_bounded_array_v<T>)
   LANGULUS(ALWAYS_INLINED)
   constexpr auto DecvqAllCast(T&& what) noexcept -> DecvqAll<T> {
      if constexpr (CT::Reference<T> or CT::Sparse<T>)
         return const_cast<DecvqAll<T>>(what);
      else
         return LglsFwd(what);
   }
   
   template<class T> requires ::std::is_bounded_array_v<T>
   LANGULUS(ALWAYS_INLINED)
   constexpr auto DecvqAllCast(T&& what) noexcept -> DecvqAll<::std::remove_extent_t<T>>* {
      return const_cast<DecvqAll<::std::remove_extent_t<T>>*>(what);
   }
   
   /// Add const qualifiers to the provided argument                          
   ///   @attention this will return pointers for bounded array arguments     
   template<class T> requires (not ::std::is_bounded_array_v<T>)
   LANGULUS(ALWAYS_INLINED)
   constexpr auto ConstAllCast(T&& what) noexcept -> ConstAll<T> {
      return const_cast<ConstAll<T>>(what);
   }
   
   template<class T> requires ::std::is_bounded_array_v<T>
   LANGULUS(ALWAYS_INLINED)
   constexpr auto ConstAllCast(T&& what) noexcept -> ConstAll<::std::remove_extent_t<T>> const* {
      return const_cast<ConstAll<::std::remove_extent_t<T>> const*>(what);
   }

   template<class T, class YES, class NO>
   using Tmut = typename ::std::conditional_t<CT::Mutable<T>,
         ::std::type_identity<YES>,
         ::std::type_identity<NO>
      >::type;

   #define LglsMutIf(CONDITION_TYPE, ...) Tmut<CONDITION_TYPE, __VA_ARGS__, ConstAll<__VA_ARGS__>>
}

LANGULUS_CTTI_CONCEPT(Null);
LANGULUS_CTTI_CONCEPT(Enum);
LANGULUS_CTTI_CONCEPT(Aggregate);