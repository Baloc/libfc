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
#ifndef IPFIX_DECODER_H
#  define IPFIX_DECODER_H

#  include <list>
#  include <utility>

#  include "InfoElement.h"
#  include "Transcoder.h"

namespace IPFIX {

  class Decoder {
  public:
    /** Adds information elements as they appear on the wire. 
     *
     * Calls to this member function must be made in the order that
     * IEs appear on the wire.  Usually, this function will be called
     * repeatedly by WireTemplate::<some function>
     *
     * @param ie Information Element that appears on the wire.
     */
    void add_src_ie(const InfoElement* ie);

    /** Adds information elements to be decoded.
     *
     * Unlike add_src_ie(), this member function can be called for IEs
     * in any order.
     *
     * @param ie Information Element to be decoded
     * @param p memory where to decode the Information Element
     */
    void add_dst_ie(const InfoElement* ie, void* p);

    void decode_record(Transcoder& xcoder) const;

  private:
    std::list<std::pair<const InfoElement*, void*> > ies;
  };

} // namespace IPFIX

#endif // IPFIX_DECODER_H
