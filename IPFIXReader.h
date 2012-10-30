/* Hi Emacs, please use -*- mode: C++; -*- */
/* Copyright (c) 2011-2012 ETH Zürich. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of ETH Zürich nor the names of other contributors 
 *      may be used to endorse or promote products derived from this software 
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT 
 * HOLDERBE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @author Stephan Neuhaus <neuhaust@tik.ee.ethz.ch>
 */

#ifndef IPFIX_IPFIXREADER_H
#  define IPFIX_IPFIXREADER_H

#  ifdef _IPFIX_HAVE_LOG4CPLUS_
#    include <log4cplus/logger.h>
#  endif /* _IPFIX_HAVE_LOG4CPLUS_ */

#  include "ContentHandler.h"
#  include "ErrorHandler.h"
#  include "InputSource.h"

namespace IPFIX {

  class IPFIXReader {
  public:
    IPFIXReader();

    /** Parses an IPFIX message stream from an input source.
     *
     * The SAX API, on which this API is based, has this as a mere
     * interface (or, in C++ terms, a virtual void parse() = 0), and
     * one has to go through a factory to get a concrete instance of
     * this class. I don't think that this is necessary, but if it's
     * needed, it can be added later.
     *
     * @param is the input source
     */
    virtual void parse(InputSource& is);

    /** Sets an error handler for this parse.
     *
     * @param handler the error handler
     */
    void set_error_handler(ErrorHandler* handler);

    /** Sets a content handler for this parse.
     *
     * @param handler the content handler
     */
    void set_content_handler(ContentHandler* handler);

  protected:
    bool parse_in_progress;
    ContentHandler* content_handler;
    ErrorHandler* error_handler;

  private:

#ifdef _IPFIX_HAVE_LOG4CPLUS_
    log4cplus::Logger logger;
#endif /* _IPFIX_HAVE_LOG4CPLUS_ */
  };

} // namespace IPFIX

#endif // IPFIX_IPFIXREADER_H
