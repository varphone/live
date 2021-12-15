/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "mTunnel" multicast access service
// Copyright (c) 1996-2021 Live Networks, Inc.  All rights reserved.
// Network Addresses
// Implementation

#include "NetAddress.hh"
#include "GroupsockHelper.hh"

#include <stddef.h>
#include <stdio.h>
#if defined(__WIN32__) || defined(_WIN32)
#define USE_GETHOSTBYNAME 1 /*because at least some Windows don't have getaddrinfo()*/
#else
#ifndef INADDR_NONE
#define INADDR_NONE 0xFFFFFFFF
#endif
#endif

////////// NetAddress //////////

NetAddress::NetAddress(u_int8_t const* data, unsigned length) {
  assign(data, length);
}

NetAddress::NetAddress(unsigned length) {
  fData = new u_int8_t[length];
  if (fData == NULL) {
    fLength = 0;
    return;
  }

  for (unsigned i = 0; i < length; ++i)	fData[i] = 0;
  fLength = length;
}

NetAddress::NetAddress(NetAddress const& orig) {
  assign(orig.data(), orig.length());
}

NetAddress& NetAddress::operator=(NetAddress const& rightSide) {
  if (&rightSide != this) {
    clean();
    assign(rightSide.data(), rightSide.length());
  }
  return *this;
}

NetAddress::~NetAddress() {
  clean();
}

void NetAddress::assign(u_int8_t const* data, unsigned length) {
  fData = new u_int8_t[length];
  if (fData == NULL) {
    fLength = 0;
    return;
  }

  for (unsigned i = 0; i < length; ++i)	fData[i] = data[i];
  fLength = length;
}

void NetAddress::clean() {
  delete[] fData; fData = NULL;
  fLength = 0;
}

void copyAddress(struct sockaddr_storage& to, NetAddress const& from) {
  if (from.length() == sizeof (ipv4AddressBits)) {
    to.ss_family = AF_INET;
    ((sockaddr_in&)to).sin_addr.s_addr = *(ipv4AddressBits*)(from.data());
  } else {
    to.ss_family = AF_INET6;
    for (unsigned i = 0; i < 16; ++i) {
      ((sockaddr_in6&)to).sin6_addr.s6_addr[i] = (from.data())[i];
    }
  }
}

Boolean operator==(struct sockaddr_storage const& left, struct sockaddr_storage const& right) {
  if (left.ss_family != right.ss_family) return False;

  switch (left.ss_family) {
    case AF_INET: {
      return ((struct sockaddr_in const&)left).sin_addr.s_addr == ((struct sockaddr_in const&)right).sin_addr.s_addr;
    }
    case AF_INET6: {
      return ((struct sockaddr_in6 const&)left).sin6_addr.s6_addr == ((struct sockaddr_in6 const&)right).sin6_addr.s6_addr;
    }
    default: {
      return False;
    }
  }
}


////////// NetAddressList //////////

NetAddressList::NetAddressList(char const* hostname)
  : fNumAddresses(0), fAddressArray(NULL) {
  if (hostname == NULL) return;

  // First, check whether "hostname" is an IP address string (check IPv4, then IPv6).
  // If so, return a 1-element list with this address:
  ipv4AddressBits addr4;
  if (inet_pton(AF_INET, hostname, (u_int8_t*)&addr4) == 1) {
    fNumAddresses = 1;
    fAddressArray = new NetAddress*[fNumAddresses];
    if (fAddressArray == NULL) return;

    fAddressArray[0] = new NetAddress((u_int8_t*)&addr4, sizeof addr4);
    return;
  }

  ipv6AddressBits addr6;
  if (inet_pton(AF_INET6, hostname, (u_int8_t*)&addr6) == 1) {
    fNumAddresses = 1;
    fAddressArray = new NetAddress*[fNumAddresses];
    if (fAddressArray == NULL) return;

    fAddressArray[0] = new NetAddress((u_int8_t*)&addr6, sizeof addr6);
    return;
  }
    
  // "hostname" is not an IP address string; try resolving it as a real host name instead:
#if defined(USE_GETHOSTBYNAME) || defined(VXWORKS)
  struct hostent* host;
#if defined(VXWORKS)
  char hostentBuf[512];

  host = (struct hostent*)resolvGetHostByName((char*)hostname, (char*)&hostentBuf, sizeof hostentBuf);
#else
  host = gethostbyname((char*)hostname);
#endif
  if (host == NULL || host->h_length != 4 || host->h_addr_list == NULL) return; // no luck

  u_int8_t const** const hAddrPtr = (u_int8_t const**)host->h_addr_list;
  // First, count the number of addresses:
  u_int8_t const** hAddrPtr1 = hAddrPtr;
  while (*hAddrPtr1 != NULL) {
    ++fNumAddresses;
    ++hAddrPtr1;
  }

  // Next, set up the list:
  fAddressArray = new NetAddress*[fNumAddresses];
  if (fAddressArray == NULL) return;

  for (unsigned i = 0; i < fNumAddresses; ++i) {
    fAddressArray[i] = new NetAddress(hAddrPtr[i], host->h_length);
  }
#else
  // Use "getaddrinfo()" (rather than the older, deprecated "gethostbyname()"):
  struct addrinfo addrinfoHints;
  memset(&addrinfoHints, 0, sizeof addrinfoHints);
  addrinfoHints.ai_family = AF_INET; // For now, we're interested in IPv4 addresses only
  struct addrinfo* addrinfoResultPtr = NULL;
  int result = getaddrinfo(hostname, NULL, &addrinfoHints, &addrinfoResultPtr);
  if (result != 0 || addrinfoResultPtr == NULL) return; // no luck

  // First, count the number of addresses:
  const struct addrinfo* p = addrinfoResultPtr;
  while (p != NULL) {
    if (p->ai_addrlen < 4) continue; // sanity check: skip over addresses that are too small
    ++fNumAddresses;
    p = p->ai_next;
  }

  // Next, set up the list:
  fAddressArray = new NetAddress*[fNumAddresses];
  if (fAddressArray == NULL) return;

  unsigned i = 0;
  p = addrinfoResultPtr;
  while (p != NULL) {
    if (p->ai_addrlen < 4) continue;
    fAddressArray[i++] = new NetAddress((u_int8_t const*)&(((struct sockaddr_in*)p->ai_addr)->sin_addr.s_addr), 4);
    p = p->ai_next;
  }

  // Finally, free the data that we had allocated by calling "getaddrinfo()":
  freeaddrinfo(addrinfoResultPtr);
#endif
}

