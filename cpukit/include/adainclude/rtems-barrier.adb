-- SPDX-License-Identifier: BSD-2-Clause

--
--  RTEMS / Body
--
--  DESCRIPTION:
--
--  This package provides the interface to the RTEMS API.
--
--
--  DEPENDENCIES:
--
--
--
--  COPYRIGHT (c) 1997-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions
--  are met:
--  1. Redistributions of source code must retain the above copyright
--     notice, this list of conditions and the following disclaimer.
--  2. Redistributions in binary form must reproduce the above copyright
--     notice, this list of conditions and the following disclaimer in the
--     documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
--

with Ada;
with Ada.Unchecked_Conversion;
with Interfaces;               use Interfaces;
with Interfaces.C;             use Interfaces.C;
with Interfaces.C.Strings;     use Interfaces.C.Strings;

package body RTEMS.Barrier is

   --
   -- Barrier Manager
   --

   procedure Create
     (Name            : in RTEMS.Name;
      Attribute_Set   : in RTEMS.Attribute;
      Maximum_Waiters : in RTEMS.Unsigned32;
      ID              : out RTEMS.ID;
      Result          : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name            : RTEMS.Name;
         Attribute_Set   : RTEMS.Attribute;
         Maximum_Waiters : RTEMS.Unsigned32;
         ID              : access RTEMS.ID)
         return            RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_barrier_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result :=
         Create_Base
           (Name,
            Attribute_Set,
            Maximum_Waiters,
            ID_Base'Access);
      ID     := ID_Base;

   end Create;

   procedure Ident
     (Name   : in RTEMS.Name;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Ident_Base
        (Name : RTEMS.Name;
         ID   : access RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Ident_Base, "rtems_barrier_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Ident_Base (Name, ID_Base'Access);
      ID     := ID_Base;

   end Ident;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Delete_Base, "rtems_barrier_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Wait
     (ID      : in RTEMS.ID;
      Timeout : in RTEMS.Interval;
      Result  : out RTEMS.Status_Codes)
   is
      function Wait_Base
        (ID      : RTEMS.ID;
         Timeout : RTEMS.Interval)
         return    RTEMS.Status_Codes;
      pragma Import (C, Wait_Base, "rtems_barrier_wait");
   begin

      Result := Wait_Base (ID, Timeout);

   end Wait;

   procedure Release
     (ID       : in RTEMS.ID;
      Released : out RTEMS.Unsigned32;
      Result   : out RTEMS.Status_Codes)
   is
      function Release_Base
        (ID       : RTEMS.ID;
         Released : access RTEMS.Unsigned32)
         return     RTEMS.Status_Codes;
      pragma Import (C, Release_Base, "rtems_barrier_release");
      Released_Base : aliased RTEMS.Unsigned32;
   begin

      Result   := Release_Base (ID, Released_Base'Access);
      Released := Released_Base;

   end Release;

end RTEMS.Barrier;
