///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Core.hpp>
#include <type_traits>


namespace Langulus
{
   ///                                                                        
   ///   Charge, carrying the four verb dimensions                            
   ///                                                                        
   struct Charge {
      using CTTI_POD      = Yup;
      using CTTI_Nullable = No;
      using CTTI_Charged  = Yup;

      static constexpr Real DefaultMass       = 1;
      static constexpr Real DefaultRate       = 0;
      static constexpr Real DefaultTime       = 0;
      static constexpr Real DefaultPrecedence = 0;
      static constexpr Real MinPrecedence     = -10000;
      static constexpr Real MaxPrecedence     = +10000;

      // MARK: Mass                                                     
      // Imagine the following spectrum:                                
      //          run away < avoid < approach < touch < punch           
      // You can use mass to reduce them to a single function with      
      // different magnitudes (a.k.a. mass). Similarily, multiplication 
      // can be represented as an addition with magnitude:              
      //          5*5 -> 5 + 5 + 5 + 5 + 5 -> 0 add*5 5                 
      // Like any other dimension, mass is context-dependent. The flow  
      // determines what the relative scale of mass is. For example, in 
      // a logarithmic context such as planetary bodies, a mass         
      // difference of 1 can mean an order of magnitude instead.        
      Real mass = DefaultMass;

      // MARK: Rate                                                     
      // Imagine the following spectrum:                                
      //          do once < once per second < until < forever           
      // You can use frequency to basically decide whether code will    
      // be executed once, in a particular update stage, or forever.    
      // This will perform "write 5" in the current context on every    
      // tick, until the program is stopped. ^1 instructs the check to  
      // be performed once on each update call.                         
      // Like any other dimension, rate is context-dependent. The       
      // flow determines what the relative scale of frequency is. For   
      // example, in the GPU rendering context, rate determines on      
      // which shading stage a code is executed:                        
      // ^0 - executed once as a precomputed constant                   
      // ^1 - executed once per render call                             
      // ^2 - executed once per rendered pass                           
      // ^3 - executed once per rendered object                         
      // ^4 - executed once per vertex                                  
      // ^5 - executed once per primitive                               
      // ^6 - executed once per pixel                                   
      Real rate = DefaultRate;

      // MARK: Time                                                     
      // Imagine the following spectrum:                                
      //          did long ago < did recently < doing now < will do     
      // You can use time dimension to decide where in the context-     
      // dependent timeline a function will go. In general everyday     
      // talk, this can mean relatively recent events, while in more    
      // historical contexts, a unit could signify a span of millenia.  
      Real time = DefaultTime;

      // MARK: Precedence                                               
      // Much like in math, functions can have different precedences.   
      // For example, multiplication is always done before addition.    
      // Also acts as an explicit manual knob to tune the order of      
      // functions inside a flow.                                       
      Real precedence = DefaultPrecedence;

      /*constexpr Charge(
         Real m = DefaultMass,
         Real r = DefaultRate,
         Real t = DefaultTime,
         Real p = DefaultPrecedence
      ) noexcept : mass{m}, rate{r}, time{t}, precedence{p} {}*/

      constexpr bool operator == (const Charge& rhs) const noexcept = default; /*{
         return mass == rhs.mass
            and rate == rhs.rate
            and time == rhs.time
            and precedence == rhs.precedence;
      }*/

      /// Return a new charge with scaled mass                                
      constexpr auto operator *  (const Real& scalar) const noexcept -> Charge {
         return {mass * scalar, rate, time, precedence};
      }

      /// Return a new charge with scaled rate                                
      constexpr auto operator ^  (const Real& scalar) const noexcept -> Charge {
         return {mass, rate * scalar, time, precedence};
      }

      /// Scale mass only                                                     
      constexpr auto operator *= (const Real& scalar) noexcept -> Charge {
         mass *= scalar;
         return *this;
      }

      /// Scale frequency only                                                
      constexpr auto operator ^= (const Real& scalar) noexcept -> Charge {
         rate *= scalar;
         return *this;
      }

      /// Check if charge is default-constructed                              
      constexpr bool IsCharged() const noexcept {
         return *this != Charge {};
      }

      /// Mass doesn't affect a charged thing's position inside flow, but all 
      /// other dimensions do. Check if they are not default-constructed.     
      /// If a charged thing is flow-dependent, it needs to have contextual   
      /// considerations before nestling in its proper place inside the flow. 
      constexpr bool IsFlowDependent() const noexcept {
         return rate != DefaultRate
             or time != DefaultTime
             or precedence != DefaultPrecedence;
      }

      /// Reset the charge, as if it is default-constructed                   
      void ResetCharge() noexcept {
         mass        = DefaultMass;
         rate        = DefaultRate;
         time        = DefaultTime;
         precedence  = DefaultPrecedence;
      }
   };

   static_assert(::std::is_standard_layout_v<Charge>);
   static_assert(::std::is_trivially_destructible_v<Charge>);
}

namespace Langulus::CTTI
{
   /// Extends T by marking it as charged at compile-time. You still need to  
   /// give the proper interface to be usable. Easiest way to do that is to   
   /// inherit from Langulus::Charge.                                         
   /// 1) template<> struct Charged<YourType> { };                            
   /// 2) struct YourType { using CTTI_Charged = Yup; };                      
   template<class T>
   struct Charged;
}

LANGULUS_CTTI_CONCEPT_DECVQ(Charged);
