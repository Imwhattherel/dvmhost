/**
* Digital Voice Modem - Host Software
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Host Software
*
*/
//
// Based on code from the MMDVMHost project. (https://github.com/g4klx/MMDVMHost)
// Licensed under the GPLv2 License (https://opensource.org/licenses/GPL-2.0)
//
/*
*   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
*   Copyright (C) 2019 by Bryan Biedenkapp N2PLL
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#if !defined(__DMR_DEFINES_H__)
#define __DMR_DEFINES_H__

#include "Defines.h"

namespace dmr
{
    // ---------------------------------------------------------------------------
    //  Constants
    // ---------------------------------------------------------------------------

    const uint32_t  DMR_FRAME_LENGTH_BITS = 264U;
    const uint32_t  DMR_FRAME_LENGTH_BYTES = 33U;

    const uint32_t  DMR_SYNC_LENGTH_BITS = 48U;
    const uint32_t  DMR_SYNC_LENGTH_BYTES = 6U;

    const uint32_t  DMR_EMB_LENGTH_BITS = 8U;
    const uint32_t  DMR_EMB_LENGTH_BYTES = 1U;

    const uint32_t  DMR_SLOT_TYPE_LENGTH_BITS = 8U;
    const uint32_t  DMR_SLOT_TYPE_LENGTH_BYTES = 1U;

    const uint32_t  DMR_EMBEDDED_SIGNALLING_LENGTH_BITS = 32U;
    const uint32_t  DMR_EMBEDDED_SIGNALLING_LENGTH_BYTES = 4U;

    const uint32_t  DMR_AMBE_LENGTH_BITS = 108U * 2U;
    const uint32_t  DMR_AMBE_LENGTH_BYTES = 27U;

    const uint32_t  DMR_LC_HEADER_LENGTH_BYTES = 12U;

    const uint8_t   BS_SOURCED_AUDIO_SYNC[] = { 0x07U, 0x55U, 0xFDU, 0x7DU, 0xF7U, 0x5FU, 0x70U };
    const uint8_t   BS_SOURCED_DATA_SYNC[] = { 0x0DU, 0xFFU, 0x57U, 0xD7U, 0x5DU, 0xF5U, 0xD0U };

    const uint8_t   MS_SOURCED_AUDIO_SYNC[] = { 0x07U, 0xF7U, 0xD5U, 0xDDU, 0x57U, 0xDFU, 0xD0U };
    const uint8_t   MS_SOURCED_DATA_SYNC[] = { 0x0DU, 0x5DU, 0x7FU, 0x77U, 0xFDU, 0x75U, 0x70U };

    const uint8_t   DIRECT_SLOT1_AUDIO_SYNC[] = { 0x05U, 0xD5U, 0x77U, 0xF7U, 0x75U, 0x7FU, 0xF0U };
    const uint8_t   DIRECT_SLOT1_DATA_SYNC[] = { 0x0FU, 0x7FU, 0xDDU, 0x5DU, 0xDFU, 0xD5U, 0x50U };

    const uint8_t   DIRECT_SLOT2_AUDIO_SYNC[] = { 0x07U, 0xDFU, 0xFDU, 0x5FU, 0x55U, 0xD5U, 0xF0U };
    const uint8_t   DIRECT_SLOT2_DATA_SYNC[] = { 0x0DU, 0x75U, 0x57U, 0xF5U, 0xFFU, 0x7FU, 0x50U };

    const uint8_t   DMR_MS_DATA_SYNC_BYTES[] = { 0x0DU, 0x5DU, 0x7FU, 0x77U, 0xFDU, 0x75U, 0x70U };
    const uint8_t   DMR_MS_VOICE_SYNC_BYTES[] = { 0x07U, 0xF7U, 0xD5U, 0xDDU, 0x57U, 0xDFU, 0xD0U };

    const uint8_t   SYNC_MASK[] = { 0x0FU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xF0U };

    // The PR FILL and Data Sync pattern.
    const uint8_t   DMR_IDLE_DATA[] = { TAG_DATA, 0x00U,
        0x53U, 0xC2U, 0x5EU, 0xABU, 0xA8U, 0x67U, 0x1DU, 0xC7U, 0x38U, 0x3BU, 0xD9U,
        0x36U, 0x00U, 0x0DU, 0xFFU, 0x57U, 0xD7U, 0x5DU, 0xF5U, 0xD0U, 0x03U, 0xF6U,
        0xE4U, 0x65U, 0x17U, 0x1BU, 0x48U, 0xCAU, 0x6DU, 0x4FU, 0xC6U, 0x10U, 0xB4U };

    // A silence frame only
    const uint8_t   DMR_SILENCE_DATA[] = { TAG_DATA, 0x00U,
        0xB9U, 0xE8U, 0x81U, 0x52U, 0x61U, 0x73U, 0x00U, 0x2AU, 0x6BU, 0xB9U, 0xE8U,
        0x81U, 0x52U, 0x60U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U, 0x73U, 0x00U,
        0x2AU, 0x6BU, 0xB9U, 0xE8U, 0x81U, 0x52U, 0x61U, 0x73U, 0x00U, 0x2AU, 0x6BU };

    const uint8_t   PAYLOAD_LEFT_MASK[] = { 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xF0U };
    const uint8_t   PAYLOAD_RIGHT_MASK[] = { 0x0FU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU };

    const uint8_t   VOICE_LC_HEADER_CRC_MASK[] = { 0x96U, 0x96U, 0x96U };
    const uint8_t   TERMINATOR_WITH_LC_CRC_MASK[] = { 0x99U, 0x99U, 0x99U };
    const uint8_t   PI_HEADER_CRC_MASK[] = { 0x69U, 0x69U };
    const uint8_t   DATA_HEADER_CRC_MASK[] = { 0xCCU, 0xCCU };
    const uint8_t   CSBK_CRC_MASK[] = { 0xA5U, 0xA5U };

    const uint32_t  DMR_SLOT_TIME = 60U;
    const uint32_t  AMBE_PER_SLOT = 3U;

    const uint8_t   DT_MASK = 0x0FU;

    const uint8_t   DMR_IDLE_RX = 0x80U;
    const uint8_t   DMR_SYNC_DATA = 0x40U;
    const uint8_t   DMR_SYNC_VOICE = 0x20U;

    const uint8_t   DMR_SLOT1 = 0x00U;
    const uint8_t   DMR_SLOT2 = 0x80U;

    const uint8_t   DPF_UDT = 0x00U;
    const uint8_t   DPF_RESPONSE = 0x01U;
    const uint8_t   DPF_UNCONFIRMED_DATA = 0x02U;
    const uint8_t   DPF_CONFIRMED_DATA = 0x03U;
    const uint8_t   DPF_DEFINED_SHORT = 0x0DU;
    const uint8_t   DPF_DEFINED_RAW = 0x0EU;
    const uint8_t   DPF_PROPRIETARY = 0x0FU;

    const uint32_t  DMR_MAX_PDU_COUNT = 32U;
    const uint32_t  DMR_MAX_PDU_LENGTH = 512U;

    const uint8_t   FID_ETSI = 0x00U;                       // ETSI Standard Feature Set
    const uint8_t   FID_DMRA = 0x10U;                       //

    const uint32_t  DMR_EXT_FNCT_CHECK = 0x0000U;           // Radio Check
    const uint32_t  DMR_EXT_FNCT_UNINHIBIT = 0x007EU;       // Radio Uninhibit
    const uint32_t  DMR_EXT_FNCT_INHIBIT = 0x007FU;         // Radio Inhibit
    const uint32_t  DMR_EXT_FNCT_CHECK_ACK = 0x0080U;       // Radio Check Ack
    const uint32_t  DMR_EXT_FNCT_UNINHIBIT_ACK = 0x00FEU;   // Radio Uninhibit Ack
    const uint32_t  DMR_EXT_FNCT_INHIBIT_ACK = 0x00FFU;     // Radio Inhibit Ack

    // Data Type(s)
    const uint8_t   DT_VOICE_PI_HEADER = 0x00U;
    const uint8_t   DT_VOICE_LC_HEADER = 0x01U;
    const uint8_t   DT_TERMINATOR_WITH_LC = 0x02U;
    const uint8_t   DT_CSBK = 0x03U;
    const uint8_t   DT_DATA_HEADER = 0x06U;
    const uint8_t   DT_RATE_12_DATA = 0x07U;
    const uint8_t   DT_RATE_34_DATA = 0x08U;
    const uint8_t   DT_IDLE = 0x09U;
    const uint8_t   DT_RATE_1_DATA = 0x0AU;

    // Dummy values
    const uint8_t   DT_VOICE_SYNC = 0xF0U;
    const uint8_t   DT_VOICE = 0xF1U;

    // Full-Link Control Opcode(s)
    const uint8_t   FLCO_GROUP = 0x00U;                     // GRP VCH USER - Group Voice Channel User
    const uint8_t   FLCO_PRIVATE = 0x03U;                   // UU VCH USER - Unit-to-Unit Voice Channel User
    const uint8_t   FLCO_TALKER_ALIAS_HEADER = 0x04U;       //
    const uint8_t   FLCO_TALKER_ALIAS_BLOCK1 = 0x05U;       //
    const uint8_t   FLCO_TALKER_ALIAS_BLOCK2 = 0x06U;       //
    const uint8_t   FLCO_TALKER_ALIAS_BLOCK3 = 0x07U;       //
    const uint8_t   FLCO_GPS_INFO = 0x08U;                  //

    // Control Signalling Block Opcode(s)
    const uint8_t   CSBKO_NONE = 0x00U;                     //
    const uint8_t   CSBKO_UU_V_REQ = 0x04U;                 // UU VCH REQ - Unit-to-Unit Voice Channel Request
    const uint8_t   CSBKO_UU_ANS_RSP = 0x05U;               // UU ANS RSP - Unit to Unit Answer Response
    const uint8_t   CSBKO_CTCSBK = 0x07U;                   // CT CSBK - Channel Timing CSBK
    const uint8_t   CSBKO_CALL_ALRT = 0x1FU;                // CALL ALRT - Call Alert
    const uint8_t   CSBKO_ACK_RSP = 0x20U;                  // ACK RSP - Acknowledge Response
    const uint8_t   CSBKO_EXT_FNCT = 0x24U;                 // EXT FNCT - Extended Function
    const uint8_t   CSBKO_NACK_RSP = 0x26U;                 // NACK RSP - Negative Acknowledgement Response
    const uint8_t   CSBKO_BSDWNACT = 0x38U;                 // BS DWN ACT - BS Outbound Activation
    const uint8_t   CSBKO_PRECCSBK = 0x3DU;                 // PRE CSBK - Preamble CSBK

    const uint8_t   TALKER_ID_NONE = 0x00U;
    const uint8_t   TALKER_ID_HEADER = 0x01U;
    const uint8_t   TALKER_ID_BLOCK1 = 0x02U;
    const uint8_t   TALKER_ID_BLOCK2 = 0x04U;
    const uint8_t   TALKER_ID_BLOCK3 = 0x08U;

    const uint32_t  NO_HEADERS_SIMPLEX = 8U;
    const uint32_t  NO_HEADERS_DUPLEX = 3U;
    const uint32_t  NO_PREAMBLE_CSBK = 15U;
} // namespace dmr

// ---------------------------------------------------------------------------
//  Namespace Prototypes
// ---------------------------------------------------------------------------
namespace edac { }
namespace dmr
{
    namespace edac
    {
        using namespace ::edac;
    } // namespace edac
} // namespace dmr

#endif // __DMR_DEFINES_H__
