// main.cc (2013-06-28)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

class my_acceptor : public boost::asio::ip::tcp::acceptor {
public:
    explicit my_acceptor(boost::asio::io_service& io_service)
        : boost::asio::ip::tcp::acceptor(io_service)
    {
    }

    template <typename IoControlCommand>
        void io_control(IoControlCommand& command)
        {
            boost::system::error_code ec;
            this->service.io_control(this->implementation, command, ec);
            boost::asio::detail::throw_error(ec);
        }
};

boost::asio::io_service g_ios;
//boost::asio::ip::tcp::acceptor g_acceptor(g_ios);
my_acceptor g_acceptor(g_ios);
boost::asio::ip::tcp::socket g_socket(g_ios);

void HandleAccept(const boost::system::error_code& e)
{
    if (e) {
        LOG(ERROR) << "accept error: errmsg=" << e.message();
        return;
    }

    g_socket.close();

    int count = 0;
    while (1) {
        boost::system::error_code ec;
        g_acceptor.accept(g_socket, ec);
        if (ec) {
            LOG(ERROR) << "accept error, errval=" 
                << ec.value() 
                << ", errmsg=" 
                << ec.message();

            break;
        }

        g_socket.close();

        count++;
        LOG(INFO) << "accept " << count << " socket";
    }

    g_acceptor.async_accept(g_socket
            , boost::bind(&HandleAccept
                , boost::asio::placeholders::error)
            );
}

int main(int argc, char *argv[])
{
    try {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4()
                , 9527);
        g_acceptor.open(endpoint.protocol());
        g_acceptor.set_option(
                boost::asio::ip::tcp::acceptor::reuse_address(true));
        g_acceptor.bind(endpoint);
        g_acceptor.listen();
    }
    catch (boost::system::system_error &e) {
        LOG(ERROR) << "listen error: errmsg=" << e.what();
        return -1;
    }
    catch (...) {
        LOG(FATAL) << "listen faile, unknown exception"; return -1;
    }

    boost::asio::socket_base::non_blocking_io command(true);
    g_acceptor.io_control(command);


    g_acceptor.async_accept(g_socket
            , boost::bind(&HandleAccept
                , boost::asio::placeholders::error));

    g_ios.run();

    getchar();

    return 0;
}

