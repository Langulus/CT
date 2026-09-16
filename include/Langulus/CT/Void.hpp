///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Complete.hpp"


namespace Langulus::CTTI
{
   /// Affects CT::Void<T>                                                    
   template<class T>
   struct Void;

   /// Make sure no one interferes with true void type                        
   template<>
   struct Void<void> {};
}

namespace Langulus::CT
{
   namespace Inner
   {
      template<class T>
      consteval bool IsVoidInner() {
         using DT = ::std::remove_cvref_t<T>;
         if constexpr (Complete<CTTI::Void<DT>>) {
            // External check                                           
            return true;
         }
         else if constexpr (::std::is_class_v<DT>) {
            // Internal check                                           
            static_assert(Complete<DT>,
               "Can't check if an incomplete type is void");
            if constexpr (requires { DT::CTTI_Void::Enabled; })
               return DT::CTTI_Void::Enabled;
            else
               return false;
         }
         else return false;
      }
   }

   /// Check if all T are marked void                                         
   template<class...T>
   concept Void = PartialValidate<T...>
       and (Inner::IsVoidInner<T>() and ...);

   template<class...T>
   concept NotVoid = PartialValidate<T...>
       and ((not Inner::IsVoidInner<T>()) and ...);
}