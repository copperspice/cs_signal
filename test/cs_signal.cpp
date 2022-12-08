/***********************************************************************
*
* Copyright (c) 2016-2022 Barbara Geller
* Copyright (c) 2016-2022 Ansel Sermersheim
*
* This file is part of CsSignal.
*
* CsSignal is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
*
* CsSignal is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#define CS_STRING_ALLOW_UNSAFE

#include <cs_signal.h>
#include <demo.h>

#include <atomic>

#include <cs_catch2.h>

TEST_CASE("CsSignal traits", "[cs_signal]")
{
   REQUIRE(std::is_copy_constructible_v<CsSignal::SignalBase> == true);
   REQUIRE(std::is_move_constructible_v<CsSignal::SignalBase> == true);

   REQUIRE(std::is_copy_assignable_v<CsSignal::SignalBase> == false);
   REQUIRE(std::is_move_assignable_v<CsSignal::SignalBase> == false);

   REQUIRE(std::has_virtual_destructor_v<CsSignal::SignalBase> == true);
}

TEST_CASE("CsSignal connect_method", "[cs_signal]")
{
   Demo_PushButton okButton;

   Demo_Receiver objReceiver = Demo_Receiver{};

   connect(okButton, &Demo_PushButton::pressed, objReceiver, &Demo_Receiver::methodPressed);

   // ensure slot has not been accidentally called
   REQUIRE(objReceiver.m_slotPressed == 0);

   // call the signal
   okButton.pressed();

   // ensure slot has been called once
   REQUIRE(objReceiver.m_slotPressed == 1);
}

TEST_CASE("CsSignal connect_lambda", "[cs_signal]")
{
   int slotPressed = 0;

   Demo_PushButton okButton;

   Demo_Receiver objReceiver = Demo_Receiver{};

   connect(okButton, &Demo_PushButton::pressed, objReceiver,
      [&slotPressed]() { ++slotPressed; } );

   // ensure slot has not been called
   REQUIRE(slotPressed == 0);

   okButton.pressed();

   // ensure slot has been called once
   REQUIRE(slotPressed == 1);
}

TEST_CASE("CsSignal connect_template", "[cs_signal]")
{
   Demo_PushButton okButton;

   Demo_Receiver objReceiver = Demo_Receiver{};

   connect(okButton, &Demo_PushButton::pressed, objReceiver, &Demo_Receiver::templatePressed<int>);

   // ensure slot has not been called
   REQUIRE(objReceiver.m_slotPressed == 0);

   okButton.pressed();

   // ensure slot has been called once
   REQUIRE(objReceiver.m_slotPressed == 1);
}

TEST_CASE("CsSignal connect_toggled", "[cs_signal]")
{
   Demo_PushButton okButton;

   Demo_Receiver objReceiver = Demo_Receiver{};

   connect(okButton, &Demo_PushButton::toggled, objReceiver, &Demo_Receiver::toggled);

   // ensure slot has not been called
   REQUIRE(objReceiver.m_slotPressed == 0);

   okButton.toggled(true);

   // ensure slot has been called once
   REQUIRE(objReceiver.m_slotPressed == 1);
}

static void callBack(std::atomic<bool> &running, std::deque<CsSignal::PendingSlot> &array,
      std::mutex &mutex, std::condition_variable &alarm)
{
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

TEST_CASE("CsSignal discontent", "[cs_signal]")
{
   Demo_PushButton okButton;

   Demo_Receiver objReceiver = Demo_Receiver{};

   connect(okButton, &Demo_PushButton::pressed, objReceiver, &Demo_Receiver::methodPressed);

   // ensure slot has not been called
   REQUIRE(objReceiver.m_slotPressed == 0);

   okButton.pressed();

   disconnect(okButton, &Demo_PushButton::pressed, objReceiver, &Demo_Receiver::methodPressed);

   okButton.pressed();

   // ensure slot has been called once
   REQUIRE(objReceiver.m_slotPressed == 1);
}

TEST_CASE("CsSignal thread", "[cs_signal]")
{
   // set up threads
   std::atomic<bool> running;
   running = true;

   std::deque<CsSignal::PendingSlot> array;
   std::mutex mutex;
   std::condition_variable alarm;

   std::thread thread1(callBack, std::ref(running), std::ref(array), std::ref(mutex), std::ref(alarm));

   Demo_PushButton okButton;

   Demo_Receiver objReceiver;
   objReceiver.m_array = &array;
   objReceiver.m_mutex = &mutex;
   objReceiver.m_alarm = &alarm;

   connect(okButton, &Demo_PushButton::pressed, objReceiver, &Demo_Receiver::threadPressed,
         CsSignal::ConnectionKind::QueuedConnection);

   // ensure slot has not been called
   REQUIRE(objReceiver.m_slotPressed == 0);

   okButton.pressed();

   running = false;

   // wake up the thread
   alarm.notify_one();

   thread1.join();

   // ensure slot has been called
   REQUIRE(objReceiver.m_slotPressed == 1);
}

