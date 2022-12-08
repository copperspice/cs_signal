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

#include "demo.h"

void Demo_Receiver::toggled(bool onOff)
{
   ++m_slotPressed;
}

void Demo_Receiver::methodPressed()
{
   ++m_slotPressed;
}

void Demo_Receiver::threadPressed()
{
   ++m_slotPressed;
}

void Demo_Receiver::queueSlot(CsSignal::PendingSlot data, CsSignal::ConnectionKind)
{
   SlotBase *receiver = data.receiver();

   std::lock_guard<std::mutex> lock(*m_mutex);
   m_array->push_back(std::move(data));

   // wake up the thread
   m_alarm->notify_one();
}

