///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Abstract.hpp"
#include "DefineTag.hpp"


namespace Langulus
{
   /*namespace Annies
   {
      struct Many;
   }

   namespace RTTI
   {
      struct DMeta;
   }*/

   ///                                                                        
   /// Descriptor intermediate type, used in constructors and assignment      
   /// operators to enable describe-construction/assignment. The inner type   
   /// is always a reference to a type-erased container.                      
   /// You should #include <Langulus/Annies/Many.hpp>                         
   ///        and #include <Langulus/CT/Describable.hpp>                      
   ///        in order to use Describe semantics                              
   struct Describe; /*{
      using Many = Annies::Many;
      const Many& what;

      using CTTI_ReflectAs     = void;
      using CTTI_Abstract      = Yup;
      using CTTI_Allocatable   = No;
      using CTTI_Intent        = Yup;

      Describe() = delete;
      constexpr Describe(const Describe&) noexcept = default;
      explicit constexpr Describe(Describe&&) noexcept = default;

      explicit constexpr Describe(const Many& descriptor) noexcept
         : what {descriptor} {}

      auto& operator *  () const noexcept { return  what; }
      auto* operator -> () const noexcept { return &what; }

      ///                                                                     
      /// These are higher order services, and are implemented in Annies      
      ///                                                                     
      template<CT::DefineTag>
      void Set(auto&&, bool force = false);

      template<CT::DefineTag...>
      bool ExtractTag(auto&...) const;
      auto ExtractData(auto&) const -> size_t;
      auto ExtractDataAs(auto&) const -> size_t;

      template<CT::NotVoid>
      auto FindType() const -> RTTI::DMeta;

      template<class TYPE>
      auto FindType(RTTI::DMeta) const -> RTTI::DMeta;
   };*/
}

namespace Langulus::CT
{
   /// Check if all T are describe-constructible.                             
   /// It has to have the T (Describe&&) constructor in order to be so.       
   template<class...T>
   concept DescribeConstructible = not Abstract<T...>
       and not Enum<T...> and not Aggregate<T...>
       and requires (Describe a) { (T {a}, ...); };
   
   /// Check if all T are describe-assignable.                                
   /// It has to have the T::operator = (Describe&&) constructor.             
   template<class...T>
   concept DescribeAssignable = not Abstract<T...>
       and not Enum<T...> and not Aggregate<T...>
       and requires (T&...lhs, Describe rhs) { ((lhs = rhs), ...); };
}
