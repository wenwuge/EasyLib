#ifndef __LDD__PROTOCOL_MESSAGE_H_
#define __LDD__PROTOCOL_MESSAGE_H_

#include <cstddef>
#include <string>

namespace ldd{
namespace protocol{


class Message{
public:
	Message();
	virtual ~Message();

	size_t MaxSize() const;
	size_t SerializeTo(void* data);
	bool   ParseFrom(const char* data, size_t size);

protected:
	virtual size_t ByteSize() const = 0;
	virtual void   SerializeToArray(void* data) = 0 ;
	virtual bool   ParseFromArray(const void* data, int size) = 0;
	
private:
	//static const double kCompressRage  =  0.7;  // 0.2 for testing
    static const double kCompressRage  =  0.5;  // 0.2 for testing
	static const size_t kCompressLimit =  128; // 1 for testing
	size_t TryCompress(char* data);

	std::string buffer_;         // todo bug
};


}	//namespace protocol
}	//namespace ldd


#endif //__LDD__PROTOCOL_MESSAGE_H_




