///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Core.hpp>
#include <Langulus/HashOf.hpp>


namespace Langulus
{
   ///                                                                        
   ///   Charge, carrying the four verb dimensions                            
   ///                                                                        
   class Charge {
      using CTTI_POD      = Yup;
      using CTTI_Nullable = No;
      using CTTI_Charged  = Yup;

      // Mass:                                                          
      // Imagine the following spectrum:                                
      //          run away < avoid < approach < touch < punch           
      // You can use mass to reduce them to a single function with      
      // different magnitudes (a.k.a. mass). Similarily, multiplication 
      // can be represented as an addition with magnitude:              
      //          5*5 -> 5 + 5 + 5 + 5 + 5 -> 0 add*5 5                 
      Real mMass = DefaultMass;

      // Frequency of the verb                                          
      Real mRate = DefaultRate;
      // Time of the verb                                               
      Real mTime = DefaultTime;
      // Priority of the verb                                           
      Real mPriority = DefaultPriority;

   public:
      static constexpr Real DefaultMass      = 1;
      static constexpr Real DefaultRate      = 0;
      static constexpr Real DefaultTime      = 0;
      static constexpr Real DefaultPriority  = 0;
      static constexpr Real MinPriority      = -10000;
      static constexpr Real MaxPriority      = +10000;

      constexpr Charge(
         Real mass = DefaultMass,
         Real rate = DefaultRate,
         Real time = DefaultTime,
         Real prio = DefaultPriority
      ) noexcept : mMass{mass}, mRate{rate}, mTime{time}, mPriority{prio} {}

      constexpr bool operator == (const Charge& rhs) const noexcept {
         return mMass == rhs.mMass
            and mRate == rhs.mRate
            and mTime == rhs.mTime
            and mPriority == rhs.mPriority;
      }

      constexpr auto operator *  (const Real&) const noexcept -> Charge;
      constexpr auto operator ^  (const Real&) const noexcept -> Charge;

      constexpr auto operator *= (const Real&) noexcept -> Charge;
      constexpr auto operator ^= (const Real&) noexcept -> Charge;

      /// Check if charge is default-constructed                              
      constexpr bool IsCharged() const noexcept {
         return *this != Charge {};
      }

      /// Mass doesn't affect a charged thing's position inside flow, but all 
      /// other dimensions do. Check if they are not default-constructed.     
      /// If a charged thing is flow-dependent, it needs to have contextual   
      /// considerations before nestling in its proper place inside the flow. 
      constexpr bool IsFlowDependent() const noexcept {
         return mRate != DefaultRate
             or mTime != DefaultTime
             or mPriority != DefaultPriority;
      }

      /// Hash the charge                                                     
      constexpr Hash GetHash() const noexcept {
         return HashOf(mMass, mRate, mTime, mPriority);
      }

      /// Reset the charge, as if it is default-constructed                   
      void ResetCharge() noexcept {
         mMass     = DefaultMass;
         mRate     = DefaultRate;
         mTime     = DefaultTime;
         mPriority = DefaultPriority;
      }
   };
}

namespace Langulus::Annies
{
   
   /// Reset the charge to the default                                        
   LANGULUS(INLINED)
   void Charge::Reset() noexcept 

   /// Scale the mass of a charge                                             
   ///   @param scalar - the scalar to multiply by                            
   ///   @return a new charge instance with changed mass                      
   LANGULUS(INLINED)
   constexpr Charge Charge::operator * (const Real& scalar) const noexcept {
      return {mMass * scalar, mRate, mTime, mPriority};
   }

   /// Scale the rate of a charge                                             
   ///   @param scalar - the scalar to multiply by                            
   ///   @return a new charge instance with changed rate                      
   LANGULUS(INLINED)
   constexpr Charge Charge::operator ^ (const Real& scalar) const noexcept {
      return {mMass, mRate * scalar, mTime, mPriority};
   }

   /// Scale the mass of a charge (destructive)                               
   ///   @param scalar - the scalar to multiply by                            
   ///   @return a reference to this charge                                   
   LANGULUS(INLINED)
   constexpr Charge& Charge::operator *= (const Real& scalar) noexcept {
      mMass *= scalar;
      return *this;
   }

   /// Scale the rate of a charge (destructive)                               
   ///   @param scalar - the scalar to multiply by                            
   ///   @return a reference to this charge                                   
   LANGULUS(INLINED)
   constexpr Charge& Charge::operator ^= (const Real& scalar) noexcept {
      mRate *= scalar;
      return *this;
   }

}
