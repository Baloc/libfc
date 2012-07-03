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

/* Hi Emacs, please use -*- mode: C++; -*- */
/**
 * @file
 * @author Brian Trammell <trammell@tik.ee.ethz.ch>
 *
 * @section DESCRIPTION
 * 
 * Defines the libfc transcoder interface.
 *
 * Transcoders are used for low-level message encoding and decoding, 
 * and wrap a non-owned buffer with an encode and decode interface.
 *
 * Transcoders are only exposed to client code through the SetReceiver
 * interface; see SetReceiver.h and Collector.h for more.
 */

#ifndef IPFIX_XCODER_H // idem
#define IPFIX_XCODER_H // hack

#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <stdint.h>
#include "InfoElement.h"

namespace IPFIX {
  
  /**
   * Represents a variable-length field in a C structure described by a
   * StructTemplate. Content will be copied to message on encode. Pointer
   * will point inside the a libfc-internal set buffer on decode, 
   * and must be copied before the set is processed (i.e., within
   * SetReceiver.receiveSet().
   */
  
  struct VarlenField {
    // length of content
    size_t    len;
    // content pointer
    uint8_t*  cp;
  };
  
  /**
   * A libfc transcoder. Transcoders handle low-level encoding, decoding,
   * and endianness issues. Each transcoder wraps an external buffer,
   * owned by the client.
   *
   * Transcoders are used extensively within libfc's internals (Exporter
   * and Collector). They are only exposed to libfc clients through the
   * SetReceiver interface. See the documentation for SetReceiver.receiveSet()
   * for detailed information.
   *
   * FIXME this class is rather cavalier about copyability and constness.
   *       fix this.
   */
  
  class Transcoder {
    
  private:
    
    static const InfoElement u16ie;
    static const InfoElement u32ie;
    
  public:
    
    Transcoder():
    base_(NULL),
    cur_(NULL),
    check_(NULL),
    max_(NULL),
    savemax_(NULL),
    msg_base_(NULL),
    set_base_(NULL)
    {}
    
    /**
     * Zero the buffer managed by the transcoder
     */
    void zero() { memset(base_, 0, max_ - base_); }
    
    /**
     * Set the buffer used by the transcoder. This buffer must
     * be owned by the client.
     *
     * @param buf base pointer to buffer
     * @param sz size of bugger
     */
    
    void setBase(uint8_t *buf, size_t sz) { 
      base_ = buf; 
      cur_ = buf; 
      max_ = buf + sz;
    }
    
    /**
     * Return the length of content written to or read from
     * the buffer to this point
     */
    size_t len() const { return cur_ - base_; }
    
    /**
     * Return the length of available content to be read from,
     * or the space still available for writing to, the buffer
     */
    size_t avail() const { return max_ - cur_; }
    
    /**
     * Return the base pointer to the buffer used by
     * the transcoder.
     */
    const uint8_t* base() const { return base_; }
    
    /**
     * Return the transcoder cursor
     */
    const uint8_t* cur() const { return cur_; }
    
    /**
     * Checkpoint the buffer. 
     * Saves the cursor for a subsequent call to rollback()
     */
    void checkpoint() { check_ = cur_; }
    
    /**
     * Restore the cursor to its state before the 
     * most recent checkpoint() call.
     */
    void rollback() { if (check_) cur_ = check_; }
    
    /**
     * Focus the transcoder to a subset of the managed buffer.
     * Moves the cursor to a given offset from the base, and sets
     * a temporary maximum.
     *
     * @param off offset from the base() pointer to focus on; 
     *            cursor starts here
     * @param len length of the subset to focus on.
     */
    void focus(size_t off, size_t len);
    
    /**
     * Restore a transcoder to its state before a previous focus() call.
     */  
    void defocus();
    
    /**
     * Skip octets in the transcoder by advancing the cursor.
     *
     * @param len number of octets to skip
     * @return true if len octets are available and were 
     *         skipped, false otherwise.
     */
    bool advance(size_t len) { 
      if (len > avail()) return false; 
      cur_ += len; return true;
    }
    
    /**
     * Skip octets in the transcoder by advancing the cursor.
     * Convenience method advances by the length of the given IE.
     *
     * @param ie pointer to InfoElement representing the field to skip.
     * @return true if ie->len() octets are available and were 
     *         skipped, false otherwise.
     */
    bool advance(const InfoElement *ie) { 
      return advance(ie->len()); 
    } 

