
#ifndef __LDD__PROTOCOL_COMMAND_TYPE_H_
#define __LDD__PROTOCOL_COMMAND_TYPE_H_

namespace ldd{
namespace protocol{

/************************************************************************/
/* message from proxy to store											*/
/************************************************************************/
#define LDD_PROXY_2_STORE_GET				0x7710
#define LDD_PROXY_2_STORE_CHECK				0x7720
#define LDD_PROXY_2_STORE_MUTATE			0x7730
#define LDD_PROXY_2_STORE_LIST				0x7740
#define LDD_PROXY_2_STORE_ATOMIC_INCR		0x7750
#define LDD_PROXY_2_STORE_ATOMIC_APPEND		0x7760

/************************************************************************/
/* message from store to store											*/
/************************************************************************/
#define LDD_STORE_2_STORE_GET_VERSION		0x5510
#define LDD_STORE_2_STORE_MUTATION_PREPARE	0x5520


}//namespace protocol
}//namespace ldd


#endif //__LDD__PROTOCOL_COMMAND_TYPE_H_
