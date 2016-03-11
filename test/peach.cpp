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

Peach::Peach()
{  
}

void Peach::toggled(bool onOff)
{
   printf("  Peach SLOT: pushButton toggled is (0 or 1) %d\n", onOff);
   m_slotPressed++;
}

void Peach::methodPressed()
{
   printf("  Peach SLOT: pressed (slot is a method pointer)\n");
   m_slotPressed++;
}

void Peach::threadPressed()
{
   printf("  Peach SLOT: pressed (slot is from a thread)\n");
   m_slotPressed++;
}

void Peach::queueSlot(CsSignal::PendingSlot data, CsSignal::ConnectionType)
{   
   SlotBase *receiver = data.receiver();   
   printf("  queueSlot(): receiver is %s\n", typeid(*receiver).name());   

   std::lock_guard<std::mutex> lock(*m_mutex);
   m_array->push_back(std::move(data)); 

   // wake up the thread
   m_alarm->notify_one();
} 




