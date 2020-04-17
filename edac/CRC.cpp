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
*   Copyright (C) 2018 by Bryan Biedenkapp N2PLL
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
#include "Defines.h"
#include "edac/CRC.h"
#include "Utils.h"

using namespace edac;

#include <cstdint>
#include <cstdio>
#include <cassert>
#include <cmath>

// ---------------------------------------------------------------------------
//  Constants
// ---------------------------------------------------------------------------

const uint8_t CRC8_TABLE[] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31,
    0x24, 0x23, 0x2A, 0x2D, 0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D, 0xE0, 0xE7, 0xEE, 0xE9,
    0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1,
    0xB4, 0xB3, 0xBA, 0xBD, 0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA, 0xB7, 0xB0, 0xB9, 0xBE,
    0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0D, 0x0A, 0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A, 0x89, 0x8E, 0x87, 0x80,
    0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8,
    0xDD, 0xDA, 0xD3, 0xD4, 0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44, 0x19, 0x1E, 0x17, 0x10,
    0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F,
    0x6A, 0x6D, 0x64, 0x63, 0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13, 0xAE, 0xA9, 0xA0, 0xA7,
    0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF,
    0xFA, 0xFD, 0xF4, 0xF3, 0x01 };

const uint16_t CRC9_TABLE[] = {
    0x000, 0x259, 0x2B2, 0x0EB, 0x23D, 0x064, 0x08F, 0x2D6,
    0x27A, 0x023, 0x0C8, 0x291, 0x047, 0x21E, 0x2F5, 0x0AC,
    0x0F4, 0x2AD, 0x246, 0x01F, 0x2C9, 0x090, 0x07B, 0x222,
    0x28E, 0x0D7, 0x03C, 0x265, 0x0B3, 0x2EA, 0x201, 0x058,
    0x2B1, 0x0E8, 0x003, 0x25A, 0x08C, 0x2D5, 0x23E, 0x067,
    0x0CB, 0x292, 0x279, 0x020, 0x2F6, 0x0AF, 0x044, 0x21D,
    0x245, 0x01C, 0x0F7, 0x2AE, 0x078, 0x221, 0x2CA, 0x093,
    0x03F, 0x266, 0x28D, 0x0D4, 0x202, 0x05B, 0x0B0, 0x2E9,
    0x03B, 0x262, 0x289, 0x0D0, 0x206, 0x05F, 0x0B4, 0x2ED,
    0x241, 0x018, 0x0F3, 0x2AA, 0x07C, 0x225, 0x2CE, 0x097,
    0x0CF, 0x296, 0x27D, 0x024, 0x2F2, 0x0AB, 0x040, 0x219,
    0x2B5, 0x0EC, 0x007, 0x25E, 0x088, 0x2D1, 0x23A, 0x063,
    0x28A, 0x0D3, 0x038, 0x261, 0x0B7, 0x2EE, 0x205, 0x05C,
    0x0F0, 0x2A9, 0x242, 0x01B, 0x2CD, 0x094, 0x07F, 0x226,
    0x27E, 0x027, 0x0CC, 0x295, 0x043, 0x21A, 0x2F1, 0x0A8,
    0x004, 0x25D, 0x2B6, 0x0EF, 0x239, 0x060, 0x08B, 0x2D2,
    0x276, 0x02F, 0x0C4, 0x29D, 0x04B, 0x212, 0x2F9, 0x0A0,
    0x00C, 0x255, 0x2BE, 0x0E7, 0x231, 0x068, 0x083, 0x2DA,
    0x282, 0x0DB, 0x030, 0x269, 0x0BF, 0x2E6, 0x20D, 0x054,
    0x0F8, 0x2A1, 0x24A, 0x013, 0x2C5, 0x09C, 0x077, 0x22E,
    0x0C7, 0x29E, 0x275, 0x02C, 0x2FA, 0x0A3, 0x048, 0x211,
    0x2BD, 0x0E4, 0x00F, 0x256, 0x080, 0x2D9, 0x232, 0x06B,
    0x033, 0x26A, 0x281, 0x0D8, 0x20E, 0x057, 0x0BC, 0x2E5,
    0x249, 0x010, 0x0FB, 0x2A2, 0x074, 0x22D, 0x2C6, 0x09F,
    0x24D, 0x014, 0x0FF, 0x2A6, 0x070, 0x229, 0x2C2, 0x09B,
    0x037, 0x26E, 0x285, 0x0DC, 0x20A, 0x053, 0x0B8, 0x2E1,
    0x2B9, 0x0E0, 0x00B, 0x252, 0x084, 0x2DD, 0x236, 0x06F,
    0x0C3, 0x29A, 0x271, 0x028, 0x2FE, 0x0A7, 0x04C, 0x215,
    0x0FC, 0x2A5, 0x24E, 0x017, 0x2C1, 0x098, 0x073, 0x22A,
    0x286, 0x0DF, 0x034, 0x26D, 0x0BB, 0x2E2, 0x209, 0x050,
    0x008, 0x251, 0x2BA, 0x0E3, 0x235, 0x06C, 0x087, 0x2DE,
    0x272, 0x02B, 0x0C0, 0x299, 0x04F, 0x216, 0x2FD, 0x0A4 };