NetAddressList::NetAddressList(NetAddressList const& orig) {
  assign(orig.numAddresses(), orig.fAddressArray);
}

NetAddressList& NetAddressList::operator=(NetAddressList const& rightSide) {
  if (&rightSide != this) {
    clean();
    assign(rightSide.numAddresses(), rightSide.fAddressArray);
  }
  return *this;
}

NetAddressList::~NetAddressList() {
  clean();
}

void NetAddressList::assign(unsigned numAddresses, NetAddress** addressArray) {
  fAddressArray = new NetAddress*[numAddresses];
  if (fAddressArray == NULL) {
    fNumAddresses = 0;
    return;
  }

  for (unsigned i = 0; i < numAddresses; ++i) {
    fAddressArray[i] = new NetAddress(*addressArray[i]);
  }
  fNumAddresses = numAddresses;
}

void NetAddressList::clean() {
  while (fNumAddresses-- > 0) {
    delete fAddressArray[fNumAddresses];
  }
  delete[] fAddressArray; fAddressArray = NULL;
}

NetAddress const* NetAddressList::firstAddress() const {
  if (fNumAddresses == 0) return NULL;

  return fAddressArray[0];
}

////////// NetAddressList::Iterator //////////
NetAddressList::Iterator::Iterator(NetAddressList const& addressList)
  : fAddressList(addressList), fNextIndex(0) {}

NetAddress const* NetAddressList::Iterator::nextAddress() {
  if (fNextIndex >= fAddressList.numAddresses()) return NULL; // no more
  return fAddressList.fAddressArray[fNextIndex++];
}


////////// Port //////////

Port::Port(portNumBits num /* in host byte order */) {
  fPortNum = htons(num);
}

UsageEnvironment& operator<<(UsageEnvironment& s, const Port& p) {
  return s << ntohs(p.num());
}


////////// AddressPortLookupTable //////////

#define NUM_RECORDS_IN_KEY_FOR_EACH_ADDRESS (sizeof (struct in6_addr)/sizeof (int))
#define NUM_RECORDS_IN_KEY_TOTAL (2*NUM_RECORDS_IN_KEY_FOR_EACH_ADDRESS + 1)

AddressPortLookupTable::AddressPortLookupTable()
  : fTable(HashTable::create(NUM_RECORDS_IN_KEY_TOTAL)) {
}

AddressPortLookupTable::~AddressPortLookupTable() {
  delete fTable;
}

static void setKeyFromAddress(int*& key, struct sockaddr_storage const& address) {
  if (address.ss_family == AF_INET) {
    // For an IPv4 address, set the upper 3 words of the key to zero, with the address last:
    *key++ = 0;
    *key++ = 0;
    *key++ = 0;
    *key++ = ((sockaddr_in const&)address).sin_addr.s_addr;
  } else {
    // Assume that the address is IPv6; use all 128 bits (4 words) for the key:
    struct sockaddr_in6 const& address6 = (struct sockaddr_in6&)address;
    u_int8_t const* s6a = address6.sin6_addr.s6_addr; // alias
    *key++ = (s6a[0]<<24)|(s6a[1]<<16)|(s6a[2]<<8)|s6a[3];
    *key++ = (s6a[4]<<24)|(s6a[5]<<16)|(s6a[6]<<8)|s6a[7];
    *key++ = (s6a[8]<<24)|(s6a[9]<<16)|(s6a[10]<<8)|s6a[11];
    *key++ = (s6a[12]<<24)|(s6a[13]<<16)|(s6a[14]<<8)|s6a[15];
  }
}

