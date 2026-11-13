///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Akin.hpp"
#include "../Utils/Types.hpp"


namespace Langulus::CTTI
{
   /// Can be used in two ways to satisfy CT::DerivedFrom<T>:                 
   /// 1. Specialize for T/concept                                            
   /// 2. Add a public `using CTTI_Bases = <single type or Types<...>>;` in T 
   //TODO when compile-time reflection is well supported, most cases of CTTI_Bases can be automated. only imposed bases should remain
   template<class T>
   struct Bases;
}

namespace Langulus::CT::Inner
{
   /// Helper function to extract reflected base types                        
   template<class T>
   consteval auto GetBaseTypes() {
      static_assert(not ::std::is_reference_v<T>,
         "Strip references first");
      static_assert(not CT::Convoluted<T>,
         "Strip constness/volatility first");
      static_assert(not CT::Sheddable<T>,
         "Strip sheddables first");

      using ctti = CTTI::Bases<T>;
      if constexpr (Complete<ctti>) {
         // Checked externally, T doesn't have to be complete           
         static_assert(CT::Typelist<ctti>,
            "Pick a type to derive from, or void, instead of using Yes/Yup/No"
            "for CTTI::Bases. Just inherit from Types<...>."
         );
         return ctti {};
      }
      else if constexpr (requires { typename T::CTTI_Bases; }) {
         // Checked internally, T has to be a complete type             
         using LIST = typename T::CTTI_Bases;
         if constexpr (CT::Typelist<LIST>)
            return LIST {};
         else
            return Types<LIST> {};
      }
      else return NoTypes {};
   };

   /// Check if T has BASE. Considers both built-in bases and custom ones.    
   template<class T, class BASE>
   consteval bool DerivedFrom() {
      if constexpr (::std::is_same_v<T, BASE>) {
         // Neither T nor BASE have to be complete                      
         return true;
      }
      else if constexpr (Complete<CTTI::Bases<T>>) {
         // Neither T nor BASE have to be complete                      
         return GetBaseTypes<T>().template Contains<BASE>;
      }
      else {
         // T has to be complete                                        
         using DT = Decay<T>;
         using DB = Decay<BASE>;
         if constexpr (::std::derived_from<DT, DB>)
            return ::std::derived_from<DT, DB>;
         else
            return GetBaseTypes<DT>().template Contains<DB>;
      }
   }
}

namespace Langulus::CT
{
   /// Check if the origin T publicly inherits (or is) all the BASE(s).       
   /// Compensates for std::derived_from not returning true for the same      
   /// primitive types...                                                     
   /// Considers both built-in bases and custom ones.                         
   template<class T, class...BASE>
   concept DerivedFrom = PartialValidate<BASE...>
       and (Inner::DerivedFrom<T, BASE>() and ...);
   
   /// Check if T1 is somehow related to all of the provided types.           
   /// Considers both built-in bases and custom ones.                         
   template<class T1, class...TN>
   concept Related = PartialValidate<TN...>
       and ((DerivedFrom<T1, TN> or DerivedFrom<TN, T1>) and ...);

   /// Check if a type is virtually derived from all the provided BASE(s).    
   /// Considers both built-in bases and custom ones.                         
   template<class T, class...BASE>
   concept VirtuallyDerivedFrom = PartialValidate<BASE...>
       and ((::std::is_base_of_v<Decay<BASE>, Decay<T>>
         and not requires (Decay<BASE>* from) { static_cast<Decay<T>*>(from); }
       ) and ...);
   
   /// Binary compatibility check between T1 and the provided TN              
   /// To be binary compatible, types must be of the same size, and be        
   /// similar or related                                                     
   template<class T1, class...TN>
   concept BinaryCompatible = PartialValidate<TN...> and ((
         Same<T1, TN> or (Related<T1, TN> and sizeof(T1) == sizeof(TN))
      ) and ...);
}

namespace Langulus
{
   /// Get the reflected base types, void if none                             
   template<class T>
   using BasesOf = decltype(CT::Inner::GetBaseTypes<Decvq<Deref<T>>>());


   ///                                                                        
   /// Some abstract bases for built-in types                                 
   ///                                                                        

   /// Context dependent number                                               
   struct AbstractNumber {
      using CTTI_Abstract  = Yup;
   };

   /// Context dependent integer number                                       
   struct AbstractInteger : AbstractNumber {
      using CTTI_Bases     = AbstractNumber;
   };

   /// Context dependent real number                                          
   struct AbstractReal : AbstractNumber {
      using CTTI_Bases     = AbstractNumber;
   };

   /// Context dependent unsigned number                                      
   struct AbstractUnsigned : AbstractNumber {
      using CTTI_Bases     = AbstractNumber;
   };

   /// Context dependent signed number                                        
   struct AbstractSigned : AbstractNumber {
      using CTTI_Bases     = AbstractNumber;
   };
   
   /// Context dependent unsigned integer                                     
   struct AbstractUnsignedInteger : AbstractInteger {
      using CTTI_Bases     = Types<AbstractInteger, AbstractUnsigned>;
   };

   /// Context dependent signed integer                                       
   struct AbstractSignedInteger : AbstractInteger {
      using CTTI_Bases     = Types<AbstractInteger, AbstractSigned>;
   };
   
   /// Context dependent unsigned real (unused but here for completeness)     
   struct AbstractUnsignedReal : AbstractReal {
      using CTTI_Bases     = Types<AbstractReal, AbstractUnsigned>;
   };

   /// Context dependent signed real                                          
   struct AbstractSignedReal : AbstractReal {
      using CTTI_Bases     = Types<AbstractReal, AbstractSigned>;
   };
}

namespace Langulus::CTTI
{
   template<> struct Bases<uint8_t>  : Types <AbstractUnsignedInteger> {};
   template<> struct Bases<uint16_t> : Types <AbstractUnsignedInteger> {};
   template<> struct Bases<uint32_t> : Types <AbstractUnsignedInteger> {};
   template<> struct Bases<uint64_t> : Types <AbstractUnsignedInteger> {};

   template<> struct Bases<int8_t>   : Types <AbstractSignedInteger> {};
   template<> struct Bases<int16_t>  : Types <AbstractSignedInteger> {};
   template<> struct Bases<int32_t>  : Types <AbstractSignedInteger> {};
   template<> struct Bases<int64_t>  : Types <AbstractSignedInteger> {};

   template<> struct Bases<float>    : Types <AbstractSignedReal> {};
   template<> struct Bases<double>   : Types <AbstractSignedReal> {};
}