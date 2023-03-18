/**
* Digital Voice Modem - Host Software
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Host Software
*
*/
//
// Based on code from the CRUD project. (https://github.com/venediktov/CRUD)
// Licensed under the BPL-1.0 License (https://opensource.org/license/bsl1-0-html)
//
/*
*   Copyright (c) 2003-2013 Christopher M. Kohlhoff
*   Copyright (C) 2023 by Bryan Biedenkapp N2PLL
*
*   Permission is hereby granted, free of charge, to any person or organization 
*   obtaining a copy of the software and accompanying documentation covered by 
*   this license (the “Software”) to use, reproduce, display, distribute, execute, 
*   and transmit the Software, and to prepare derivative works of the Software, and
*   to permit third-parties to whom the Software is furnished to do so, all subject
*   to the following:
*
*   The copyright notices in the Software and this entire statement, including the
*   above license grant, this restriction and the following disclaimer, must be included
*   in all copies of the Software, in whole or in part, and all derivative works of the
*   Software, unless such copies or derivative works are solely in the form of
*   machine-executable object code generated by a source language processor.
*
*   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
*   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
*   PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE
*   DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
*   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
*   OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#if !defined(__REST_HTTP__HTTP_REQUEST_PARSER_H__)
#define __REST_HTTP__HTTP_REQUEST_PARSER_H__

#include <tuple>

namespace rest {
    namespace server {

        // ---------------------------------------------------------------------------
        //  Class Prototypes
        // ---------------------------------------------------------------------------

        struct HTTPRequest;
    
        // ---------------------------------------------------------------------------
        //  Class Declaration
        //      This class implements the lexer for incoming requests.
        // ---------------------------------------------------------------------------

        class HTTPRequestLexer
        {
        public:
            enum ResultType { GOOD, BAD, INDETERMINATE };

            /// <summary>Initializes a new instance of the HTTPRequestLexer class.</summary>
            HTTPRequestLexer();

            /// <summary>Reset to initial parser state.</summary>
            void reset();

            /// <summary>Parse some data. The enum return value is good when a complete request has
            /// been parsed, bad if the data is invalid, indeterminate when more data is
            /// required. The InputIterator return value indicates how much of the input
            /// has been consumed.</summary>
            template <typename InputIterator>
            std::tuple<ResultType, InputIterator> parse(HTTPRequest& req, InputIterator begin, InputIterator end)
            {
                while (begin != end) {
                    ResultType result = consume(req, *begin++);
                    if (result == GOOD || result == BAD)
                        return std::make_tuple(result, begin);
                }
                return std::make_tuple(INDETERMINATE, begin);
            }

        private:
            /// <summary>Handle the next character of input.</summary>
            ResultType consume(HTTPRequest& req, char input);

            /// <summary>Check if a byte is an HTTP character.</summary>
            static bool isChar(int c);
            /// <summary>Check if a byte is an HTTP control character.</summary>
            static bool isControl(int c);
            /// <summary>Check if a byte is an HTTP special character.</summary>
            static bool isSpecial(int c);
            /// <summary>Check if a byte is an digit.</summary>
            static bool isDigit(int c);

            enum state
            {
                METHOD_START,
                METHOD,
                URI,
                HTTP_VERSION_H,
                HTTP_VERSION_T_1,
                HTTP_VERSION_T_2,
                HTTP_VERSION_P,
                HTTP_VERSION_SLASH,
                HTTP_VERSION_MAJOR_START,
                HTTP_VERSION_MAJOR,
                HTTP_VERSION_MINOR_START,
                HTTP_VERSION_MINOR,
                EXPECTING_NEWLINE_1,
                HEADER_LINE_START,
                HEADER_LWS,
                HEADER_NAME,
                SPACE_BEFORE_HEADER_VALUE,
                HEADER_VALUE,
                EXPECTING_NEWLINE_2,
                EXPECTING_NEWLINE_3
            } m_state;
        };
    } // namespace server
} // namespace rest

#endif // __REST_HTTP__HTTP_REQUEST_PARSER_H__
