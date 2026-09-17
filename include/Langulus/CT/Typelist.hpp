#pragma once
#include "Complete.hpp"


namespace Langulus::CTTI
{
   /// Affects CT::Typelist<T>                                                
   template<class T>
   struct Typelist;
}

namespace Langulus::CT
{
   namespace Inner
   {
      template<class T>
      consteval bool IsTypelistInner() {
         using DT = ::std::remove_cvref_t<T>;
         if constexpr (Complete<CTTI::Typelist<DT>>) {
            // Internal check                                           
            return true;
         }
         else if constexpr (::std::is_class_v<DT>) {
            // External check                                           
            static_assert(Complete<DT>,
               "Can't check if an incomplete type is a type list");
               
            if constexpr (requires { DT::CTTI_Typelist::Enabled; })
               return DT::CTTI_Typelist::Enabled;
            else
               return false;
         }
         else return false;
      }
   }

   /// Check if all T are typelists                                           
   template<class...T>
   concept Typelist = PartialValidate<T...>
       and (Inner::IsTypelistInner<T>() and ...);

   template<class...T>
   concept NotTypelist = PartialValidate<T...>
       and ((not Inner::IsTypelistInner<T>()) and ...);
}