const uint16_t CCITT16_TABLE1[] = {
    0x0000U, 0x1189U, 0x2312U, 0x329bU, 0x4624U, 0x57adU, 0x6536U, 0x74bfU,
    0x8c48U, 0x9dc1U, 0xaf5aU, 0xbed3U, 0xca6cU, 0xdbe5U, 0xe97eU, 0xf8f7U,
    0x1081U, 0x0108U, 0x3393U, 0x221aU, 0x56a5U, 0x472cU, 0x75b7U, 0x643eU,
    0x9cc9U, 0x8d40U, 0xbfdbU, 0xae52U, 0xdaedU, 0xcb64U, 0xf9ffU, 0xe876U,
    0x2102U, 0x308bU, 0x0210U, 0x1399U, 0x6726U, 0x76afU, 0x4434U, 0x55bdU,
    0xad4aU, 0xbcc3U, 0x8e58U, 0x9fd1U, 0xeb6eU, 0xfae7U, 0xc87cU, 0xd9f5U,
    0x3183U, 0x200aU, 0x1291U, 0x0318U, 0x77a7U, 0x662eU, 0x54b5U, 0x453cU,
    0xbdcbU, 0xac42U, 0x9ed9U, 0x8f50U, 0xfbefU, 0xea66U, 0xd8fdU, 0xc974U,
    0x4204U, 0x538dU, 0x6116U, 0x709fU, 0x0420U, 0x15a9U, 0x2732U, 0x36bbU,
    0xce4cU, 0xdfc5U, 0xed5eU, 0xfcd7U, 0x8868U, 0x99e1U, 0xab7aU, 0xbaf3U,
    0x5285U, 0x430cU, 0x7197U, 0x601eU, 0x14a1U, 0x0528U, 0x37b3U, 0x263aU,
    0xdecdU, 0xcf44U, 0xfddfU, 0xec56U, 0x98e9U, 0x8960U, 0xbbfbU, 0xaa72U,
    0x6306U, 0x728fU, 0x4014U, 0x519dU, 0x2522U, 0x34abU, 0x0630U, 0x17b9U,
    0xef4eU, 0xfec7U, 0xcc5cU, 0xddd5U, 0xa96aU, 0xb8e3U, 0x8a78U, 0x9bf1U,
    0x7387U, 0x620eU, 0x5095U, 0x411cU, 0x35a3U, 0x242aU, 0x16b1U, 0x0738U,
    0xffcfU, 0xee46U, 0xdcddU, 0xcd54U, 0xb9ebU, 0xa862U, 0x9af9U, 0x8b70U,
    0x8408U, 0x9581U, 0xa71aU, 0xb693U, 0xc22cU, 0xd3a5U, 0xe13eU, 0xf0b7U,
    0x0840U, 0x19c9U, 0x2b52U, 0x3adbU, 0x4e64U, 0x5fedU, 0x6d76U, 0x7cffU,
    0x9489U, 0x8500U, 0xb79bU, 0xa612U, 0xd2adU, 0xc324U, 0xf1bfU, 0xe036U,
    0x18c1U, 0x0948U, 0x3bd3U, 0x2a5aU, 0x5ee5U, 0x4f6cU, 0x7df7U, 0x6c7eU,
    0xa50aU, 0xb483U, 0x8618U, 0x9791U, 0xe32eU, 0xf2a7U, 0xc03cU, 0xd1b5U,
    0x2942U, 0x38cbU, 0x0a50U, 0x1bd9U, 0x6f66U, 0x7eefU, 0x4c74U, 0x5dfdU,
    0xb58bU, 0xa402U, 0x9699U, 0x8710U, 0xf3afU, 0xe226U, 0xd0bdU, 0xc134U,
    0x39c3U, 0x284aU, 0x1ad1U, 0x0b58U, 0x7fe7U, 0x6e6eU, 0x5cf5U, 0x4d7cU,
    0xc60cU, 0xd785U, 0xe51eU, 0xf497U, 0x8028U, 0x91a1U, 0xa33aU, 0xb2b3U,
    0x4a44U, 0x5bcdU, 0x6956U, 0x78dfU, 0x0c60U, 0x1de9U, 0x2f72U, 0x3efbU,
    0xd68dU, 0xc704U, 0xf59fU, 0xe416U, 0x90a9U, 0x8120U, 0xb3bbU, 0xa232U,
    0x5ac5U, 0x4b4cU, 0x79d7U, 0x685eU, 0x1ce1U, 0x0d68U, 0x3ff3U, 0x2e7aU,
    0xe70eU, 0xf687U, 0xc41cU, 0xd595U, 0xa12aU, 0xb0a3U, 0x8238U, 0x93b1U,
    0x6b46U, 0x7acfU, 0x4854U, 0x59ddU, 0x2d62U, 0x3cebU, 0x0e70U, 0x1ff9U,
    0xf78fU, 0xe606U, 0xd49dU, 0xc514U, 0xb1abU, 0xa022U, 0x92b9U, 0x8330U,
    0x7bc7U, 0x6a4eU, 0x58d5U, 0x495cU, 0x3de3U, 0x2c6aU, 0x1ef1U, 0x0f78U };

