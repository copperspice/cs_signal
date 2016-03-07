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

#ifndef TEST_PEACH_H
#define TEST_PEACH_H

#include <deque>
#include <condition_variable>

#include <cs_signal.h>
#include <cs_slot.h>

class TestPushButton : public CsSignal::SignalBase
{   
   public: 
      SIG_1(void pressed())
      SIG_2(pressed)

      SIG_1(void clicked(bool checked = false))
      SIG_2(clicked, checked) 

      SIG_1(void toggled(bool checked))
      SIG_2(toggled, checked)     
};

class Peach : public CsSignal::SlotBase
{
   public:
      Peach();

      void methodPressed();    
      void threadPressed();  

      template<class T>
      void templatePressed();

      int m_slotPressed = 0;

      std::deque<CsSignal::PendingSlot> *m_array;
      std::mutex *m_mutex;
      std::condition_variable *m_alarm;
     
   private:    
      void queueSlot(CsSignal::PendingSlot data, CsSignal::ConnectionType type) override;     
};

template<class T>
void Peach::templatePressed()
{
   printf("  Peach SLOT: pressed (slot is a template method)\n");
   m_slotPressed++;
}

#endif