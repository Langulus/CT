///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Core.hpp>


namespace Langulus::CT
{
   /// Check if any T is the built-in `No`, which signifies lack of support   
   template<class...T>
   concept Unsupported = (::std::is_same_v<T, No> or ...);

   /// Check if all T are supported                                           
   template<class...T>
   concept Supported = ((not Unsupported<T>) and ...);
}