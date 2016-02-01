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

void test_1();
void test_2();


int main(int argc, char *argv[])
{
   printf("\n\n%s\n\n", "** CsSignal Library - Start of Unit Test **");

   test_1();   
   test_2();  

   printf("\n\n");
 
}

void test_1()
{
   printf("Hello from the CS Signal libray. How may I serve you?\n");
}

void test_2()
{
   printf("Run you clever Signal.\n");
}



/*

Ginger::Ginger()
{
}


Ginger::~Ginger()
{
}

*/