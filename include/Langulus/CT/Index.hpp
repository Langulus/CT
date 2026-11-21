///                                                                           
/// Langulus::CT                                                              
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Integer.hpp"
#include "Signed.hpp"
#include "DefineConst.hpp"
#include <limits>
#include <stdexcept>


namespace Langulus::CTTI
{
   /// Affects CT::Index<T>                                                   
   template<class T>
   struct Index;

   /// All CT::Integer satisfy CT::Index as well                              
   template<CT::Integer T>
   struct Index<T> {};
}

LANGULUS_CTTI_CONCEPT_UNSHEDDABLE(Index);

namespace Langulus::Index
{
   struct At;

   namespace Inner
   {
      template<bool VALID, int64_t CODE>
      struct Common {
         int64_t index = CODE;

         using CTTI_Index     = Yup;
         using CTTI_ReflectAs = At;

         constexpr explicit operator bool() const noexcept {
            return VALID;
         }

         constexpr bool IsSpecial() const noexcept {
            return true;
         }

         constexpr bool operator == (this auto const& self, CT::Index auto const& i) noexcept {
            return self.index == i.index;
         }
      };

      struct None     : Common<false, ::std::numeric_limits<int64_t>::max()   > {};
      struct Last     : Common<true,  ::std::numeric_limits<int64_t>::min()   > {};
      struct All      : Common<true,  ::std::numeric_limits<int64_t>::min()+1 > {};
      struct Many     : Common<true,  ::std::numeric_limits<int64_t>::min()+2 > {};
      struct Single   : Common<true,  ::std::numeric_limits<int64_t>::min()+3 > {};
      struct Front    : Common<true,  ::std::numeric_limits<int64_t>::min()+4 > {};
      struct Middle   : Common<true,  ::std::numeric_limits<int64_t>::min()+5 > {};
      struct Back     : Common<true,  ::std::numeric_limits<int64_t>::min()+6 > {};
      struct Modal    : Common<true,  ::std::numeric_limits<int64_t>::min()+7 > {};
      struct Biggest  : Common<true,  ::std::numeric_limits<int64_t>::min()+8 > {};
      struct Smallest : Common<true,  ::std::numeric_limits<int64_t>::min()+9 > {};
      struct Random   : Common<true,  ::std::numeric_limits<int64_t>::min()+10> {};
      struct First    : Common<true,  ::std::numeric_limits<int64_t>::min()+11> {};
   }

   /// Equivalent to the container's initialized count                        
   constexpr auto All = Inner::All {};

   /// Equivalent to a count larger than one                                  
   constexpr auto Many = Inner::Many {};

   /// Equivalent to a count of one                                           
   constexpr auto Single = Inner::Single {};

   /// Equivalent to a count of zero, or Container::CountType's max value     
   /// For std containers it's equivalent to 'npos'                           
   constexpr auto None = Inner::None {};

   /// Equivalent to the space before the first element in continuous memory  
   constexpr auto Front = Inner::Front {};

   /// Equivalent to the middle element of continuous memory, or the space    
   /// between the two middle elements, in the case that count is even        
   constexpr auto Middle = Inner::Middle {};

   /// Equivalent to the space after the last element in continuous memory    
   /// i.e. one-past the last element                                         
   constexpr auto Back = Inner::Back {};

   /// Equivalent to the first index of the most occuring element in memory   
   constexpr auto Modal = Inner::Modal {};

   /// Equivalent to the index of the first largest element in memory         
   constexpr auto Biggest = Inner::Biggest {};

   /// Equivalent to the index of the first smallest element in memory        
   constexpr auto Smallest = Inner::Smallest {};

   /// A random index inside a valid memory range                             
   constexpr auto Random = Inner::Random {};

   /// Equivalent to 0 for continuous memory                                  
   constexpr auto First = Inner::First {};

   /// Equivalent to size-1 for continuous memory                             
   constexpr auto Last = Inner::Last {};

   /// An arithmetic index that is evaluated to false if equal to npos        
   /// in order to easily use it in if statements, instead of constantly      
   /// comparing against npos like a chump.                                   
   //TODO some values should be reserved and mapped to the special indices above
   struct At {
      using CTTI_Index  = Yup;
      using CTTI_Named  = Yes<"Index">;
      using CTTI_POD    = Yup;
      using CTTI_Values = Types<
         NamedValue<All,      "Index::All",        "Represents all elements in a container">,
         NamedValue<Many,     "Index::Many",       "Represents more than one element">,
         NamedValue<Single,   "Index::Single",     "Represents one element">,
         NamedValue<None,     "Index::None",       "Represents no elements">,
         NamedValue<Front,    "Index::Front",      "Represents the location in front of a contiguous container">,
         NamedValue<Middle,   "Index::Middle",     "Represents the location in the middle of a contiguous container">,
         NamedValue<Back,     "Index::Back",       "Represents the location in the back of a contiguous container">,
         NamedValue<Modal,    "Index::Modal",      "Represents the most frequently occuring element">,
         NamedValue<Biggest,  "Index::Biggest",    "Represents the biggest element">,
         NamedValue<Smallest, "Index::Smallest",   "Represents the smallest element">,
         NamedValue<Random,   "Index::Random",     "Represents a random element">,
         NamedValue<First,    "Index::First",      "Represents the first element">,
         NamedValue<Last,     "Index::Last",       "Represents the last element">
      >;

      int64_t index;

      constexpr At(CT::Index auto const& i) noexcept
         : index {i.index} {}

      explicit constexpr At(Real r)
         : index {static_cast<int64_t>(r)} {
         if (IsSpecial())
            throw std::runtime_error{"Invalid arithmetic index"};
      }

      constexpr explicit operator bool() const noexcept {
         return index != ::std::numeric_limits<int64_t>::max();
      }

      constexpr int64_t TypedCast() const noexcept {
         return index;
      }

      constexpr bool operator == (CT::Index auto const& i) const noexcept {
         return index == i.index;
      }

      /*constexpr bool operator == (Inner::None     i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Last     i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::All      i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Many     i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Single   i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Front    i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Middle   i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Back     i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Modal    i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Biggest  i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Smallest i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::Random   i) const noexcept { return index == i.index; }
      constexpr bool operator == (Inner::First    i) const noexcept { return index == i.index; }*/

      /// If true, then indicates that the index needs processing before      
      /// turning into contiguous offset.                                     
      constexpr bool IsSpecial() const noexcept {
         if (index < 0)
            return true;

         return index == ::std::numeric_limits<int64_t>::max();
      }
   };
}