static void setKey(int* key,
		   struct sockaddr_storage const& address1,
		   struct sockaddr_storage const& address2,
		   Port port) {
  setKeyFromAddress(key, address1);
  setKeyFromAddress(key, address2);
  *key = (int)port.num();
}

void* AddressPortLookupTable::Add(struct sockaddr_storage const& address1,
				  struct sockaddr_storage const& address2,
				  Port port, void* value) {
  int key[NUM_RECORDS_IN_KEY_TOTAL];
  setKey(key, address1, address2, port);
  return fTable->Add((char*)key, value);
}

void* AddressPortLookupTable::Lookup(struct sockaddr_storage const& address1,
				     struct sockaddr_storage const& address2,
				     Port port) {
  int key[NUM_RECORDS_IN_KEY_TOTAL];
  setKey(key, address1, address2, port);
  return fTable->Lookup((char*)key);
}

Boolean AddressPortLookupTable::Remove(struct sockaddr_storage const& address1,
				       struct sockaddr_storage const& address2,
				       Port port) {
  int key[NUM_RECORDS_IN_KEY_TOTAL];
  setKey(key, address1, address2, port);
  return fTable->Remove((char*)key);
}

static struct sockaddr_storage _dummyAddress;
struct sockaddr_storage const& AddressPortLookupTable::dummyAddress() {
  _dummyAddress.ss_family = AF_INET;
  ((sockaddr_in&)_dummyAddress).sin_addr.s_addr = (~0);

  return _dummyAddress;
}

Boolean AddressPortLookupTable::addressIsDummy(sockaddr_storage const& address) {
  return address.ss_family == AF_INET && ((sockaddr_in const&)address).sin_addr.s_addr == (~0);
}

AddressPortLookupTable::Iterator::Iterator(AddressPortLookupTable& table)
  : fIter(HashTable::Iterator::create(*(table.fTable))) {
}

AddressPortLookupTable::Iterator::~Iterator() {
  delete fIter;
}

void* AddressPortLookupTable::Iterator::next() {
  char const* key; // dummy
  return fIter->next(key);
}


////////// isMulticastAddress() implementation //////////

Boolean IsMulticastAddress(netAddressBits address) {
  // Note: We return False for addresses in the range 224.0.0.0
  // through 224.0.0.255, because these are non-routable
  netAddressBits addressInNetworkOrder = htonl(address);
  return addressInNetworkOrder >  0xE00000FF &&
         addressInNetworkOrder <= 0xEFFFFFFF;
}
Boolean IsMulticastAddress(struct sockaddr_storage const& address) {
  switch (address.ss_family) {
    case AF_INET: {
      return IsMulticastAddress(((sockaddr_in const&)address).sin_addr.s_addr);
    }
    case AF_INET6: {
      // An IPv6 address is multicast if the top two bits are 1:
      return (((sockaddr_in6 const&)address).sin6_addr.s6_addr[0] & 0xC0) == 0xC0;
    }
    default: {
      return False;
    }
  }
}


////////// AddressString implementation //////////

AddressString::AddressString(struct sockaddr_in const& addr) {
  init(addr.sin_addr.s_addr);
}
AddressString::AddressString(struct in_addr const& addr) {
  init(addr.s_addr);
}
AddressString::AddressString(ipv4AddressBits const& addr) {
  init(addr);
}

AddressString::AddressString(struct sockaddr_in6 const& addr) {
  init(addr.sin6_addr.s6_addr);
}
AddressString::AddressString(struct in6_addr const& addr) {
  init(addr.s6_addr);
}
AddressString::AddressString(ipv6AddressBits const& addr) {
  init(addr);
}

AddressString::AddressString(struct sockaddr_storage const& addr) {
  switch (addr.ss_family) {
    case AF_INET: {
      init(((sockaddr_in&)addr).sin_addr.s_addr);
      break;
    }
    case AF_INET6: {
      init(((sockaddr_in6&)addr).sin6_addr.s6_addr);
      break;
    }
  }
}

void AddressString::init(ipv4AddressBits const& addr) {
  fVal = new char[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr, fVal, INET_ADDRSTRLEN);
}

void AddressString::init(ipv6AddressBits const& addr) {
  fVal = new char[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &addr, fVal, INET6_ADDRSTRLEN);
}

AddressString::~AddressString() {
  delete[] fVal;
}

portNumBits portNum(struct sockaddr_storage const& addr) {
  switch (addr.ss_family) {
    case AF_INET: {
      return ((sockaddr_in&)addr).sin_port;
    }
    case AF_INET6: {
      return ((sockaddr_in6&)addr).sin6_port;
    }
    default: {
      return 0;
    }
  }
}