    /**
     * Given a pointer to a value of a given length, encode it 
     * to the transcoder at a given offset from the cursor. 
     * Does not advance the cursor.
     *
     * @param val pointer to value to encode
     * @param len length of value to encode
     * @param off offset from the cursor at which to encode the value
     * @param ie pointer to IE representing the field to encode;
     *           the length of the encoded field and type info
     *           is taken from this IE. The type of val is assumed
     *           to be type-compatible with this IE.
     * @return offset from the cursor to the first byte after the 
     *         encoded byte, or 0 if nor enough space available.
     */
    size_t encodeAt(const void* val, size_t len, size_t off, 
                     const InfoElement* ie);

    /**
     * Encode a zero of a given length from the given offset from the cursor.
     * Does not advance the cursor.
     *
     * @param ie pointer to InfoElement representing the field to zero.
     * @return offset from the cursor to the first byte after the encoded
     *         zero, or 0 if not enough space available.
     */
     size_t encodeZeroAt(size_t len, size_t off);

    /**
     * Given a pointer to a value of a given length, encode it 
     * to the transcoder at the cursor. Advances the cursor to
     * the byte after the encoded value. Used for in-order encoding. 
     *
     * @param val pointer to value to encode
     * @param len length of value to encode
     * @param ie pointer to IE representing the field to encode;
     *           the length of the encoded field and type info
     *           is taken from this IE. The type of val is assumed
     *           to be type-compatible with this IE.
     * @return true if the encode succeeded, 
     *         false if not enough space available
     */
    bool encode(const void* val, size_t len, const InfoElement* ie) {
      size_t rv = encodeAt(val, len, 0, ie);
      if (rv) {
        cur_ += rv;
        return true;
      } else {
        return false;
      }
    }
    
    /**
     * Given a pointer to a VarlenField, encode it 
     * to the transcoder at the cursor. Advances the cursor.
     *
     * @param vf pointer to VarlenField to encode
     * @param ie pointer to IE representing the field to encode;
     *           the length of the encoded field and type info
     *           is taken from this IE. The type of vf is assumed
     *           to be type-compatible with this IE.
     * @return true if the encode succeeded, 
     *         false if not enough space available
     */
    bool encode(const VarlenField* vf, const InfoElement *ie) {
      return encode(vf->cp, vf->len, ie);
    }
    
    /**
     * Encode a 16-bit integer at the cursor. Advances the cursor.
     * Used for internal template and header encoding.
     *
     * @param val integer to encode
     * @return true if the encode succeeded, 
     *         false if not enough space available
     */
    bool encode(uint16_t val) {
      return encode(reinterpret_cast<uint8_t *>(&val), sizeof(uint16_t), &u16ie);
    }
    
    /**
     * Encode a 32-bit integer at the cursor. Advances the cursor. 
     * Used for internal template and header encoding.
     *
     * @param val integer to encode
     * @return true if the encode succeeded, 
     *         false if not enough space available
     */
    bool encode(uint32_t val) {
      return encode(reinterpret_cast<uint8_t *>(&val), sizeof(uint32_t), &u32ie);
    }
    
    /**
     * Encode zero for the length of the given IE at the cursor. 
     * Advances the cursor.
     *
     * @param ie pointer to InfoElement representing the field to zero.
     * @return true if the encode succeeded, 
     *         false if not enough space available
     */
    bool encodeZero(const InfoElement* ie);

    /**
     * Open a new message at the cursor. Used internally by Exporter.
     *
     * @return true if the encode succeeded, 
     *         false if not enough space available
     */
    bool encodeMessageStart();

    /**
     * End the currently open message. Used internally by Exporter.
     *
     * @param export_time export time in UNIX epoch seconds
     * @param sequence sequence number of message
     * @param domain observation domain of message
     *
     */    
    void encodeMessageEnd(uint32_t export_time, uint32_t sequence, uint32_t domain);

    /**
     * Open a new set at the cursor. Used internally by Exporter.
     *
     * @param set_id ID of the set to start
     * @return true if the encode succeeded, 
     *         false if not enough space available
     */
    bool encodeSetStart(uint16_t set_id);

    /**
     * End the currently open set. Used internally by Exporter.
     */    
    void encodeSetEnd();
    
