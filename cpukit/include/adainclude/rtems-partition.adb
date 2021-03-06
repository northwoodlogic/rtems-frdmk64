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

package body RTEMS.Partition is

   --
   -- Partition Manager
   --

   procedure Create
     (Name             : in RTEMS.Name;
      Starting_Address : in RTEMS.Address;
      Length           : in RTEMS.Size;
      Buffer_Size      : in RTEMS.Size;
      Attribute_Set    : in RTEMS.Attribute;
      ID               : out RTEMS.ID;
      Result           : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name             : RTEMS.Name;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Size;
         Buffer_Size      : RTEMS.Size;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.Event_Set)
         return             RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_partition_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result :=
         Create_Base
           (Name,
            Starting_Address,
            Length,
            Buffer_Size,
            Attribute_Set,
            ID_Base'Access);
      ID     := ID_Base;

   end Create;

   procedure Ident
     (Name   : in RTEMS.Name;
      Node   : in RTEMS.Unsigned32;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Ident_Base
        (Name : RTEMS.Name;
         Node : RTEMS.Unsigned32;
         ID   : access RTEMS.Event_Set)
         return RTEMS.Status_Codes;
      pragma Import (C, Ident_Base, "rtems_partition_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Ident_Base (Name, Node, ID_Base'Access);
      ID     := ID_Base;

   end Ident;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Delete_Base, "rtems_partition_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Get_Buffer
     (ID     : in RTEMS.ID;
      Buffer : out RTEMS.Address;
      Result : out RTEMS.Status_Codes)
   is
      function Get_Buffer_Base
        (ID     : RTEMS.ID;
         Buffer : access RTEMS.Address)
         return   RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Buffer_Base,
         "rtems_partition_get_buffer");
      Buffer_Base : aliased RTEMS.Address;
   begin

      Result := Get_Buffer_Base (ID, Buffer_Base'Access);
      Buffer := Buffer_Base;

   end Get_Buffer;

   procedure Return_Buffer
     (ID     : in RTEMS.ID;
      Buffer : in RTEMS.Address;
      Result : out RTEMS.Status_Codes)
   is
      function Return_Buffer_Base
        (ID     : RTEMS.Name;
         Buffer : RTEMS.Address)
         return   RTEMS.Status_Codes;
      pragma Import
        (C,
         Return_Buffer_Base,
         "rtems_partition_return_buffer");
   begin

      Result := Return_Buffer_Base (ID, Buffer);

   end Return_Buffer;

end RTEMS.Partition;