const uint16_t CCITT16_TABLE2[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0 };

const uint32_t CRC32_TABLE[] = {
    0x00000000, 0x04C11DB5, 0x09823B6A, 0x0D4326DF, 0x130476D4, 0x17C56B61, 0x1A864DBE, 0x1E47500B,
    0x2608EDA8, 0x22C9F01D, 0x2F8AD6C2, 0x2B4BCB77, 0x350C9B7C, 0x31CD86C9, 0x3C8EA016, 0x384FBDA3,
    0x4C11DB50, 0x48D0C6E5, 0x4593E03A, 0x4152FD8F, 0x5F15AD84, 0x5BD4B031, 0x569796EE, 0x52568B5B,
    0x6A1936F8, 0x6ED82B4D, 0x639B0D92, 0x675A1027, 0x791D402C, 0x7DDC5D99, 0x709F7B46, 0x745E66F3,
    0x9823B6A0, 0x9CE2AB15, 0x91A18DCA, 0x9560907F, 0x8B27C074, 0x8FE6DDC1, 0x82A5FB1E, 0x8664E6AB,
    0xBE2B5B08, 0xBAEA46BD, 0xB7A96062, 0xB3687DD7, 0xAD2F2DDC, 0xA9EE3069, 0xA4AD16B6, 0xA06C0B03,
    0xD4326DF0, 0xD0F37045, 0xDDB0569A, 0xD9714B2F, 0xC7361B24, 0xC3F70691, 0xCEB4204E, 0xCA753DFB,
    0xF23A8058, 0xF6FB9DED, 0xFBB8BB32, 0xFF79A687, 0xE13EF68C, 0xE5FFEB39, 0xE8BCCDE6, 0xEC7DD053,
    0x348670F5, 0x30476D40, 0x3D044B9F, 0x39C5562A, 0x27820621, 0x23431B94, 0x2E003D4B, 0x2AC120FE,
    0x128E9D5D, 0x164F80E8, 0x1B0CA637, 0x1FCDBB82, 0x018AEB89, 0x054BF63C, 0x0808D0E3, 0x0CC9CD56,
    0x7897ABA5, 0x7C56B610, 0x711590CF, 0x75D48D7A, 0x6B93DD71, 0x6F52C0C4, 0x6211E61B, 0x66D0FBAE,
    0x5E9F460D, 0x5A5E5BB8, 0x571D7D67, 0x53DC60D2, 0x4D9B30D9, 0x495A2D6C, 0x44190BB3, 0x40D81606,
    0xACA5C655, 0xA864DBE0, 0xA527FD3F, 0xA1E6E08A, 0xBFA1B081, 0xBB60AD34, 0xB6238BEB, 0xB2E2965E,
    0x8AAD2BFD, 0x8E6C3648, 0x832F1097, 0x87EE0D22, 0x99A95D29, 0x9D68409C, 0x902B6643, 0x94EA7BF6,
    0xE0B41D05, 0xE47500B0, 0xE936266F, 0xEDF73BDA, 0xF3B06BD1, 0xF7717664, 0xFA3250BB, 0xFEF34D0E,
    0xC6BCF0AD, 0xC27DED18, 0xCF3ECBC7, 0xCBFFD672, 0xD5B88679, 0xD1799BCC, 0xDC3ABD13, 0xD8FBA0A6,
    0x690CE1EA, 0x6DCDFC5F, 0x608EDA80, 0x644FC735, 0x7A08973E, 0x7EC98A8B, 0x738AAC54, 0x774BB1E1,
    0x4F040C42, 0x4BC511F7, 0x46863728, 0x42472A9D, 0x5C007A96, 0x58C16723, 0x558241FC, 0x51435C49,
    0x251D3ABA, 0x21DC270F, 0x2C9F01D0, 0x285E1C65, 0x36194C6E, 0x32D851DB, 0x3F9B7704, 0x3B5A6AB1,
    0x0315D712, 0x07D4CAA7, 0x0A97EC78, 0x0E56F1CD, 0x1011A1C6, 0x14D0BC73, 0x19939AAC, 0x1D528719,
    0xF12F574A, 0xF5EE4AFF, 0xF8AD6C20, 0xFC6C7195, 0xE22B219E, 0xE6EA3C2B, 0xEBA91AF4, 0xEF680741,
    0xD727BAE2, 0xD3E6A757, 0xDEA58188, 0xDA649C3D, 0xC423CC36, 0xC0E2D183, 0xCDA1F75C, 0xC960EAE9,
    0xBD3E8C1A, 0xB9FF91AF, 0xB4BCB770, 0xB07DAAC5, 0xAE3AFACE, 0xAAFBE77B, 0xA7B8C1A4, 0xA379DC11,
    0x9B3661B2, 0x9FF77C07, 0x92B45AD8, 0x9675476D, 0x88321766, 0x8CF30AD3, 0x81B02C0C, 0x857131B9,
    0x5D8A911F, 0x594B8CAA, 0x5408AA75, 0x50C9B7C0, 0x4E8EE7CB, 0x4A4FFA7E, 0x470CDCA1, 0x43CDC114,
    0x7B827CB7, 0x7F436102, 0x720047DD, 0x76C15A68, 0x68860A63, 0x6C4717D6, 0x61043109, 0x65C52CBC,
    0x119B4A4F, 0x155A57FA, 0x18197125, 0x1CD86C90, 0x029F3C9B, 0x065E212E, 0x0B1D07F1, 0x0FDC1A44,
    0x3793A7E7, 0x3352BA52, 0x3E119C8D, 0x3AD08138, 0x2497D133, 0x2056CC86, 0x2D15EA59, 0x29D4F7EC,
    0xC5A927BF, 0xC1683A0A, 0xCC2B1CD5, 0xC8EA0160, 0xD6AD516B, 0xD26C4CDE, 0xDF2F6A01, 0xDBEE77B4,
    0xE3A1CA17, 0xE760D7A2, 0xEA23F17D, 0xEEE2ECC8, 0xF0A5BCC3, 0xF464A176, 0xF92787A9, 0xFDE69A1C,
    0x89B8FCEF, 0x8D79E15A, 0x803AC785, 0x84FBDA30, 0x9ABC8A3B, 0x9E7D978E, 0x933EB151, 0x97FFACE4,
    0xAFB01147, 0xAB710CF2, 0xA6322A2D, 0xA2F33798, 0xBCB46793, 0xB8757A26, 0xB5365CF9, 0xB1F7414C };

