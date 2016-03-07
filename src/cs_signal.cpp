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

CsSignal::SignalBase::~SignalBase()
{

   // broom:  m_beingDestroyed    set to true and then wait for activate 


}

void CsSignal::SignalBase::addConnection(const Internal::BentoAbstract *signalMethod, const SlotBase *receiver,
                  const Internal::BentoAbstract *slotMethod, ConnectionType type) const
{
   struct ConnectStruct tempStruct;
   
   tempStruct.signalMethod = signalMethod;
   tempStruct.receiver     = receiver;
   tempStruct.slotMethod   = slotMethod;
   tempStruct.type         = type;

   // list is in sender
   this->m_connectList.push_back(std::move(tempStruct));

   if (m_activateBusy) {
      // warns activate the connectList has changed
      this->m_raceCount++;
   }

   if (receiver != nullptr)  {
      // list is in receiver  
      std::unique_lock<std::mutex> receiverLock {receiver->m_mutex_possibleSenders};
      receiver->m_possibleSenders.push_back(this);
   }
}

bool CsSignal::SignalBase::isSignalConnected(const Internal::BentoAbstract &signalMethod_Bento) const
{
   bool retval = false;
   std::unique_lock<std::mutex> senderLock {this->m_mutex_ToReceiver};

   for (auto &item : this->m_connectList) {

      if (*(item.signalMethod) != signalMethod_Bento)  {
         continue;
      }

      if (item.type == ConnectionType::InternalDisconnected) {
         // connection is marked for deletion
         continue;
      }

      retval = true;
      break;
   }

   return retval;
}


