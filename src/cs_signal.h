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

#ifndef LIB_CS_SIGNAL_H
#define LIB_CS_SIGNAL_H

#include <algorithm>
#include <exception>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <tuple> 
#include <vector>

#include "cs_internal.h"
#include "cs_slot.h"

namespace CsSignal {

// ** signal macros
#define SIG_1(...)  \
   __VA_ARGS__ {
// do not remove the "{", this is required for part two of the macro

#define SIG_2(signalName, ...) \
      activate(*this, &std::remove_reference<decltype(*this)>::type::signalName, ##__VA_ARGS__); \
   }

enum class ConnectionType {
   AutoConnection,
   DirectConnection, 
   QueuedConnection,
   BlockingQueuedConnection,   
      
   // internal, do not use as this can change
   InternalDisconnected   
};

enum class DisconnectType { 
   DisconnectAll, 
   DisconnectOne 
};

// base class
class SignalBase
{
   public:
      virtual ~SignalBase();             
     
   private:
      mutable std::mutex m_mutex_ToReceiver;

      mutable bool m_beingDestroyed = false;            

      mutable int m_activateBusy;      
      mutable int m_raceCount;   

      struct ConnectStruct {         
         const Internal::BentoAbstract *signalMethod;
         const SlotBase *receiver;
         const Internal::BentoAbstract *slotMethod;
         ConnectionType type;
      };               

      // list of connections from my Signal to some Receiver
      mutable std::vector<ConnectStruct> m_connectList;    
     
      void addConnection(const Internal::BentoAbstract *signalMethod, const SlotBase *,
                  const Internal::BentoAbstract *slotMethod, ConnectionType type) const;    

      bool isSignalConnected(const Internal::BentoAbstract &signalMethod_Bento) const;

      template<class Sender, class SignalClass, class ...SignalArgTypes, class ...Ts>
      friend void activate(Sender &sender, void (SignalClass::*signal)(SignalArgTypes...), Ts &&... Vs); 
       
      template<class Sender, class SignalClass, class ...SignalArgs, class Receiver, class SlotClass, class ...SlotArgs, class SlotReturn>
      friend bool connect(const Sender &sender, void (SignalClass::*signalMethod)(SignalArgs...),
                  const Receiver &receiver, SlotReturn (SlotClass::*slotMethod)(SlotArgs...), 
                  ConnectionType type = ConnectionType::AutoConnection, bool uniqueConnection = false);   

      template<class Sender, class SignalClass, class ...SignalArgs, class Receiver, class T>
      friend bool connect(const Sender &sender, void (SignalClass::*signalMethod)(SignalArgs...), const Receiver &receiver, 
                  T slotLambda, ConnectionType type = ConnectionType::AutoConnection, bool uniqueConnection = false);

      template<class Sender, class Receiver>
      friend bool internal_disconnect(const Sender &sender, const Internal::BentoAbstract *signalBento, 
                  const Receiver *receiver, const Internal::BentoAbstract *slotBento); 
};

template<class Sender, class SignalClass, class ...SignalArgTypes, class ...Ts>
void activate(Sender &sender, void (SignalClass::*signal)(SignalArgTypes...), Ts &&... Vs) 
{ 
   // ensure signal args are passed
   static_assert( std::is_convertible<std::tuple<Ts...>, std::tuple<SignalArgTypes...>>::value,
                  "activate():  Signal parameter mismatch.");

   Internal::Bento<void (SignalClass::*)(SignalArgTypes...)> signal_Bento(signal);  

   bool isConnected = sender.isSignalConnected(signal_Bento);

   if (! isConnected)  {
      // nothing is connected to this signal
      return;
   }
 
   // threading and queuedConnections  
   std::unique_lock<std::mutex> senderLock {sender.m_mutex_ToReceiver};

   // store the signal data, false indicates the data will not be copied
   CsSignal::Internal::TeaCup_Data<SignalArgTypes...> dataPack(false, std::forward<Ts>(Vs)...);

   bool raceHappened = false;
   int maxCount = sender.m_connectList.size();

   SignalBase *priorSender   = SlotBase::t_currentSender;
   SlotBase::t_currentSender = &sender;

   for (int k = 0; k < maxCount; ++k) {
      const SignalBase::ConnectStruct &connection = sender.m_connectList[k];

      if (*(connection.signalMethod) != signal_Bento)  {
         // no match in connectionList for this signal
         continue;
      }

      if (connection.type == ConnectionType::InternalDisconnected) {
         // connection is marked for deletion
         continue;
      }

      SlotBase *receiver = const_cast<SlotBase *>(connection.receiver);
      const CsSignal::Internal::BentoAbstract *slot_Bento = connection.slotMethod;
     
      bool receiverInSameThread = receiver->compareThreads();

      if ( (connection.type == ConnectionType::AutoConnection && ! receiverInSameThread) ||
           (connection.type == ConnectionType::QueuedConnection)) {

            PendingSlot tempObj(&sender, signal_Bento.clone(), receiver, slot_Bento->clone(), 
                        Internal::make_unique<CsSignal::Internal::TeaCup_Data<SignalArgTypes...>>(true, std::forward<Ts>(Vs)... ));

            receiver->queueSlot(std::move(tempObj), ConnectionType::QueuedConnection);                        

      } else if (connection.type == ConnectionType::BlockingQueuedConnection) {

         senderLock.unlock();

         PendingSlot tempObj(&sender, signal_Bento.clone(), receiver, slot_Bento->clone(), 
                        Internal::make_unique<CsSignal::Internal::TeaCup_Data<SignalArgTypes...>>(true, std::forward<Ts>(Vs)... ));

         receiver->queueSlot(std::move(tempObj), ConnectionType::BlockingQueuedConnection);   
         senderLock.lock();

      } else if (connection.type == ConnectionType::DirectConnection || connection.type == ConnectionType::AutoConnection) {
         // direct connection

         sender.m_activateBusy++;
         int old_raceCount = sender.m_raceCount;

         senderLock.unlock();

         try {
            // invoke calls the actual method
            slot_Bento->invoke(receiver, &dataPack);

            if (sender.m_beingDestroyed) {
               // sender object was destroyed during the invoke of the slot
               SlotBase::t_currentSender = priorSender;
               sender.m_activateBusy--;
               return;
            }

         } catch (...) {
            senderLock.lock();

            if (receiverInSameThread) {     
               SlotBase::t_currentSender = priorSender;
            }

            throw;
         }

         try {
            senderLock.lock();

         } catch (std::exception &) {

            if (receiverInSameThread) {
               SlotBase::t_currentSender = priorSender;
            }
           
            throw std::invalid_argument("activate(): Failed to obtain sender lock");            
         }

         if (old_raceCount != sender.m_raceCount) {
            // connectionList modified
            raceHappened = true;

            maxCount = sender.m_connectList.size();

            // connect() can add an entry to the end of the list
            // disconnect() can mark a connection as pending deletion
         }

         sender.m_activateBusy--;        
      }
   }

   SlotBase::t_currentSender = priorSender;
         
   if (raceHappened && sender.m_activateBusy == 0)  {
      // finish clean up for disconnect
      sender.m_connectList.erase(std::remove_if(sender.m_connectList.begin(), sender.m_connectList.end(), 
                  [](const SignalBase::ConnectStruct & tmp){ return tmp.type == ConnectionType::InternalDisconnected; } ));
   }
}

// signal & slot method ptr
template<class Sender, class SignalClass, class ...SignalArgs, class Receiver, class SlotClass, class ...SlotArgs, class SlotReturn>
bool connect(const Sender &sender, void (SignalClass::*signalMethod)(SignalArgs...),
                  const Receiver &receiver, SlotReturn (SlotClass::*slotMethod)(SlotArgs...), 
                  ConnectionType type = ConnectionType::AutoConnection, bool uniqueConnection = false)
{
   // (1) Sender must be the same class as SignalClass OR (2) Sender is a child of SignalClass
   static_assert( std::is_base_of<SignalClass, Sender>::value,
                  "connect():  Signal was not a child class of Sender");

   // (1) Receiver must be the same class as SlotClass OR (2) Receiver is a child of SlotClass
   static_assert( std::is_base_of<SlotClass, Receiver>::value,
                  "connect():  Slot was not a child class of Receiver");  

   // compare signal and slot paramerter list
   static_assert( Internal::cs_check_connect_args<void (*)(SignalArgs...), void (*)(SlotArgs...) >::value,
                  "connect():  Incompatible signal/slot arguments");

   if (signalMethod == nullptr) {
      throw std::invalid_argument("connect() Can not connect, signal is null");      
   }

   if (slotMethod == nullptr) {
      throw std::invalid_argument("connect(): Can not connect, slot is null");
   }
     
   Internal::Bento<void (SignalClass::*)(SignalArgs...)> *signalMethod_Bento =
            new Internal::Bento<void (SignalClass::*)(SignalArgs...)>(signalMethod);

   Internal::Bento<void (SlotClass::*)(SlotArgs...)> *slotMethod_Bento = 
            new Internal::Bento<void (SlotClass::*)(SlotArgs...)>(slotMethod);

   std::unique_lock<std::mutex> senderLock {sender.m_mutex_ToReceiver};       

   if (uniqueConnection) {
      // ensure the connection is not added twice

      for (auto &item : sender.m_connectList) {
        
         if (item.receiver != &receiver) {
            continue;
         }

         if (*(item.signalMethod) != *(signalMethod_Bento))  {
            continue;
         }

         if (*(item.slotMethod) != *(slotMethod_Bento))  {
            continue;
         }

         // connection already exists
         return false;
      }
   }

   sender.addConnection(signalMethod_Bento, &receiver, slotMethod_Bento, type);

   return true;
}

// signal method ptr, slot lambda
template<class Sender, class SignalClass, class ...SignalArgs, class Receiver, class T>
bool connect(const Sender &sender, void (SignalClass::*signalMethod)(SignalArgs...), const Receiver &receiver, 
                  T slotLambda, ConnectionType type = ConnectionType::AutoConnection, bool uniqueConnection = false)
{   
   // Sender must be the same class as SignalClass and Sender is a child of SignalClass
   Internal::cs_testConnect_SenderSignal<Sender, SignalClass>();

   // compare signal and slot paramerter list
   Internal::cs_testConnect_SignalSlotArgs_1<T, SignalArgs...>();  

   if (signalMethod == nullptr) {
      throw std::invalid_argument("connect(): Can not connect, signal is null");
   }
   
   Internal::Bento<void (SignalClass::*)(SignalArgs...)> *signalMethod_Bento = 
                  new Internal::Bento<void (SignalClass::*)(SignalArgs...)>(signalMethod);

   Internal::Bento<T> *slotLambda_Bento = new Internal::Bento<T>(slotLambda);

   std::unique_lock<std::mutex> senderLock {sender.m_mutex_ToReceiver};
  
   if (uniqueConnection) {
      // ensure the connection is not added twice

      for (auto &item : sender.m_connectList) {       

         if (item.receiver != &receiver) {
            continue;
         }

         if (*(item.signalMethod) != *(signalMethod_Bento))  {
            continue;
         }

         // unable to test if the passed slotLambda = slotLambda_Bento

         // connection already exists
         return false;
      }      
   }

   sender.addConnection(signalMethod_Bento, &receiver, slotLambda_Bento, type);

   return true;
}

// signal & slot method ptr
template<class Sender, class SignalClass, class ...SignalArgs, class Receiver, class SlotClass, class ...SlotArgs, class SlotReturn>
bool disconnect(const Sender &sender, void (SignalClass::*signalMethod)(SignalArgs...), const Receiver &receiver, 
                  SlotReturn (SlotClass::*slotMethod)(SlotArgs...))
{   
   // Sender must be the same class as SignalClass and Sender is a child of SignalClass
   Internal::cs_testConnect_SenderSignal<Sender, SignalClass>();

   // Receiver must be the same class as SlotClass and Receiver is a child of SlotClass
   Internal::cs_testConnect_ReceiverSlot<SlotClass, Receiver>();

   // signal & slot arguments do not agree
   Internal::cs_testConnect_SignalSlotArgs_2< void (*)(SignalArgs...), void (*)(SlotArgs...) >();
   
   Internal::Bento<void (SignalClass::*)(SignalArgs...)> signalMethod_Bento(signalMethod);
   Internal::Bento<void (SlotClass::*)(SlotArgs...)> slotMethod_Bento(slotMethod);

   if (! internal_disconnect(sender, &signalMethod_Bento, &receiver, &slotMethod_Bento)) {
      return false;
   }

   return true;
}

// slot lambda or slot function ptr
template<class Sender, class SignalClass, class ...SignalArgs, class Receiver, class T>
bool disconnect(const Sender &sender, void (SignalClass::*signalMethod)(SignalArgs...), const Receiver &receiver, T slotMethod)
{
   // lambda, compile error
   static_assert(std::is_convertible<decltype(slotMethod == slotMethod), bool>::value,
                 "disconnect():  Slot argument invalid or calling disconnect using a lambda" );

   // function ptr
   Internal::Bento<void (SignalClass::*)(SignalArgs...)> signalMethod_Bento(signalMethod);
   Internal::Bento<T> slotMethod_Bento(slotMethod);

   if (! internal_disconnect(sender, &signalMethod_Bento, &receiver, &slotMethod_Bento)) {
      return false;
   }

   return true;
}

template<class Sender, class Receiver>
bool internal_disconnect(const Sender &sender, const Internal::BentoAbstract *signalBento, 
                  const Receiver *receiver, const Internal::BentoAbstract *slotBento)
{
   bool retval = false;
   bool isDone = false;

   std::unique_lock<std::mutex> senderLock {sender.m_mutex_ToReceiver};
   std::unique_lock<std::mutex> receiverLock;

   if (receiver != nullptr) {
      receiverLock = std::unique_lock<std::mutex> {receiver->m_mutex_possibleSenders};
   }

   int maxCount = sender.m_connectList.size();

   for (int k = 0; k < maxCount; ++k) {
      SignalBase::ConnectStruct &temp = sender.m_connectList[k];
     
      bool isMatch = false;

      if (signalBento == nullptr && receiver == nullptr) {
         // delete all connections in Sender
         isMatch = true;

      } else if (receiver != nullptr)  {

         if (receiver == temp.receiver) {

            if (signalBento == nullptr && (slotBento == nullptr || *slotBento == *temp.slotMethod)) {
               isMatch = true;

            } else if (signalBento != nullptr && *signalBento == *temp.signalMethod && (slotBento == nullptr ||
                       *slotBento == *temp.slotMethod)) {
               isMatch = true;

            }
         }

      } else if (signalBento != nullptr) {
         // receiver must be null therefore slot is null

         if (*signalBento == *temp.signalMethod) {
            isMatch = true;
         }
      }

      if (isMatch)  {
         // delete possible sender in the receiver
         retval = true;

         if (receiver == nullptr)  {
            // no receiver, lock temp.receiver now
            receiverLock = std::unique_lock<std::mutex> {temp.receiver->m_mutex_possibleSenders};
         
            auto &senderList = temp.receiver->m_possibleSenders;
            senderList.erase(std::find(senderList.rbegin(), senderList.rend(), &sender).base() - 1);
           
            receiverLock.unlock();

         } else {
            auto &senderList = temp.receiver->m_possibleSenders;
            senderList.erase(std::find(senderList.rbegin(), senderList.rend(), &sender).base() - 1);         

         }
    
         if (sender.m_activateBusy != 0) {

            if (! isDone) {
               // warn activate() the connectList has changed
               sender.m_raceCount++;
               isDone = true;
            }

            // mark connection for deletion, activate() will finish the clean up
            temp.type = ConnectionType::InternalDisconnected;

         } else {
            // delete conneciton in sender
            sender.m_connectList.erase(sender.m_connectList.begin() + k);

            // yes, this is required
            k = k - 1;
         }

      }
   }

   return retval;
}


}

#endif