    /**
     * Decode the value described by a given information element
     * from a given offset from the cursor into a bounded value buffer.
     * For variable length Information Elements, offset must point
     * to the start of the length encoding. The decoded value is in
     * host byte order but otherwise in IPFIX native encoding for the
     * information element. Does not advance the cursor.
     *
     * @param val pointer to storage for decoded value
     * @param len length of storage for decoded value
     * @param off offset from cursor to value to decode
     * @param ie pointer to IE representing the field to decode;
     *           the length and type of the field to decode
     *           is taken from this IE. The type of *val will be
     *           type-compatible with this IE.
     * @return true if the decode succeeded, 
     *         false if not enough content available
     */
    size_t decodeAt(void* val, size_t len, size_t off, const InfoElement *ie);

    /**
     * Decode the value described by a given information element
     * at the cursor into a bounded value buffer.
     *
     * @param val pointer to storage for decoded value
     * @param len length of storage for decoded value
     * @param ie pointer to IE representing the field to decode;
     *           the length and type of the field to decode
     *           is taken from this IE. The type of *val is assumed
     *           to be type-compatible with this IE.
     * @return true if the decode succeeded, 
     *         false if not enough content available
     */
    bool decode(void* val, size_t len, const InfoElement *ie) {
      size_t rv = decodeAt(val, len, 0, ie);
      if (rv) {
          cur_ += rv;
          return true;
      } else {
          return false;
      }
    }

    
    /**
     * Decode the value described by a given information element
     * at the cursor into an unbounded VarlenField
     * FIXME what is the ownership contract here
     *
     * @param vf pointer to VarlenField to fill
     * @param ie pointer to IE representing the field to decode;
     *           the length and type of the field to decode
     *           is taken from this IE. 
     * @return true if the decode succeeded, 
     *         false if not enough content available
     */

    bool decode(VarlenField *vf, const InfoElement *ie);

    bool decodeSkip(const InfoElement *ie);

    size_t decodeVarlenLengthAt(size_t off, size_t &varlen);

    /**
     * Decode a 16-bit integer at the cursor. Used for internal 
     * template and header decoding.
     *
     * @param val integer (by reference) to decode
     * @return true if the decode succeeded, 
     *         false if not enough content available
     */
    bool decode(uint16_t& val) {
      uint16_t deval;
      if (decode(reinterpret_cast<uint8_t *>(&deval), sizeof(uint16_t), &u16ie)) {
        val = deval;
        return true;
      } else {
        return false;
      }
    }
    
    /**
     * Decode a 32-bit integer at the cursor. Used for internal 
     * template and header decoding.
     *
     * @param val integer (by reference) to decode
     * @return true if the decode succeeded, 
     *         false if not enough content available
     */
    bool decode(uint32_t& val) {
      uint32_t deval;
      if (decode(reinterpret_cast<uint8_t *>(&deval), sizeof(uint32_t), &u32ie)) {
        val = deval;
        return true;
      } else {
        return false;
      }
    }
    
    /**
     * Decode a message header at the cursor, filling in fields by
     * reference.
     *
     * @param len total length of message, returned by reference
     * @param export_time export time in UNIX epoch seconds, returned by reference
     * @param sequence sequence number of message, returned by reference
     * @param domain observation domain of message, returned by reference
     * @return true if the decode succeeded, 
     *         false if not enough content available
     */    
    bool decodeMessageHeader(uint16_t& len, 
                             uint32_t& export_time, 
                             uint32_t& sequence, 
                             uint32_t& domain);

    /**
     * Decode a set header at the cursor, filling in fields by
     * reference.
     *
     * @param sid set ID, returned by reference
     * @param len total length of set, returned by reference
     * @return true if the decode succeeded, 
     *         false if not enough content available
     */    
    bool decodeSetHeader(uint16_t& sid, uint16_t& len);
    
  private:
    
    /* Buffer */
    uint8_t*    base_;    // buffer base pointer
    /* Cursors */
    uint8_t*    cur_;     // current read/write pointer
    uint8_t*    check_;   // checkpoint for rollback
    uint8_t*    max_;     // buffer end pointer
    uint8_t*    savemax_; // saved buffer end pointer for restriction
    /* Higher-level cursors */
    uint8_t*    msg_base_;  // currently open message header
    uint8_t*    set_base_;  // currently open set header
  };
  
} // namespace IPFIX

#endif // idem hack
