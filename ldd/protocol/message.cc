
#include <snappy.h>
#include <glog/logging.h>
#include "coded_stream.h"
#include "message.h"




namespace ldd{
namespace protocol{

Message::Message(){

}
Message::~Message(){

}

size_t Message::MaxSize() const {
	size_t n = ByteSize();
	size_t m = snappy::MaxCompressedLength(n);
	
	return m + 1;
}
size_t Message::SerializeTo(void* data) {
	char *cur = static_cast<char*>(data);
	size_t n = ByteSize();
	if (n <= kCompressLimit) {
		cur[0] = 0;
		SerializeToArray(cur + 1);
		DLOG(INFO)<<" Message::SerializeTo() not compressed, original len="
                    <<n<<" kCompressLimit="<<kCompressLimit;
		return n + 1;
	} else {
		buffer_.resize(n);
		SerializeToArray(&buffer_[0]);
		DLOG(INFO)<<" Message::SerializeTo() compressed, original="
                    <<n<<" kCompressLimit="<<kCompressLimit;
		return TryCompress(cur);
	}
}

size_t Message::TryCompress(char* data) {
	size_t len = 0;
	snappy::RawCompress(buffer_.c_str(), buffer_.size(), data + 1, &len);
	double x = double(len) / buffer_.size();
	if (x < kCompressRage) {
		data[0] = 1;
		DLOG(INFO)<<" Message::TryCompress() compressed, compress ratio="
            <<x<<" compressed len="<<len;
		return len + 1;
	} else {
		data[0] = 0;
		memcpy(data + 1, buffer_.c_str(), buffer_.size());
		DLOG(INFO)<<" Message::TryCompress() not compressed, test compressed len="
			<<len<<" compress ratio="<<x;
		return buffer_.size() + 1;
	}
}

bool Message::ParseFrom(const char* data, size_t size) {
	bool compressed = data[0];
	const char* ptr = data + 1;
	size_t length = size - 1;
	if (compressed) {
		size_t len=0;
		if (!snappy::GetUncompressedLength(data+1, size-1, &len)) {
			LOG(ERROR)<<" error Message::ParseFrom() GetUncompressedLength, size="<<size<<" len="<<len;
			return false;
		}
		buffer_.resize(len);
		if (!snappy::RawUncompress(data+1, size-1, &buffer_[0])) {
			LOG(ERROR)<<" error Message::ParseFrom() RawUncompress, size="<<size<<" len="<<len;
			return false;
		}
		ptr = buffer_.data();
		length = buffer_.size();
        DLOG(INFO)<<"Message::ParseFrom(), compressed len="<<size
                    <<" original len="<<len;
    }else{
        DLOG(INFO)<<"Message::ParseFrom(), not compressed, original len="<<size;
    }
	return ParseFromArray(ptr, length);
}

}//namespace protocol
}//namespace ldd


