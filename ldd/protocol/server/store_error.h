
#ifndef __LDD__PROTOCOL_STORE_ERROR_H_
#define __LDD__PROTOCOL_STORE_ERROR_H_

namespace ldd{
namespace protocol{

enum StoreError{
	kOk					=  0,


    // for store
    kStoreFailed                = -501,
    kStoreTimedOut              = -502,
    kStoreCanceled              = -503,

	kInvalidOperation			= -508,

	kInvalidParameter			= -521,
	kInvalidPriorTask			= -522,
	kNotMatchValue				= -523,
	kNotMatchVersion			= -524,

    kPrepareError               = -525,

	// from leveldb
	kNotFound					= -531,		// == level status 1
	kCorruption					= -532,		// == level status 2
	kIOError					= -535,		// == level status 5

	
	// for client
	kNotValidPacket			    = -540,
	kTimeout				    = -541,
	kNoConnection				= -542,


	// 
	kMsgNotFromMaster			= -550,
    kRolesNotReady              = -551,
    kSlaveNotReady              = -552,
    kSynVersionError            = -513,

	kUnrecoveralError			= -1000
};

}//namespace protocol
}//namespace ldd


#endif //__LDD__PROTOCOL_STORE_ERROR_H_
