#include <netdb.h>
#include <strings.h>
#include <iostream>

#include <dser/socket_utils.h>
#include <sys/socket.h>

namespace dser
{

    address_info::~address_info()
    {
        ::freeaddrinfo(this->_ai);
    }

    int address_info::get_error()
    {
        return this->_error;
    }
    
    ::addrinfo* address_info::get_info(const char* node, const char* service)
    {
        ::addrinfo hints;
        ::bzero(&hints, sizeof(hints));
        hints.ai_flags |= AI_CANONNAME | AI_ALL;

        this->_error = ::getaddrinfo(node, service, &hints, &this->_ai);
        return this->_error ? nullptr : this->_ai;
    }

    static void print_flags(int flags)
    {
        std::cout << "Flags:" << std::endl;
#define PRINT_FLAG_EXACT_IF_PRESENT(flag_name) if (flags & flag_name) std::cout << "    " << #flag_name << std::endl
        PRINT_FLAG_EXACT_IF_PRESENT(AI_PASSIVE);
        PRINT_FLAG_EXACT_IF_PRESENT(AI_CANONNAME);
        PRINT_FLAG_EXACT_IF_PRESENT(AI_NUMERICHOST);
        PRINT_FLAG_EXACT_IF_PRESENT(AI_V4MAPPED);
        PRINT_FLAG_EXACT_IF_PRESENT(AI_ALL);
        PRINT_FLAG_EXACT_IF_PRESENT(AI_ADDRCONFIG);
#undef PRINT_FLAG_EXACT_IF_PRESENT
    }

    static void print_family(int family)
    {
        std::cout << "Family:" << std::endl;
        if (family & AF_UNSPEC) std::cout << "    " << "AF_UNSPEC" << std::endl;
        if (family & AF_LOCAL) std::cout << "    " << "AF_LOCAL" << std::endl;
        if (family & AF_UNIX) std::cout << "    " << "AF_UNIX" << std::endl;
        if (family & AF_FILE) std::cout << "    " << "AF_FILE" << std::endl;
        if (family & AF_INET) std::cout << "    " << "AF_INET" << std::endl;
        if (family & AF_AX25) std::cout << "    " << "AF_AX25" << std::endl;
        if (family & AF_IPX) std::cout << "    " << "AF_IPX" << std::endl;
        if (family & AF_APPLETALK) std::cout << "    " << "AF_APPLETALK" << std::endl;
        if (family & AF_NETROM) std::cout << "    " << "AF_NETROM" << std::endl;
        if (family & AF_BRIDGE) std::cout << "    " << "AF_BRIDGE" << std::endl;
        if (family & AF_ATMPVC) std::cout << "    " << "AF_ATMPVC" << std::endl;
        if (family & AF_X25) std::cout << "    " << "AF_X25" << std::endl;
        if (family & AF_INET6) std::cout << "    " << "AF_INET6" << std::endl;
        if (family & AF_ROSE) std::cout << "    " << "AF_ROSE" << std::endl;
        if (family & AF_DECnet) std::cout << "    " << "AF_DECnet" << std::endl;
        if (family & AF_NETBEUI) std::cout << "    " << "AF_NETBEUI" << std::endl;
        if (family & AF_SECURITY) std::cout << "    " << "AF_SECURITY" << std::endl;
        if (family & AF_KEY) std::cout << "    " << "AF_KEY" << std::endl;
        if (family & AF_NETLINK) std::cout << "    " << "AF_NETLINK" << std::endl;
        if (family & AF_ROUTE) std::cout << "    " << "AF_ROUTE" << std::endl;
        if (family & AF_PACKET) std::cout << "    " << "AF_PACKET" << std::endl;
        if (family & AF_ASH) std::cout << "    " << "AF_ASH" << std::endl;
        if (family & AF_ECONET) std::cout << "    " << "AF_ECONET" << std::endl;
        if (family & AF_ATMSVC) std::cout << "    " << "AF_ATMSVC" << std::endl;
        if (family & AF_RDS) std::cout << "    " << "AF_RDS" << std::endl;
        if (family & AF_SNA) std::cout << "    " << "AF_SNA" << std::endl;
        if (family & AF_IRDA) std::cout << "    " << "AF_IRDA" << std::endl;
        if (family & AF_PPPOX) std::cout << "    " << "AF_PPPOX" << std::endl;
        if (family & AF_WANPIPE) std::cout << "    " << "AF_WANPIPE" << std::endl;
        if (family & AF_LLC) std::cout << "    " << "AF_LLC" << std::endl;
        if (family & AF_IB) std::cout << "    " << "AF_IB" << std::endl;
        if (family & AF_MPLS) std::cout << "    " << "AF_MPLS" << std::endl;
        if (family & AF_CAN) std::cout << "    " << "AF_CAN" << std::endl;
        if (family & AF_TIPC) std::cout << "    " << "AF_TIPC" << std::endl;
        if (family & AF_BLUETOOTH) std::cout << "    " << "AF_BLUETOOTH" << std::endl;
        if (family & AF_IUCV) std::cout << "    " << "AF_IUCV" << std::endl;
        if (family & AF_RXRPC) std::cout << "    " << "AF_RXRPC" << std::endl;
        if (family & AF_ISDN) std::cout << "    " << "AF_ISDN" << std::endl;
        if (family & AF_PHONET) std::cout << "    " << "AF_PHONET" << std::endl;
        if (family & AF_IEEE802154) std::cout << "    " << "AF_IEEE802154" << std::endl;
        if (family & AF_CAIF) std::cout << "    " << "AF_CAIF" << std::endl;
        if (family & AF_ALG) std::cout << "    " << "AF_ALG" << std::endl;
        if (family & AF_NFC) std::cout << "    " << "AF_NFC" << std::endl;
        if (family & AF_VSOCK) std::cout << "    " << "AF_VSOCK" << std::endl;
        if (family & AF_KCM) std::cout << "    " << "AF_KCM" << std::endl;
        if (family & AF_QIPCRTR) std::cout << "    " << "AF_QIPCRTR" << std::endl;
        if (family & AF_SMC) std::cout << "    " << "AF_SMC" << std::endl;
        if (family & AF_XDP) std::cout << "    " << "AF_XDP" << std::endl;
        if (family & AF_MCTP) std::cout << "    " << "AF_MCTP" << std::endl;
        if (family & AF_MAX) std::cout << "    " << "AF_MAX" << std::endl;
    }

    static void print_info_verbose(::addrinfo* ai)
    {
        if (!ai) return;

        if (ai->ai_canonname)
            std::cout << "Canonical name: " << ai->ai_canonname << std::endl;
        print_flags(ai->ai_flags);
        print_family(ai->ai_family);
    }

    static void print_info_short(::addrinfo* ai)
    {
        std::cout << "Sock len:\n" << "    " << ai->ai_addrlen << std::endl;
        std::cout << "Family:\n" << "    " << ai->ai_family << std::endl;
    }

    void address_info::print_info_verbose()
    {
        dser::print_info_verbose(this->_ai);
    }

    void address_info::print_info_verbose_all()
    {
        auto* ai = this->_ai;
        size_t count = 0;

        if (!ai) return;
        dser::print_info_verbose(ai);
        ai = ai->ai_next;
        ++count;

        while (ai)
        {
            std::cout << "\n" << "Info struct " << count + 1 << ":" << std::endl;
            dser::print_info_short(ai);
            ai = ai->ai_next;
            ++count;
        }

        std::cout << "\nFound " << count << " info structs for address provider" << std::endl;
    }

    const char* address_info::get_error_message()
    {
        return gai_strerror(this->_error);
    }

}
