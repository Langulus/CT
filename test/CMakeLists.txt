add_langulus_test(LangulusCTTest
   SOURCES     TestAssume.cpp
               TestCTTI.cpp
               TestHashOf.cpp
               TestIntentOf.cpp
               TestLambda.cpp
               TestNameOf.cpp
               TestRetype.cpp
               TestTypenav.cpp
               TestTypeOf.cpp
               
               Utils/TestLiteral.cpp
               Utils/TestSequence.cpp
               Utils/TestTypes.cpp

               CT/TestAbstract.cpp
               CT/TestAkin.cpp
               CT/TestAllocatable.cpp
               CT/TestBool.cpp
               CT/TestCharacter.cpp
               CT/TestConvertible.cpp
               CT/TestCoreConcepts.cpp
               CT/TestDeep.cpp
               CT/TestInteger.cpp
               CT/TestNumber.cpp
               CT/TestPOD.cpp
               CT/TestReal.cpp
               CT/TestReflectAs.cpp
               CT/TestSuffix.cpp
               CT/TestUnfold.cpp

   LIBRARIES   LangulusCT
)