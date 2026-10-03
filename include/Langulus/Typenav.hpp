///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "CT/Sheddable.hpp"
#include "CT/Macros.hpp"


///                                                                           
///   A namespace for defining compile-time type information tags.            
///                                                                           
///   Specializing <type_traits> is generally undefined behavior, but here    
/// we have alternatives that are more flexible, using type_traits as the     
/// ground truth and building concepts on top of them in Langulus::CT.        
/// Read more: https://stackoverflow.com/questions/25345486                   
///   Each of the structures in this namespace correspond to a concept in     
/// Langulus::CT. These concepts can be affected in two ways (unless          
/// specified otherwise):                                                     
///   1. Specialize the appropriate CTTI::<name> struct for a type/concept    
///   2. Add a public `using CTTI_<Name> = Yes/No;` in the desired type       
///   3. Some CTTI_<Name> tags might require types or values instead -        
///      they should have additional documentation alongside them             
namespace Langulus::CTTI
{
   /// MARK: CTTI                                                             
   /// Affects CT::Sparse<T>:                                                 
   template<class T>
   struct Sparse {
      static constexpr bool Default = true;
      static constexpr bool Enabled = ::std::is_pointer_v<T>;
   };
   
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

   namespace CT::Inner
   {
      /// Removes a pointer from the type. Supports custom pointers.          
      ///   @attention if an incomplete type is reached the nesting ceases    
      template<class T, unsigned TIMES>
      consteval auto NestedDeptr() {
         static_assert(not ::std::is_reference_v<T>,
            "Shed all references prior to this call");
         static_assert(TIMES >= 1,
            "Can't deptr zero times");

         if constexpr (not Complete<T>)
            return ::std::type_identity<T> {};
         else {
            if constexpr (::std::is_pointer_v<T>) {
               if constexpr (::std::is_void_v<::std::remove_pointer_t<T>>)
                  return ::std::type_identity<void> {};
               else {
                  // Conventional pointer dereferencing                 
                  using deptr_once = ::std::remove_pointer_t<T>;
                  if constexpr (TIMES == 1)
                     return ::std::type_identity<deptr_once> {};
                  else
                     return NestedDeptr<deptr_once, TIMES - 1>();
               }
            }
            else if constexpr (::std::is_bounded_array_v<T>) {
               // Conventional bounded array dereferencing              
               using deptr_once = ::std::remove_extent_t<T>;
               if constexpr (TIMES == 1)
                  return ::std::type_identity<deptr_once> {};
               else
                  return NestedDeptr<deptr_once, TIMES - 1>();
            }
            else if constexpr (LANGULUS_CTTI_CHECK(T, Sparse)) {
               // Custom pointer dereferencing                          
               static_assert(requires(T t) { *t; },
                  "Custom pointer doesn't have unary operator*");
               
               using deptr_once = Deref<decltype(*LglsFake(T))>;
               if constexpr (TIMES == 1)
                  return ::std::type_identity<deptr_once> {};
               else
                  return NestedDeptr<deptr_once, TIMES - 1>();
            }
            else return ::std::type_identity<T> {};
         }
      }
   }

   /// Remove a number of pointers from type. Supports custom pointer types.  
   ///   @attention may result in a reference                                 
   ///   @attention if an incomplete type is reached the nesting ceases       
   template<class T, unsigned TIMES = 1>
   using Deptr = typename decltype(CT::Inner::NestedDeptr<ShedDeref<T>, TIMES>())::type;

   namespace Inner
   {
      /// Nest-strip any qualifiers, extents, references, sheddables, and     
      /// indirections (including custom pointers).                           
      ///   @return a pointer to the stripped T                               
      ///   @attention if an incomplete type is reached, the nesting ceases   
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
   /// this strongly guarantees, that it strips _everything_, including nested
   /// pointers, sheddables, and extents.                                     
   template<class T>
   using Decay = ::std::remove_pointer_t<decltype(Inner::NestedDecay<T>())>;
   

   /// MARK: CT                                                               
   namespace CT
   {
      /// Check if all T are volatile-qualified                               
      template<class...T>
      concept Volatile = PartialValidate<T...>
          and (::std::is_volatile_v<ShedDeref<T>> and ...);

      /// Check if all T are sparse. Supports custom pointer types.           
      template<class...T>
      concept Sparse = PartialValidate<T...>
          and (LANGULUS_CTTI_CHECK(Decvq<ShedDeref<T>>, Sparse) and ...);

