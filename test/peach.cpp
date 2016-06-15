/***********************************************************************
*
* Copyright (c) 2015-2016 Barbara Geller
* Copyright (c) 2015-2016 Ansel Sermersheim
* All rights reserved.
*
* This file is part of libCsSignal
*
* libCsSignal is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
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

void Peach::queueSlot(CsSignal::PendingSlot data, CsSignal::ConnectionKind)
{   
   SlotBase *receiver = data.receiver();   
   printf("  queueSlot(): receiver is %s\n", typeid(*receiver).name());   

   std::lock_guard<std::mutex> lock(*m_mutex);
   m_array->push_back(std::move(data)); 

   // wake up the thread
   m_alarm->notify_one();
} 



