/***********************************************************************
*
* Copyright (c) 2015-2016 Barbara Geller
* Copyright (c) 2015-2016 Ansel Sermersheim
* All rights reserved.
*
* This file is part of libCsSignal
*
* libCsSignal is free software: you can redistribute it and/or 
* modify it under the terms of the GNU Lesser General Public License
* version 2.1 as published by the Free Software Foundation.
*
* libCsSignal is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with CopperSpice.  If not, see 
* <http://www.gnu.org/licenses/>.
*
***********************************************************************/

#include <stdio.h>

#include "peach.h"

bool g_unitTest = true;

void test_1();
void test_2();

int main(int argc, char *argv[])
{
   printf("\n\n%s\n\n", "** CsSignal Library - Start of Unit Test **");

   test_1();   
   test_2();  

   printf("\n\n"); 

   if (! g_unitTest) {
      return 1;
   }
}

void test_1()
{
   bool ok = true;
   printf("Begin Unit Test One\n");

   TestPushButton okButton;  
   Peach obj = Peach{};

   connect(okButton, &TestPushButton::pressed, obj, &Peach::actionPressed);

   if (obj.m_slotActionPressed != 0) {
      // ensure slot has not been accidentally called
      ok = false;
   }

   // call the signal
   okButton.pressed();

   if (obj.m_slotActionPressed != 1) {
      // ensure slot has been called once
      ok = false;
   }

   if (ok) {
      printf("End Unit Test One - PASSED\n\n");

   } else {
      printf("End Unit Test One - Failed\n\n");
      g_unitTest = false;
      
   }
}

void test_2()
{
   bool ok = true;
   printf("Begin Unit Test Two\n");

   TestPushButton okButton;  
   Peach obj = Peach{};

   connect(okButton, &TestPushButton::pressed, obj, &Peach::actionPressed, CsSignal::ConnectionType::QueuedConnection);

   if (obj.m_slotActionPressed != 0) {
      // ensure slot has not been accidentally called
      ok = false;
   }

   // call the signal
   okButton.pressed();

   if (obj.m_slotActionPressed != 1) {
      // ensure slot has been called once
      ok = false;
   }

   if (ok) {
      printf("End Unit Test Two - PASSED\n\n");

   } else {
      printf("End Unit Test Two - Failed\n\n");
      g_unitTest = false;
      
   }
}



