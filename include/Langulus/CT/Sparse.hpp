///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Complete.hpp"
#include "Sheddable.hpp"


namespace Langulus::CTTI
{
   /// Turns T into a custom pointer. Examples:                               
   /// 1) template<> struct Sparse<MyData> {};                                
   /// 2) struct MyData { using CTTI_Sparse = Yup; };                         
   template<class T>
   struct Sparse;

   /// All conventional pointers are also considered CT::Sparse               
   template<class T> requires ::std::is_pointer_v<T>
   struct Sparse<T> {};
}

namespace Langulus::CT
{
   namespace Inner
   {
      /// Removes a pointer from the type. Supports custom pointers.          
      ///   @attention if an incomplete type is reached the nesting ceases,   
      ///      as incomplete types are always considered dense                
      template<class T, unsigned TIMES = 1>
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
            /*else if constexpr (::std::is_bounded_array_v<T>) { //TODO hmmmm, should arrays be considered sparse? there's no real indirection?
               // Conventional bounded array dereferencing              
               using deptr_once = ::std::remove_extent_t<T>;
               if constexpr (TIMES == 1)
                  return ::std::type_identity<deptr_once> {};
               else
                  return NestedDeptr<deptr_once, TIMES - 1>();
            }*/
            else if constexpr (Complete<CTTI::Sparse<T>>) {
               // Custom pointer (externally defined) dereferencing     
               static_assert(requires(T t) { *t; },
                  "Custom pointer doesn't have unary operator*");
               
               using deptr_once = ::std::remove_reference_t<decltype(*LglsFake(T))>;
               if constexpr (TIMES == 1)
                  return ::std::type_identity<deptr_once> {};
               else
                  return NestedDeptr<deptr_once, TIMES - 1>();
            }
            else if constexpr (requires { typename T::CTTI_Sparse; }) {
               if constexpr (T::CTTI_Sparse::Enabled) {
                  // Custom pointer (internally defined) dereferencing  
                  static_assert(requires(T t) { *t; },
                     "Custom pointer doesn't have unary operator*");
                  
                  using deptr_once = ::std::remove_reference_t<decltype(*LglsFake(T))>;
                  if constexpr (TIMES == 1)
                     return ::std::type_identity<deptr_once> {};
                  else
                     return NestedDeptr<deptr_once, TIMES - 1>();
               }
               else return ::std::type_identity<T> {};
            }
            else return ::std::type_identity<T> {};
         }
      }
   }

   /// Check if all T are sparse. Supports custom pointer types.              
   ///   @attention incomplete types are never considered sparse, because     
   ///      it is assumed, that pointers are simple types that are always     
   ///      complete. Make sure you define your custom pointers!              
   template<class...T>
   concept Sparse = PartialValidate<T...> and ((not
      ::std::is_same_v<::std::type_identity<ShedDeref<T>>, 
                       decltype(CT::Inner::NestedDeptr<ShedDeref<T>>())>) and ...);

   /// Check if all T are dense. Supports custom pointer types.               
   ///   @attention incomplete types are always considered dense              
   template<class...T>
   concept Dense = PartialValidate<T...> and ((not Sparse<T>) and ...);

   /// Check if all T are custom pointer types.                               
   template<class...T>
   concept CustomPointer = PartialValidate<T...> and Sparse<T...>
       and ((not ::std::is_pointer_v<ShedDeref<T>>) and ...);
}

namespace Langulus
{
   /// Remove a number of pointers from type. Supports custom pointer types.  
   ///   @attention may result in a reference                                 
   ///   @attention if an incomplete type is reached the nesting ceases, as   
   ///      incomplete types are always considered dense                      
   template<class T, unsigned TIMES = 1>
   using Deptr = typename decltype(CT::Inner::NestedDeptr<ShedDeref<T>, TIMES>())::type;

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
   
   /// Count the number of indirections, including custom pointers.           
   ///   @attention ignores sheddable layers                                  
   template<class T>
   constexpr size_t IndirectsOf = Inner::CountIndirections<T>();

   /// Execute a lambda for each indirection inside a type T.                 
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

   /// Execute a lambda for each indirection inside a type T by dereferencing.
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