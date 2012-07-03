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

#include "IEType.h"

namespace IPFIX {

  /** The number of types known to libfc. This won't change without updating RFC5101 */
  static const size_t kIETypeCount = 20;

  static const unsigned int kOctetArray = 0;
  static const unsigned int kUnsigned8 = 1;
  static const unsigned int kUnsigned16 = 2;
  static const unsigned int kUnsigned32 = 3;
  static const unsigned int kUnsigned64 = 4;
  static const unsigned int kSigned8 = 5;
  static const unsigned int kSigned16 = 6;
  static const unsigned int kSigned32 = 7;
  static const unsigned int kSigned64 = 8;
  static const unsigned int kFloat32 = 9;
  static const unsigned int kFloat64 = 10;
  static const unsigned int kBoolean = 11;
  static const unsigned int kMacAddress = 12;
  static const unsigned int kString = 13;
  static const unsigned int kDateTimeSeconds = 14;
  static const unsigned int kDateTimeMilliseconds = 15;
  static const unsigned int kDateTimeMicroseconds = 16;
  static const unsigned int kDateTimeNanoseconds = 17;
  static const unsigned int kIpv4Address = 18;
  static const unsigned int kIpv6Address = 19;

  size_t IEType::ieTypeCount() {
    return kIETypeCount;
  }

const IEType* IEType::octetArray() { 
  static OctetArrayType iet(std::string("octetArray"), kOctetArray, 0, kVarlen); 
  return &iet; 
}

const IEType* IEType::unsigned8() {
  static UnsignedType iet(std::string("unsigned8"), kUnsigned8, 1, 1); 
  return &iet;
}

const IEType* IEType::unsigned16() { 
  static UnsignedType iet(std::string("unsigned16"), kUnsigned16, 1, 2);
  return &iet;
}

const IEType* IEType::unsigned32() { 
  static UnsignedType iet(std::string("unsigned32"), kUnsigned32, 1, 4);
  return &iet;
}

const IEType* IEType::unsigned64() { 
  static UnsignedType iet(std::string("unsigned64"), kUnsigned64, 1, 8); 
  return &iet; 
}

const IEType* IEType::signed8() { 
  static SignedType iet(std::string("signed8"), kSigned8, 1, 1); 
  return &iet;
}

const IEType* IEType::signed16() { 
  static SignedType iet(std::string("signed16"), kSigned16, 1, 2); 
  return &iet; 
}

const IEType* IEType::signed32() { 
  static SignedType iet(std::string("signed32"), kSigned32, 1, 4); 
  return &iet; 
}

const IEType* IEType::signed64() { 
  static SignedType iet(std::string("signed64"), kSigned64, 1, 8); 
  return &iet; 
}

const IEType* IEType::float32() { 
  static Float32Type iet(std::string("float32"), kFloat32, 4, 4); 
  return &iet; 
}

const IEType* IEType::float64() { 
  static Float64Type iet(std::string("float64"), kFloat64, 4, 8); 
  return &iet; 
}

const IEType* IEType::boolean() { 
  static UnsignedType iet(std::string("boolean"), kBoolean, 1, 8); 
  return &iet; 
}

const IEType* IEType::macAddress() { 
  static OctetArrayType iet(std::string("macAddress"), kMacAddress, 6, 6); 
  return &iet; 
}

const IEType* IEType::string() { 
  static OctetArrayType iet(std::string("string"), kString, 0, kVarlen); 
  return &iet; 
}

const IEType* IEType::dateTimeSeconds() { 
  static UnsignedType iet(std::string("dateTimeSeconds"), kDateTimeSeconds, 4, 4); 
  return &iet; 
}

const IEType* IEType::dateTimeMilliseconds() { 
  static UnsignedType iet(std::string("dateTimeMilliseconds"), kDateTimeMilliseconds, 8, 8); 
  return &iet; 
}

const IEType* IEType::dateTimeMicroseconds() { 
  static MicrosecondsType iet(std::string("dateTimeMicroseconds"), kDateTimeMicroseconds, 8, 8); 
  return &iet;
}

const IEType* IEType::dateTimeNanoseconds() { 
  static MicrosecondsType iet(std::string("dateTimeNanoseconds"), kDateTimeNanoseconds, 8, 8); 
  return &iet; 
}

const IEType* IEType::ipv4Address() { 
  static UnsignedType iet(std::string("ipv4Address"), kIpv4Address, 4, 4); 
  return &iet; 
}

const IEType* IEType::ipv6Address() { 
  static OctetArrayType iet(std::string("ipv6Address"), kIpv6Address, 16, 16); 
  return &iet; 
}

const IEType* IEType::unknown() { 
  static OctetArrayType iet(std::string("unknown"), 0, 0, 0); 
  return &iet; 
}

}
