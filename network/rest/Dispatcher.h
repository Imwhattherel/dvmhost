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
#if !defined(__REST__DISPATHER_H__)
#define __REST__DISPATHER_H__

#include "Defines.h" 
 
#include <functional>
#include <map>
#include <string>
#include <regex>
#include <memory>
 
namespace rest {
    // ---------------------------------------------------------------------------
    //  Structure Declaration
    //      
    // ---------------------------------------------------------------------------

    template<typename Matched>
    struct RequestMatch : Matched 
    {
        /// <summary>Initializes a new instance of the RequestMatch structure.</summary>
        RequestMatch(const Matched& m, const std::string& d) : Matched(m) , data(d) { /* stub */ }
        
        std::string data;
    };

    // ---------------------------------------------------------------------------
    //  Structure Declaration
    //      
    // ---------------------------------------------------------------------------
    
    template<typename Response, typename Regex, typename Matched>
    struct RequestMatcher {
        typedef std::function<void(Response&, const RequestMatch<Matched>&)> RequestHandlerType;
        typedef RequestMatcher<Response, Regex, Matched> selfType;

        /// <summary>Initializes a new instance of the RequestMatch structure.</summary>
        explicit RequestMatcher(const Regex& expression) : m_expression(expression) { /* stub */ }

        /// <summary></summary>
        selfType& get(RequestHandlerType handler) {
            m_handlers["GET"] = handler;
            return *this;
        }
        /// <summary></summary>
        selfType& post(RequestHandlerType handler) {
            m_handlers["POST"] = handler;
            return *this;
        }
        /// <summary></summary>
        selfType& put(RequestHandlerType handler) {
            m_handlers["PUT"] = handler;
            return *this;
        }
        /// <summary></summary>
        selfType& del(RequestHandlerType handler) {
            m_handlers["DELETE"] = handler;
            return *this;
        }
        /// <summary></summary>
        selfType& options(RequestHandlerType handler) {
            m_handlers["OPTIONS"] = handler;
            return *this;
        }

        /// <summary></summary>
        template<typename Request>
        void handleRequest(const Request& request, Response& response, const Matched &what) {
            // dispatching to matching based on handler
            RequestMatch<Matched> match(what, request.data);
            auto& handler = m_handlers[request.method];
            if (handler) {
                handler(response, match);
            }
        }
    
    private:
       Regex m_expression;
       std::map<std::string, RequestHandlerType> m_handlers;
    };

    // ---------------------------------------------------------------------------
    //  Class Declaration
    //      This class implements RESTful web request dispatching.
    // ---------------------------------------------------------------------------

    template<typename Request, typename Response, typename Match = std::cmatch, typename Expression = std::regex>
    class RequestDispatcher {
        typedef RequestMatcher<Response, Expression, Match> MatcherType;
        typedef std::shared_ptr<MatcherType> MatcherTypePtr;
    public:
        /// <summary>Initializes a new instance of the RequestDispatcher class.</summary>
        RequestDispatcher() : m_basePath() { /* stub */ }
        /// <summary>Initializes a new instance of the RequestDispatcher class.</summary>
        RequestDispatcher(const std::string &basePath) : m_basePath(basePath) { /* stub */ }

        /// <summary></summary>
        MatcherType& match(const Expression& expression) 
        {
            MatcherTypePtr& p = m_matchers[expression];
            if(!p) {
                p = std::make_shared<crud_matcher_type>(expression);
            }

            return *p;
        }

        /// <summary></summary>
        template<typename E = Expression>
        typename std::enable_if<!std::is_same<E, std::string>::value, void>::type
        handleRequest(const Request& request, Response& response) 
        {
            for (const auto& matcher : m_matchers) {
                Match what;
                if (std::regex_match(request.uri.c_str(), what, matcher.first)) {
                    matcher.second->handle_request(request, response, what);
                }
            }
        }

        /// <summary></summary>
        template<typename E = Expression>
        typename std::enable_if<std::is_same<E, std::string>::value, void>::type
        handleRequest(const Request& request, Response& response) 
        {
            for ( const auto& matcher : m_matchers) {
                Match what;
                if (request.uri.find(matcher.first) != std::string::npos) {
                    what = matcher.first;
                    matcher.second->handle_request(request, response, what);
                }
            }
        }
    
    private:
        std::string m_basePath;
        std::map<Expression, MatcherTypePtr> m_matchers;
    };
} // namespace rest
  
#endif // __REST__DISPATHER_H__ 
