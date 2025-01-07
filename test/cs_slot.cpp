/***********************************************************************
*
* Copyright (c) 2016-2025 Barbara Geller
* Copyright (c) 2016-2025 Ansel Sermersheim
*
* This file is part of CsSignal.
*
* CsSignal is free software which is released under the BSD 2-Clause license.
* For license details refer to the LICENSE provided with this project.
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

#include <cs_catch2.h>

TEST_CASE("CsSlot traits", "[cs_slot]")
{
   REQUIRE(std::is_copy_constructible_v<CsSignal::SlotBase> == true);
   REQUIRE(std::is_move_constructible_v<CsSignal::SlotBase> == true);

   REQUIRE(std::is_copy_assignable_v<CsSignal::SlotBase> == false);
   REQUIRE(std::is_move_assignable_v<CsSignal::SlotBase> == false);

   REQUIRE(std::has_virtual_destructor_v<CsSignal::SlotBase> == true);
}