      /// Check if all T are custom pointer types.                            
      template<class...T>
      concept CustomPointer = PartialValidate<T...> and Sparse<T...>
          and ((not ::std::is_pointer_v<ShedDeref<T>>) and ...);

      /// Check if all T are dense. Detects custom pointer types.             
      template<class...T>
      concept Dense = PartialValidate<T...> and ((not Sparse<T>) and ...);

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

      /// Check if all T are reference types                                  
      template<class...T>
      concept Reference = PartialValidate<T...>
          and (::std::is_reference_v<Shed<T>> and ...);

      /// Check if all T are not reference types                              
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
      concept Slab = PartialValidate<T...> and [] {
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
               return CT::Constant<Deext<T>> and NestedConstantEverywhere<Deext<T>>();
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

   ///                                                                        
   /// Structure for describing custom packed pointers.                       
   /// The default PointerSpecification with all members initialized to zero  
   /// corresponds to a pointer with sizeof(void*) and thus not packed.       
   struct PointerSpecification {
      unsigned PoolBits = 0;
      unsigned EntryBits = 0;
      unsigned OffsetBits = 0;

      constexpr unsigned GetTotalBits() const noexcept {
         const auto total = PoolBits + EntryBits + OffsetBits;
         return total ? total : sizeof(void*)*8;
      }
      
      constexpr unsigned GetTotalBytes() const noexcept {
         const auto total = PoolBits + EntryBits + OffsetBits;
         return total ? total/8u : sizeof(void*);
      }
      
      constexpr bool IsPacked() const noexcept {
         return (PoolBits + EntryBits + OffsetBits) != 0;
      }
   };
   
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

      /// Count the number of indirections, including custom pointers.        
      ///   @return the number of pointers in a type                          
      template<class T>
      consteval size_t CountIndirections() {
         if constexpr (not CT::Complete<T>)
            return 0;
         else if constexpr (CT::Sparse<T>)
            return 1 + CountIndirections<Deptr<T>>();
         else
            return 0;
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
   constexpr auto DecvqAllCast(T&& what) noexcept -> DecvqAll<Deext<T>>* {
      return const_cast<DecvqAll<Deext<T>>*>(what);
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
   constexpr auto ConstAllCast(T&& what) noexcept -> ConstAll<Deext<T>> const* {
      return const_cast<ConstAll<Deext<T>> const*>(what);
   }
   
   /// Count the number of indirections, including custom pointers.           
   ///   @attention ignores sheddable layers                                  
   template<class T>
   constexpr size_t IndirectsOf = Inner::CountIndirections<T>();

   template<class T, class YES, class NO>
   using Tmut = typename ::std::conditional_t<CT::Mutable<T>,
         ::std::type_identity<YES>,
         ::std::type_identity<NO>
      >::type;

   #define LglsMutIf(CONDITION_TYPE, ...) Tmut<CONDITION_TYPE, __VA_ARGS__, ConstAll<__VA_ARGS__>>

   /// Execute a lambda for each indirection inside a type T                  
   /// The provided lambda must be of the form: [whatever]<class C>{...},     
   /// so that if you provide T as void***, three lambdas will be generated   
   /// and executed, with C being void***, void** and void*.                  
   template<class T>
   void ForEachIndirection(auto&& lambda) {
      if constexpr (CT::Sparse<T>) {
         lambda();
         if constexpr (CT::Sparse<Deptr<T>>)
            ForEachIndirection<Deptr<T>>(LglsFwd(lambda));
      }
   }

   /// Execute a lambda for each indirection inside a type T by dereferencing 
   /// The provided lambda must be of the form: [whatever](auto ptr){...},    
   /// so that if you provide argument as void***, three lambdas will be      
   /// generated and executed, with 'ptr' being void***, void** and void*.    
   template<class T>
   void ForEachIndirection(T& pointer, auto&& lambda) {
      if constexpr (CT::Sparse<T>) {
         lambda(pointer);
         if constexpr (CT::Sparse<Deptr<T>>)
            ForEachIndirection((*pointer), LglsFwd(lambda));
      }
   }
}

LANGULUS_CTTI_CONCEPT(Null);
LANGULUS_CTTI_CONCEPT(Enum);
LANGULUS_CTTI_CONCEPT(Aggregate);