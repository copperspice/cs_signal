/***********************************************************************
*
* Copyright (c) 2016-2021 Barbara Geller
* Copyright (c) 2016-2021 Ansel Sermersheim
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

#ifndef TEST_PEACH_H
#define TEST_PEACH_H

#include <deque>
#include <condition_variable>

#include <cs_signal.h>
#include <cs_slot.h>

class TestPushButton : public CsSignal::SignalBase
{
   public:
      SIGNAL_1(void pressed())
      SIGNAL_2(pressed)

      SIGNAL_1(void clicked(bool checked = false))
      SIGNAL_2(clicked, checked)

      SIGNAL_1(void toggled(bool checked))
      SIGNAL_2(toggled, checked)
};

class Peach : public CsSignal::SlotBase
{
   public:
      Peach();

      void toggled(bool onOff);
      void methodPressed();
      void threadPressed();

      template<class T>
      void templatePressed();

      int m_slotPressed = 0;

      std::deque<CsSignal::PendingSlot> *m_array;
      std::mutex *m_mutex;
      std::condition_variable *m_alarm;

   private:
      void queueSlot(CsSignal::PendingSlot data, CsSignal::ConnectionKind type) override;
};

template<class T>
void Peach::templatePressed()
{
   printf("  Peach SLOT: pressed (slot is a template method)\n");
   m_slotPressed++;
}

#endif