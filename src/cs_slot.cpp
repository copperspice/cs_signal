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

#include "cs_signal.h"
#include "cs_slot.h"

thread_local CsSignal::SignalBase *CsSignal::SlotBase::t_currentSender = nullptr;

CsSignal::SlotBase::SlotBase()
   : m_threadId(std::this_thread::get_id())
{
}

CsSignal::SlotBase::SlotBase(const SlotBase&)
   : m_threadId(std::this_thread::get_id())
{
}

CsSignal::SlotBase::~SlotBase()
{

   // broom: clean up possible Sender connections

}

CsSignal::SignalBase *CsSignal::SlotBase::sender() const
{
   return t_currentSender;
}

void CsSignal::SlotBase::queueSlot(PendingSlot data, ConnectionType type)
{   
   // calls the slot immediately
   data();
} 

bool CsSignal::SlotBase::compareThreads() const
{
   std::thread::id currentThreadId = std::this_thread::get_id();
   return currentThreadId == getThread();
}

CsSignal::PendingSlot::PendingSlot(SignalBase *sender, std::unique_ptr<Internal::BentoAbstract> signal_Bento, 
                  SlotBase *receiver, std::unique_ptr<Internal::BentoAbstract> slot_Bento, 
                  std::unique_ptr<Internal::TeaCupAbstract> teaCup_Data)
   : m_sender(sender), m_signal_Bento(std::move(signal_Bento)), m_receiver(receiver), 
     m_slot_Bento(std::move(slot_Bento)), m_teaCup_Data(std::move(teaCup_Data))
{
}

void CsSignal::PendingSlot::operator()() const
{
   // invoke the slot
   m_slot_Bento->invoke(m_receiver, m_teaCup_Data.get());
} 

