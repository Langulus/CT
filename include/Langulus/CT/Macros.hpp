///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Complete.hpp"


/// Checks for reflection traits inside types themselves.                     
/// Requires the PATTERN to be complete in order to do that.                  
#define LANGULUS_CTTI_DELVE_IN_EXTRACT(PATTERN, FALLBACK) \
   if constexpr (requires { PATTERN::Constant; }) { \
      if constexpr (PATTERN::Enabled) return PATTERN::Constant; \
      else return FALLBACK; \
   } else return FALLBACK;

/// Checks for reflection traits outside types by CTTI struct specializations 
#define LANGULUS_CTTI_CHECK_EXTRACT(TYPE, NAME, FALLBACK) ([] consteval { \
      if constexpr (CT::Complete<CTTI::NAME<TYPE>>) { \
         LANGULUS_CTTI_DELVE_IN_EXTRACT(CTTI::NAME<TYPE>, FALLBACK); \
      } else if constexpr (::std::is_class_v<TYPE>) { \
         static_assert(CT::Complete<TYPE>, \
            "Can't access `CTTI_" #NAME "` inside incomplete type " #TYPE); \
         LANGULUS_CTTI_DELVE_IN_EXTRACT(TYPE::CTTI_##NAME, FALLBACK); \
      } else return FALLBACK; \
   }())
   
/// Checks for reflection traits inside types themselves.                     
/// Requires the TYPE to be complete in order to do that.                     
#define LANGULUS_CTTI_DELVE_IN(TYPE, NAME, FALLBACK) \
   if constexpr (::std::is_class_v<TYPE>) { \
      static_assert(Complete<TYPE>, \
         "Can't access `CTTI_" #NAME "` inside incomplete type " #TYPE); \
      if constexpr (requires { TYPE::CTTI_##NAME::Enabled; }) \
         return TYPE::CTTI_##NAME::Enabled; \
      else return FALLBACK; \
   } else return FALLBACK;

/// Checks for reflection traits outside types by CTTI struct specializations 
/// If CTTI struct is incomplete, it has no effect.                           
/// If CTTI struct has a Default member, LANGULUS_CTTI_DELVE_IN is checked    
///   before utilizing the Enabled member.                                    
/// If CTTI struct has no Default member, it is assumed specialized, and no   
///   LANGULUS_CTTI_DELVE_IN is required, the Enabled member is used.         
#define LANGULUS_CTTI_CHECK(TYPE, NAME) ([] consteval -> bool { \
      using ctti = CTTI::NAME<TYPE>; \
      if constexpr (Complete<ctti>) { \
         if constexpr(requires { ctti::Default; }) { \
            { LANGULUS_CTTI_DELVE_IN(TYPE, NAME, ctti::Enabled) } \
         } else if constexpr (requires { ctti::Enabled; }) { \
            return ctti::Enabled; \
         } else return true; \
      } else { LANGULUS_CTTI_DELVE_IN(TYPE, NAME, false) } \
   }())
   
/// Automatically populates the Langulus::CT namespace with the appropriate   
/// concepts, based on the provided Langulus::CTTI::<structure name>.         
/// Used to reduce boilerplate.                                               
///   @attention types need to be complete only if we end up 'delving in'     
///   @attention use this macro in the global namespace                       
///   @param NAME the name of the concept - must be the same as the trait in  
///      Langulus::CTTI::NAME                                                 
///   @param HOW how to filter the type when checking it. Use ShedDeref<T>    
///      by default                                                           
#define LANGULUS_CTTI_CONCEPT_INNER(NAME, HOW) \
   namespace Langulus::CT { \
      template<class...T> \
      concept NAME = PartialValidate<T...> \
          and (LANGULUS_CTTI_CHECK(HOW, NAME) and ...); \
      template<class...T> \
      concept Not##NAME = PartialValidate<T...> \
          and ((not NAME<T>) and ...); \
   }

/// Automatically populates the Langulus::CT namespace with the appropriate   
/// concepts, based on the provided Langulus::CTTI::<structure name>.         
/// Used to reduce boilerplate. Sheds all sheddables and dereferences.        
///   @attention types need to be complete only if we end up 'delving in'     
///   @attention removes all sheddables and dereferences                      
///   @attention use this macro in the global namespace                       
#define LANGULUS_CTTI_CONCEPT(NAME) \
   LANGULUS_CTTI_CONCEPT_INNER(NAME, ShedDeref<T>)

/// MARK: CTTI Macros                                                         
/// Automatically populates the Langulus::CT namespace with the appropriate   
/// concepts, based on the provided Langulus::CTTI::<structure name>.         
/// Used to reduce boilerplate. Removes only references.                      
///   @attention types need to be complete only if we end up 'delving in'     
///   @attention will only shed references                                    
///   @attention use this macro in the global namespace                       
#define LANGULUS_CTTI_CONCEPT_UNSHEDDABLE(NAME) \
   LANGULUS_CTTI_CONCEPT_INNER(NAME, Deref<T>)

/// Automatically populates the Langulus::CT namespace with the appropriate   
/// concepts, based on the provided Langulus::CTTI::<structure name>.         
/// Used to reduce boilerplate.                                               
///   @attention types need to be complete only if we end up 'delving in'     
///   @attention will shed only references and cv qualifiers                  
///   @attention use this macro in the global namespace                       
#define LANGULUS_CTTI_CONCEPT_UNSHEDDABLE_DECVQ(NAME) \
   LANGULUS_CTTI_CONCEPT_INNER(NAME, Decvq<Deref<T>>)

/// Automatically populates the Langulus::CT namespace with the appropriate   
/// concepts, based on the provided Langulus::CTTI::<structure name>.         
/// Used to reduce boilerplate. Removes qualifiers from argument.             
///   @attention types need to be complete only if we end up 'delving in'     
///   @attention will shed all sheddables, as well as references and cv       
///      qualifiers after that                                                
///   @attention use this macro in the global namespace                       
#define LANGULUS_CTTI_CONCEPT_DECVQ(NAME) \
   LANGULUS_CTTI_CONCEPT_INNER(NAME, Decvq<ShedDeref<T>>)

/// Automatically populates the Langulus::CT namespace with the appropriate   
/// concepts, based on the provided Langulus::CTTI::<structure name>.         
/// Used to reduce boilerplate. Removes qualifiers and extents from argument. 
///   @attention types need to be complete only if we end up 'delving in'     
///   @attention will shed all sheddables, as well as references, cv          
///      qualifiers, and extents after that                                   
///   @attention use this macro in the global namespace                       
#define LANGULUS_CTTI_CONCEPT_DECVQE(NAME) \
   LANGULUS_CTTI_CONCEPT_INNER(NAME, Decvq<DeextAll<ShedDeref<T>>>)

/// Automatically populates the Langulus::CT namespace with the appropriate   
/// concepts, based on the provided Langulus::CTTI::<structure name>.         
/// Used to reduce boilerplate. Decays the argument.                          
///   @attention types need to be complete only if we end up 'delving in'     
///   @attention will shed all sheddables, as well as references, pointers,   
///      and cv qualifiers after that                                         
///   @attention use this macro in the global namespace                       
#define LANGULUS_CTTI_CONCEPT_DECAY(NAME) \
   LANGULUS_CTTI_CONCEPT_INNER(NAME, Decay<T>)