// ---------------------------------------------------------------------------
//  Static Class Members
// ---------------------------------------------------------------------------
/// <summary>
/// Check 5-bit CRC.
/// </summary>
/// <param name="in">Boolean bit array.</param>
/// <param name="tcrc">Computed CRC to check.</param>
/// <returns>True, if CRC is valid, otherwise false.</returns>
bool CRC::checkFiveBit(bool* in, uint32_t tcrc)
{
    assert(in != NULL);

    uint32_t crc;
    encodeFiveBit(in, crc);

    return crc == tcrc;
}

/// <summary>
/// Encode 5-bit CRC.
/// </summary>
/// <param name="in">Boolean bit array.</param>
/// <param name="tcrc">Computed CRC.</param>
void CRC::encodeFiveBit(const bool* in, uint32_t& tcrc)
{
    assert(in != NULL);

    unsigned short total = 0U;
    for (uint32_t i = 0U; i < 72U; i += 8U) {
        uint8_t c;
        Utils::bitsToByteBE(in + i, c);
        total += c;
    }

    total %= 31U;

    tcrc = total;
}

/// <summary>
/// Check 16-bit CRC-CCITT.
/// </summary>
/// <remarks>This uses polynomial 0x1021.</remarks>
/// <param name="in">Input byte array.</param>
/// <param name="length">Length of byte array.</param>
/// <returns>True, if CRC is valid, otherwise false.</returns>
bool CRC::checkCCITT162(const uint8_t *in, uint32_t length)
{
    assert(in != NULL);
    assert(length > 2U);

    union {
        uint16_t crc16;
        uint8_t  crc8[2U];
    };

    crc16 = 0U;

    for (unsigned i = 0U; i < (length - 2U); i++)
        crc16 = (uint16_t(crc8[0U]) << 8) ^ CCITT16_TABLE2[crc8[1U] ^ in[i]];

    crc16 = ~crc16;

    return crc8[0U] == in[length - 1U] && crc8[1U] == in[length - 2U];
}

