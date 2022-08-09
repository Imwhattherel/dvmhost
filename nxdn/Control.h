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
*   Copyright (C) 2015-2020 by Jonathan Naylor G4KLX
*   Copyright (C) 2022 by Bryan Biedenkapp N2PLL
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
#if !defined(__NXDN_CONTROL_H__)
#define __NXDN_CONTROL_H__

#include "Defines.h"
#include "nxdn/NXDNDefines.h"
#include "nxdn/channel/LICH.h"
#include "nxdn/lc/LC.h"
#include "nxdn/packet/Voice.h"
#include "nxdn/packet/Data.h"
#include "nxdn/SiteData.h"
#include "network/BaseNetwork.h"
#include "network/RemoteControl.h"
#include "lookups/RSSIInterpolator.h"
#include "lookups/IdenTableLookup.h"
#include "lookups/RadioIdLookup.h"
#include "lookups/TalkgroupIdLookup.h"
#include "modem/Modem.h"
#include "RingBuffer.h"
#include "Timer.h"
#include "yaml/Yaml.h"

#include <cstdio>
#include <string>

namespace nxdn
{
    // ---------------------------------------------------------------------------
    //  Class Prototypes
    // ---------------------------------------------------------------------------
    
    namespace packet { class HOST_SW_API Voice; }
    namespace packet { class HOST_SW_API Data; }

    // ---------------------------------------------------------------------------
    //  Class Declaration
    //      This class implements core logic for handling NXDN.
    // ---------------------------------------------------------------------------

    class Control {
    public:
        /// <summary>Initializes a new instance of the Control class.</summary>
        Control(uint32_t ran, uint32_t callHang, uint32_t queueSize, uint32_t timeout, uint32_t tgHang, 
            modem::Modem* modem, network::BaseNetwork* network, bool duplex, lookups::RadioIdLookup* ridLookup,
            lookups::TalkgroupIdLookup* tidLookup, lookups::IdenTableLookup* idenTable, lookups::RSSIInterpolator* rssiMapper,
            bool debug, bool verbose);
        /// <summary>Finalizes a instance of the Control class.</summary>
        ~Control();

        /// <summary>Resets the data states for the RF interface.</summary>
        void reset();

        /// <summary>Helper to set NXDN configuration options.</summary>
        void setOptions(yaml::Node& conf, const std::string cwCallsign, const std::vector<uint32_t> voiceChNo,
            uint16_t locId, uint8_t channelId, uint32_t channelNo, bool printOptions);

        /// <summary>Process a data frame from the RF interface.</summary>
        bool processFrame(uint8_t* data, uint32_t len);
        /// <summary>Get frame data from data ring buffer.</summary>
        uint32_t getFrame(uint8_t* data);

        /// <summary>Updates the processor by the passed number of milliseconds.</summary>
        void clock(uint32_t ms);

        /// <summary>Flag indicating whether the processor or is busy or not.</summary>
        bool isBusy() const;

        /// <summary>Helper to change the debug and verbose state.</summary>
        void setDebugVerbose(bool debug, bool verbose);

    private:
        friend class packet::Voice;
        packet::Voice* m_voice;
        friend class packet::Data;
        packet::Data* m_data;

        uint32_t m_ran;
        uint32_t m_timeout;

        modem::Modem* m_modem;
        network::BaseNetwork* m_network;

        bool m_duplex;
        bool m_control;
        bool m_dedicatedControl;
        bool m_voiceOnControl;

        channel::LICH m_rfLastLICH;
        lc::LC m_rfLC;
        lc::LC m_netLC;

        uint8_t m_rfMask;
        uint8_t m_netMask;

        lookups::IdenTableLookup* m_idenTable;
        lookups::RadioIdLookup* m_ridLookup;
        lookups::TalkgroupIdLookup* m_tidLookup;

        lookups::IdenTable m_idenEntry;

        RingBuffer<uint8_t> m_queue;

        RPT_RF_STATE m_rfState;
        uint32_t m_rfLastDstId;
        RPT_NET_STATE m_netState;
        uint32_t m_netLastDstId;

        bool m_ccRunning;
        bool m_ccPrevRunning;
        bool m_ccHalted;

        Timer m_rfTimeout;
        Timer m_rfTGHang;
        Timer m_netTimeout;
        Timer m_networkWatchdog;

        SiteData m_siteData;

        lookups::RSSIInterpolator* m_rssiMapper;
        uint8_t m_rssi;
        uint8_t m_maxRSSI;
        uint8_t m_minRSSI;
        uint32_t m_aveRSSI;
        uint32_t m_rssiCount;

        bool m_verbose;
        bool m_debug;

        /// <summary>Add data frame to the data ring buffer.</summary>
        void addFrame(const uint8_t* data, uint32_t length, bool net = false);

        /// <summary>Process a data frames from the network.</summary>
        void processNetwork();

        /// <summary></summary>
        void scrambler(uint8_t* data) const;

        /// <summary>Helper to write RF end of frame data.</summary>
        void writeEndRF();
        /// <summary>Helper to write network end of frame data.</summary>
        void writeEndNet();
    };
} // namespace nxdn

#endif // __NXDN_CONTROL_H__
