#pragma once
#include "Complete.hpp"


namespace Langulus::CTTI
{
   /// Affects CT::Valuelist<T>                                               
   template<class T>
   struct Valuelist;
}

namespace Langulus::CT
{
   namespace Inner
   {
      template<class T>
      consteval bool IsValuelistInner() {
         using DT = ::std::remove_cvref_t<T>;
         if constexpr (Complete<CTTI::Valuelist<DT>>) {
            // Internal check                                           
            return true;
         }
         else if constexpr (::std::is_class_v<DT>) {
            // External check                                           
            static_assert(Complete<DT>,
               "Can't check if an incomplete type is a value list");
               
            if constexpr (requires { DT::CTTI_Valuelist::Enabled; })
               return DT::CTTI_Valuelist::Enabled;
            else
               return false;
         }
         else return false;
      }
   }

   /// Check if all T are typelists                                           
   template<class...T>
   concept Valuelist = PartialValidate<T...>
       and (Inner::IsValuelistInner<T>() and ...);

   template<class...T>
   concept NotValuelist = PartialValidate<T...>
       and ((not Valuelist<T>) and ...);
}