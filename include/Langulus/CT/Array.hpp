///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Typed.hpp"
#include <Langulus/Utils/Literal.hpp>


namespace Langulus::CTTI
{
   /// Extends T with an extent to turn it into a custom array. Examples:     
   /// 1) template<> struct Array<MyData> : Yes<5> {};                        
   /// 2) struct MyData { using CTTI_Array = Yes<5>; };                       
   /// Optional: in many use cases, you should also make MyData CT::Typed     
   ///   and make sure sizeof(MyData) == TypeOf<MyData> * ExtentOf<MyData>,   
   ///   if you want to reap the benefits of SIMD optimizations for T         
   template<class T>
   struct Array;

   /// All bounded arrays are considered CT::Array, even those with extent 1  
   template<class T> requires ::std::is_bounded_array_v<T>
   struct Array<T> : Yes<::std::extent_v<T>> {};

   /// All std::arrays are considered CT::Array                               
   template<class T, size_t S>
   struct Array<::std::array<T, S>> : Yes<S> {};
}

namespace Langulus::CT
{
   namespace Inner
   {
      /// Get the inner type of a custom array. Returns void if not array.    
      template<class T>
      consteval auto GetCustomArrayType() {
         static_assert(not ::std::is_reference_v<T>,
            "Shed all references prior to this call");
         static_assert(not Sheddable<T>,
            "Shed all sheddables prior to this call");

         using ctti = CTTI::Array<T>;
         if constexpr (Complete<ctti>) {
            if constexpr (ctti::Enabled)
               return ::std::type_identity<TypeOf<T>> {};
            else
               return ::std::type_identity<void> {};
         }
         else {
            static_assert(Complete<T>,
               "Can't access `CTTI_Array` inside incomplete type");

            if constexpr (requires { typename T::CTTI_Array; }) {
               using inner = typename T::CTTI_Array;
               if constexpr (inner::Enabled)
                  return ::std::type_identity<TypeOf<T>> {};
               else
                  return ::std::type_identity<void> {};
            }
            else return ::std::type_identity<void> {};
         }
      };

      /// Returns 1 if T is not marked as an array, otherwise returns the     
      /// extent.                                                             
      template<class T>
      consteval size_t GetCustomExtent() {
         static_assert(not ::std::is_reference_v<T>,
            "Shed all references prior to this call");
         static_assert(not Sheddable<T>,
            "Shed all sheddables prior to this call");

         using ctti = CTTI::Array<T>;
         if constexpr (Complete<ctti>) {
            if constexpr (ctti::Enabled) {
               static_assert(ctti::Constant >= 1,
                  "Wrongly specialized `CTTI::Array`");  
               return ctti::Constant;
            }
            else return 1;
         }
         else {
            static_assert(Complete<T>,
               "Can't access `CTTI_Array` inside incomplete type");

            if constexpr (requires { typename T::CTTI_Array; }) {
               using inner = typename T::CTTI_Array;
               if constexpr (inner::Enabled) {
                  static_assert(inner::Constant >= 1,
                     "Wrongly specified `T::CTTI_Array`");  
                  return inner::Constant;
               }
               else return 1;
            }
            else return 1;
         }
      };

      /// Multiplies all the nested array sizes together.                     
      /// Results in 1 if T is not an array.                                  
      template<class T>
      consteval size_t GetCustomExtentNested() {
         using InnerT = typename decltype(GetCustomArrayType<T>())::type;
         if constexpr (not ::std::is_void_v<InnerT>)
            return GetCustomExtent<T>() * GetCustomExtentNested<InnerT>();
         else
            return GetCustomExtent<T>();
      };

      /// Removes all custom and bounded extents from arrays.                 
      /// Removes references as well.                                         
      template<class T>
      consteval auto NestedDeext() {
         using InnerT = typename decltype(GetCustomArrayType<T>())::type;
         if constexpr (not ::std::is_void_v<InnerT>)
            return NestedDeext<InnerT>();
         else
            return ::std::type_identity<T> {};
      }
   }

   /// Check if all T are bounded or custom arrays                            
   template<class...T>
   concept Array = PartialValidate<T...>
       and ((not ::std::is_void_v<typename decltype(Inner::GetCustomArrayType<ShedDeref<T>>())::type>) and ...);
}

namespace Langulus
{
   /// Remove the topmost array extent from a type                            
   ///   @attention will remove references as well                            
   template<class T>
   using Deext = typename decltype(CT::Inner::GetCustomArrayType<ShedDeref<T>>())::type;

   /// Get the extent of a bounded array type, or 1 if T is not an array.     
   /// If multiple types are provided, the sum of the extents is done.        
   template<class...T>
   constexpr size_t ExtentOf = (CT::Inner::GetCustomExtent<ShedDeref<T>>() + ...);

   /// Get the extent of an array argument, or 1 if T is not an array.        
   /// If multiple arguments are provided, the sum of the extents is done.    
   template<class...T>
   constexpr size_t GetExtentOf(T&&...) { return ExtentOf<ShedDeref<T>...>; }

   /// Get all nested extents of a bounded array type, multiplied, or 1       
   /// if T is not an array. If multiple types are provided, the sum of the   
   /// individual AllExtentsOf is done.                                       
   template<class...T>
   constexpr size_t AllExtentsOf = (CT::Inner::GetCustomExtentNested<ShedDeref<T>>() + ...);

   /// Get all nested extents of a bounded array argument, multiplied, or 1   
   /// if T is not an array. If multiple arguments are provided, the sum of   
   /// the individual AllExtentsOf is done.                                   
   template<class...T>
   constexpr size_t GetAllExtentsOf(T&&...) { return AllExtentsOf<ShedDeref<T>...>; }

   /// Removes all bounded array extents from a type.                         
   /// Removes references if type had extent.                                 
   /// For example: `void**(&)[6][6][6]` becomes `void**`.                    
   ///              `void*&` remains `void*&`.                                
   template<class T>
   using DeextAll = typename decltype(CT::Inner::NestedDeext<ShedDeref<T>>())::type;
}