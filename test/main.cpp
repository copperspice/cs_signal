/***********************************************************************
*
* Copyright (c) 2015-2020 Barbara Geller
* Copyright (c) 2015-2020 Ansel Sermersheim
*
* This file is part of CsSignal.
*
* CsSignal is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
*
* CopperSpice is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#include <atomic>

#include <stdio.h>

#include "peach.h"

bool g_unitTest = true;

void test_1();
void test_2();
void test_3();
void test_4();
void test_5();
void test_6();
void test_7();
void test_8();

int main()
{
   printf("\n\n%s\n\n", "** CsSignal Library - Start of Unit Test **");

   test_1();
   test_2();
   test_3();
   test_4();
   test_5();
   test_6();
   test_7();
   test_8();

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

   connect(okButton, &TestPushButton::pressed, obj, &Peach::methodPressed);

   if (obj.m_slotPressed != 0) {
      // ensure slot has not been accidentally called
      ok = false;
   }

   // call the signal
   okButton.pressed();

   if (obj.m_slotPressed != 1) {
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

void funcPressed()
{
   printf("  SLOT: pressed (slot is a function pointer)\n");
}

void test_2()
{
   bool ok = true;
   printf("Begin Unit Test Two\n");

   TestPushButton okButton;
   Peach obj = Peach{};

   connect(okButton, &TestPushButton::pressed, obj, &funcPressed);

   // call the signal
   okButton.pressed();

   if (ok) {
      printf("End Unit Test Two - PASSED\n\n");

   } else {
      printf("End Unit Test Two - Failed\n\n");
      g_unitTest = false;

   }
}

void test_3()
{
   bool ok = true;
   printf("Begin Unit Test Three\n");

   int slotPressed = 0;

   TestPushButton okButton;
   Peach obj = Peach{};

   connect(okButton, &TestPushButton::pressed, obj, [&slotPressed]()
   {
      printf("  SLOT: pressed (slot is a lambda)\n");
      slotPressed++;
   } );

   if (slotPressed != 0) {
      // ensure slot has not been accidentally called
      ok = false;
   }

   // call the signal
   okButton.pressed();

   if (slotPressed != 1) {
      // ensure slot has been called once
      ok = false;
   }

   if (ok) {
      printf("End Unit Test Three - PASSED\n\n");

   } else {
      printf("End Unit Test Three - Failed\n\n");
      g_unitTest = false;

   }
}

void test_4()
{
   bool ok = true;
   printf("Begin Unit Test Four\n");

   TestPushButton okButton;
   Peach obj = Peach{};

   connect(okButton, &TestPushButton::pressed, obj, &Peach::templatePressed<int> );

   if (obj.m_slotPressed != 0) {
      // ensure slot has not been accidentally called
      ok = false;
   }

   // call the signal
   okButton.pressed();

   if (obj.m_slotPressed != 1) {
      // ensure slot has been called once
      ok = false;
   }

   if (ok) {
      printf("End Unit Test Four - PASSED\n\n");

   } else {
      printf("End Unit Test Fout - Failed\n\n");
      g_unitTest = false;

   }
}

void callBack(std::atomic<bool> &running, std::deque<CsSignal::PendingSlot> &array, std::mutex &mutex,
                  std::condition_variable &alarm)
{
   printf("  Test 5: Message from thread\n");

   while (true) {
      std::unique_lock<std::mutex> lock(mutex);

      if (! array.empty())  {
         auto data = std::move(array.front());
         array.pop_front();
         lock.unlock();

         // call the slot
         data();
         continue;

      } else if (! running) {
         break;

      }

      alarm.wait(lock);
   }
}

void test_5()
{
   bool ok = true;
   printf("Begin Unit Test Five\n");

   // set up threads
   std::atomic<bool> running;
   running = true;

   std::deque<CsSignal::PendingSlot> array;
   std::mutex mutex;
   std::condition_variable alarm;

   std::thread thread1(callBack, std::ref(running), std::ref(array), std::ref(mutex), std::ref(alarm));

   Peach obj;
   obj.m_array = &array;
   obj.m_mutex = &mutex;
   obj.m_alarm = &alarm;

   TestPushButton okButton;

   connect(okButton, &TestPushButton::pressed, obj, &Peach::threadPressed, CsSignal::ConnectionKind::QueuedConnection);

   if (obj.m_slotPressed != 0) {
      // ensure the slots were not been accidentally called
      ok = false;
   }

   // emit the signal
   okButton.pressed();

   running = false;

   // wake up the thread
   alarm.notify_one();

   thread1.join();

   if (obj.m_slotPressed != 1) {
      // ensure slot has been called once
      ok = false;
   }

   if (ok) {
      printf("End Unit Test Five - PASSED\n\n");

   } else {
      printf("End Unit Test Five - Failed\n\n");
      g_unitTest = false;

   }
}

void test_6()
{
   bool ok = true;
   printf("Begin Unit Test Six\n");

   TestPushButton okButton;
   Peach obj = Peach{};

   connect(okButton, &TestPushButton::pressed, obj, &Peach::methodPressed);

   if (obj.m_slotPressed != 0) {
      // ensure slot has not been accidentally called
      ok = false;
   }

   // call the signal
   okButton.pressed();

   //
   printf("  Disconnect() then emit signal again\n");
   disconnect(okButton, &TestPushButton::pressed, obj, &Peach::methodPressed);
   okButton.pressed();

   if (obj.m_slotPressed == 1) {
      printf("  Signal emitted after disconnect(), did nothing\n");

   } else {
      // ensure slot has been called once
      ok = false;
   }

   if (ok) {
      printf("End Unit Test Six - PASSED\n\n");

   } else {
      printf("End Unit Test Six - Failed\n\n");
      g_unitTest = false;

   }
}

void test_7()
{
   bool ok = true;
   printf("Begin Unit Test Seven\n");

   TestPushButton okButton;
   Peach obj = Peach{};

   // testing a signal with a parameter
   connect(okButton, &TestPushButton::toggled, obj, &Peach::toggled);

   if (obj.m_slotPressed != 0) {
      // ensure slot has not been accidentally called
      ok = false;
   }

   // call the signal
   okButton.toggled(true);

   if (obj.m_slotPressed != 1) {
      // ensure slot has been called once
      ok = false;
   }

   if (ok) {
      printf("End Unit Test Seven - PASSED\n\n");

   } else {
      printf("End Unit Test Seven - Failed\n\n");
      g_unitTest = false;

   }
}

void test_8()
{
   bool ok = true;
   printf("Begin Unit Test Eight\n");

   if (true) {

      TestPushButton okButton;
      Peach obj = Peach{};

      // sender is an rvalue
      connect(TestPushButton{}, &TestPushButton::pressed, obj, &funcPressed);

      // receiver is an rvalue
      // connect(okButton, &TestPushButton::pressed, Peach{}, &funcPressed);

      // sender and receiver are rvalues
      // connect(TestPushButton{}, &TestPushButton::pressed, Peach{}, &funcPressed);

   } else {
       printf("  Not enabled, used to test for compile issues\n");

   }

   if (ok) {
      printf("End Unit Test Eight - PASSED\n\n");

   } else {
      printf("End Unit Test Eight - Failed\n\n");
      g_unitTest = false;

   }
}

