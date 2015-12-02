#ifndef LDD_PROTOCOL_COMMON_H
#define LDD_PROTOCOL_COMMON_H


namespace ldd{
namespace protocol{


/************************************************************************/
/* message from client to proxy											*/
/************************************************************************/
#define LDD_CLIENT_2_PROXY_GET				0x9910
#define LDD_CLIENT_2_PROXY_CHECK			0x9920
#define LDD_CLIENT_2_PROXY_MUTATE			0x9930
#define LDD_CLIENT_2_PROXY_LIST				0x9940
#define LDD_CLIENT_2_PROXY_ATOMIC_INCR		0x9950
#define LDD_CLIENT_2_PROXY_ATOMIC_APPEND	0x9960


}//namespace protocol
}//namespace ldd


#endif  // LDD_PROTOCOL_COMMON_H
