add_langulus_test(LangulusCTTest
   SOURCES     TestAssume.cpp
               TestHashOf.cpp
               TestIntentOf.cpp
               TestLambda.cpp
               TestNameOf.cpp
               TestRetype.cpp
               TestTypenav.cpp
               
               Utils/TestLiteral.cpp
               Utils/TestSequence.cpp
               Utils/TestTypes.cpp

               CT/TestAbstract.cpp
               CT/TestAkin.cpp
               CT/TestBool.cpp
               CT/TestCharacter.cpp
               CT/TestConvertible.cpp
               CT/TestDeep.cpp
               CT/TestDefaultable.cpp
               CT/TestDestroyable.cpp
               CT/TestInteger.cpp
               CT/TestNumber.cpp
               CT/TestPOD.cpp
               CT/TestReal.cpp
               CT/TestReflectAs.cpp
               CT/TestSheddable.cpp
               CT/TestSuffix.cpp
               CT/TestTyped.cpp
               CT/TestUnallocatable.cpp
               CT/TestUnfold.cpp

   LIBRARIES   LangulusCT
)