/// <summary>
/// Encode 16-bit CRC-CCITT.
/// </summary>
/// <remarks>This uses polynomial 0x1021.</remarks>
/// <param name="in">Input byte array.</param>
/// <param name="length">Length of byte array.</param>
void CRC::addCCITT162(uint8_t* in, uint32_t length)
{
    assert(in != NULL);
    assert(length > 2U);

    union {
        uint16_t crc16;
        uint8_t  crc8[2U];
    };

    crc16 = 0U;

    for (unsigned i = 0U; i < (length - 2U); i++)
        crc16 = (uint16_t(crc8[0U]) << 8) ^ CCITT16_TABLE2[crc8[1U] ^ in[i]];

    crc16 = ~crc16;

    in[length - 1U] = crc8[0U];
    in[length - 2U] = crc8[1U];
}

/// <summary>
/// Check 16-bit CRC-CCITT.
/// </summary>
/// <remarks>This uses polynomial 0x1189.</remarks>
/// <param name="in">Input byte array.</param>
/// <param name="length">Length of byte array.</param>
/// <returns>True, if CRC is valid, otherwise false.</returns>
bool CRC::checkCCITT161(const uint8_t *in, uint32_t length)
{
    assert(in != NULL);
    assert(length > 2U);

    union {
        uint16_t crc16;
        uint8_t  crc8[2U];
    };

    crc16 = 0xFFFFU;

    for (uint32_t i = 0U; i < (length - 2U); i++)
        crc16 = uint16_t(crc8[1U]) ^ CCITT16_TABLE1[crc8[0U] ^ in[i]];

    crc16 = ~crc16;

    return crc8[0U] == in[length - 2U] && crc8[1U] == in[length - 1U];
}

/// <summary>
/// Encode 16-bit CRC-CCITT.
/// </summary>
/// <remarks>This uses polynomial 0x1189.</remarks>
/// <param name="in">Input byte array.</param>
/// <param name="length">Length of byte array.</param>
void CRC::addCCITT161(uint8_t* in, uint32_t length)
{
    assert(in != NULL);
    assert(length > 2U);

    union {
        uint16_t crc16;
        uint8_t  crc8[2U];
    };

    crc16 = 0xFFFFU;

    for (uint32_t i = 0U; i < (length - 2U); i++)
        crc16 = uint16_t(crc8[1U]) ^ CCITT16_TABLE1[crc8[0U] ^ in[i]];

    crc16 = ~crc16;

    in[length - 2U] = crc8[0U];
    in[length - 1U] = crc8[1U];
}

/// <summary>
/// Check 32-bit CRC.
/// </summary>
/// <param name="in">Input byte array.</param>
/// <param name="length">Length of byte array.</param>
/// <returns>True, if CRC is valid, otherwise false.</returns>
bool CRC::checkCRC32(const uint8_t *in, uint32_t length)
{
    assert(in != NULL);
    assert(length > 4U);

    union {
        uint32_t crc32;
        uint8_t  crc8[4U];
    };

    crc32 = 0xFFFFFFFFU;

    for (uint32_t i = 0U; i < (length - 4U); i++)
        crc32 = (crc32 << 8) ^ CRC32_TABLE[((crc32 >> 24) ^ in[i]) & 0xFF];

    crc32 = ~crc32;

    return crc8[0U] == in[length - 4U] && crc8[1U] == in[length - 3U] && crc8[2U] == in[length - 2U] && crc8[3U] == in[length - 1U];
}

/// <summary>
/// Encode 32-bit CRC.
/// </summary>
/// <param name="in">Input byte array.</param>
/// <param name="length">Length of byte array.</param>
void CRC::addCRC32(uint8_t* in, uint32_t length)
{
    assert(in != NULL);
    assert(length > 4U);

    union {
        uint32_t crc32;
        uint8_t  crc8[4U];
    };

    crc32 = 0xFFFFFFFFU;

    for (uint32_t i = 0U; i < (length - 4U); i++)
        crc32 = (crc32 << 8) ^ CRC32_TABLE[((crc32 >> 24) ^ in[i]) & 0xFF];

    crc32 = ~crc32;

    in[length - 4U] = crc8[0U];
    in[length - 3U] = crc8[1U];
    in[length - 2U] = crc8[2U];
    in[length - 1U] = crc8[3U];
}

/// <summary>
/// Generate 8-bit CRC.
/// </summary>
/// <param name="in">Input byte array.</param>
/// <param name="length">Length of byte array.</param>
/// <returns>Calculated 8-bit CRC value.</returns>
uint8_t CRC::crc8(const uint8_t *in, uint32_t length)
{
    assert(in != NULL);

    uint8_t crc = 0U;

    for (uint32_t i = 0U; i < length; i++)
        crc = CRC8_TABLE[crc ^ in[i]];

    return crc;
}

/// <summary>
/// Generate 9-bit CRC.
/// </summary>
/// <param name="in">Input byte array.</param>
/// <param name="length">Length of byte array.</param>
/// <returns>Calculated 9-bit CRC value.</returns>
uint16_t CRC::crc9(const uint8_t* in, uint32_t length)
{
    assert(in != NULL);

    uint16_t crc = 0U;

    for (uint32_t i = 0U; i < length; i++)
        crc = CRC9_TABLE[(crc ^ in[i]) & 0xFF];

    return crc;
}
