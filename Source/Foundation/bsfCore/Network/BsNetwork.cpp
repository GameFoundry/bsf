//************************************ bs::framework - Copyright 2019 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//

#include "BsNetwork.h"
#include "RakNet/RakPeer.h"
#include "RakNet/MessageIdentifiers.h"
#include "Allocators/BsPoolAlloc.h"

using namespace RakNet;

namespace bs
{
	static_assert(NETWORK_USER_MESSAGE_ID == ID_USER_PACKET_ENUM, "");

	/** Converts a RakNet SystemAddress into the framework's NetworkAddress type. */
	void systemToNetworkAddress(const SystemAddress& address, NetworkAddress& output)
	{
		if(address.address.addr4.sin_family == AF_INET)
		{
			output.ipType = IPV4;
			output.port = address.address.addr4.sin_port;
			memcpy(output.ip, &address.address.addr4.sin_addr, 4);
		}
		else
		{
			output.ipType = IPV6;
			output.port = address.address.addr6.sin6_port;
			output.ip6FlowInfo = address.address.addr6.sin6_flowinfo;
			output.ip6ScopeId = address.address.addr6.sin6_scope_id;
			memcpy(output.ip, &address.address.addr6.sin6_addr, 16);
		}
	}

	/**
	 * Same as systemToNetworkAddress(const SystemAddress&, NetworkAddress&) except it returns a brand new NetworkAddress
	 * object.
	 */
	NetworkAddress systemToNetworkAddress(const SystemAddress& address)
	{
		NetworkAddress output;
		systemToNetworkAddress(address, output);
		return output;
	}

	/** Converts the framework's NetworkAddress into RakNet's SystemAddres type. */
	SystemAddress networkToSystemAddress(const NetworkAddress& address)
	{
		SystemAddress output;
		output.address.addr4.sin_port = address.port;

		if(address.ipType == IPV4)
		{
			output.address.addr4.sin_family = AF_INET;
			memcpy(&output.address.addr4.sin_addr, address.ip, 4);
		}
		else
		{
			output.address.addr6.sin6_family = AF_INET6;
			output.address.addr6.sin6_flowinfo = (ULONG)address.ip6FlowInfo;
			output.address.addr6.sin6_scope_id = (ULONG)address.ip6ScopeId;
			memcpy(&output.address.addr6.sin6_addr, address.ip, 16);
		}

		return output;
	}

	/** Converts PacketChannel reliability, priority and ordering into equivalent RakNet enums. */
	void mapChannelToRakNet(const PacketChannel& channel, ::PacketReliability& reliability, ::PacketPriority& priority)
	{
		switch(channel.priority)
		{
		case PacketPriority::Immediate:
			priority = IMMEDIATE_PRIORITY;
			break;
		case PacketPriority::High:
			priority = HIGH_PRIORITY;
			break;
		default:
		case PacketPriority::Medium:
			priority = MEDIUM_PRIORITY;
			break;
		case PacketPriority::Low:
			priority = LOW_PRIORITY;
			break;
		}

		if(channel.reliability == PacketReliability::Unreliable)
		{
			if(channel.ordering == PacketOrdering::Ordered || channel.ordering == PacketOrdering::Sequenced)
				reliability = UNRELIABLE_SEQUENCED;
			else
				reliability = UNRELIABLE;
		}
		else
		{
			switch(channel.ordering)
			{
			default:
			case PacketOrdering::Unordered:
				reliability = RELIABLE;
				break;
			case PacketOrdering::Ordered:
				reliability = RELIABLE_ORDERED;
				break;
			case PacketOrdering::Sequenced:
				reliability = RELIABLE_SEQUENCED;
				break;
			}
		}
	}

	NetworkAddress NetworkAddress::UNASSIGNED;

	NetworkAddress::NetworkAddress(const char* address)
	{
		SystemAddress sysAddress(address);
		systemToNetworkAddress(sysAddress, *this);
	}

	NetworkAddress::NetworkAddress(const char* ip, UINT16 port)
	{
		SystemAddress sysAddress(ip, port);
		systemToNetworkAddress(sysAddress, *this);
	}

	String NetworkAddress::toString(bool withPort) const
	{
		SystemAddress sysAddress = networkToSystemAddress(*this);
		return String(sysAddress.ToString(withPort, '|'));
	}

	bool NetworkAddress::compareIP(const NetworkAddress& other) const
	{
		if(ipType != other.ipType)
			return false;

		if(ipType == IPV4)
			return memcmp(ip, other.ip, 4) == 0;

		return (memcmp(ip, other.ip, sizeof(ip)) == 0 && ip6FlowInfo == other.ip6FlowInfo && ip6ScopeId == other.ip6ScopeId);
	}

	NetworkAddress& NetworkAddress::operator= (const NetworkAddress& rhs)
	{
		ipType = rhs.ipType;
		port = rhs.port;
		ip6FlowInfo = rhs.ip6FlowInfo;
		ip6ScopeId = rhs.ip6ScopeId;

		memcpy(&ip, &rhs.ip, sizeof(ip));
		return *this;
	}

	bool NetworkAddress::operator==(const NetworkAddress& rhs) const
	{
		return port == rhs.port && compareIP(rhs);
	}

	bool NetworkAddress::operator!=(const NetworkAddress& rhs) const
	{
		return !(*this==rhs);
	}

	/** Contains information associated with a single connected peer. */
	struct NetworkConnection
	{
		NetworkId id;
		NetworkAddress networkAddress;
		SystemAddress systemAddress;
		RakNetGUID guid;
	};

	struct NetworkPeer::Pimpl
	{
		RakPeerInterface* peer;
		Vector<NetworkConnection> networkIdMapping;
		PoolAlloc<sizeof(NetworkEvent)> eventPool;

		/** Maps a network ID into a RakNet system address. Returns null if the ID is not valid. */
		const SystemAddress* getSystemAddress(const NetworkId& id)
		{
			if(id.id < 0 || id.id >= (INT32)networkIdMapping.size())
				return nullptr;

			return &networkIdMapping[id.id].systemAddress;
		}

		/** Maps a network ID into a RakNet GUID. Returns null if the ID is not valid. */
		const RakNetGUID* getGUID(const NetworkId& id)
		{
			if(id.id < 0 || id.id >= (INT32)networkIdMapping.size())
				return nullptr;

			return &networkIdMapping[id.id].guid;
		}

		/**
		 * Using a RakNet address or a GUID, attempts to retrieve a network ID, or allocates a new ID if one cannot be
		 * found.
		 */
		NetworkId getOrRegisterNetworkId(const SystemAddress& address, const RakNetGUID& guid)
		{
			INT32 systemIndex = address.systemIndex;
			assert(systemIndex >= 0 && systemIndex < (INT32)networkIdMapping.size());

			NetworkConnection& connection = networkIdMapping[systemIndex];
			if(connection.systemAddress != address || connection.guid != guid)
			{
				connection.networkAddress = systemToNetworkAddress(address);
				connection.systemAddress = address;
				connection.guid = guid;
			}

			return connection.id;
		}

		/** Allocates a new network event, responsible for reporting data received in @p packet. */
		NetworkEvent* allocNetworkEvent(Packet* packet)
		{
			NetworkEvent* event = eventPool.construct<NetworkEvent>();
			event->_backendData = packet;
			event->sender = getOrRegisterNetworkId(packet->systemAddress, packet->guid);

			switch (packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				event->type = NetworkEventType::ConnectingDone;
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				event->type = NetworkEventType::ConnectingFailed;
				break;
			case ID_ALREADY_CONNECTED:
				event->type = NetworkEventType::AlreadyConnected;
				break;
			case ID_NEW_INCOMING_CONNECTION:
				event->type = NetworkEventType::IncomingNew;
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				event->type = NetworkEventType::IncomingNoFree;
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				event->type = NetworkEventType::Disconnected;
				break;
			case ID_CONNECTION_LOST:
				event->type = NetworkEventType::LostConnection;
				break;
			default:
				event->type = NetworkEventType::Data;
				event->data.bytes = packet->data;
				event->data.length = packet->length;
				break;
			}

			return event;
		}

		/** Frees an event allocated with allocNetworkEvent(). */
		void freeNetworkEvent(NetworkEvent* event)
		{
			Packet* packet = (Packet*)event->_backendData;

			peer->DeallocatePacket(packet);
			eventPool.free(event);
		}
	};

	NetworkPeer::NetworkPeer(const NETWORK_PEER_DESC& desc)
		:m(bs_new<Pimpl>())
	{
		m->peer = RakPeerInterface::GetInstance();
		m->networkIdMapping.resize(desc.maxNumConnections);

		for(INT32 i = 0; i < (INT32)m->networkIdMapping.size(); i++)
			m->networkIdMapping[i].id = NetworkId(i);

		UINT32 numDescriptors = (UINT32)desc.listenAddresses.size();
		SocketDescriptor* descriptors = bs_stack_alloc<SocketDescriptor>(numDescriptors);

		for(UINT32 i = 0; i < numDescriptors; i++)
		{
			const NetworkAddress& address = desc.listenAddresses[i];

			if(address.compareIP(NetworkAddress::UNASSIGNED))
			{
				if(address.port == 0)
					descriptors[i] = SocketDescriptor();
				else
					descriptors[i] = SocketDescriptor(address.port, nullptr);
			}
			else
			{
				if(address.ipType == IPV6)
				{
					LOGERR("IPV6 not supported for listener addreses on this backend");
					descriptors[i] = SocketDescriptor();
				}
				else // IPV4
				{
					String addressStr = address.toString();
					descriptors[i] = SocketDescriptor(address.port, addressStr.c_str());
				}
			}
		}

		StartupResult result = m->peer->Startup(desc.maxNumConnections, descriptors, numDescriptors);

		bs_stack_free(descriptors);

		switch(result)
		{
		case RAKNET_ALREADY_STARTED:
			LOGWRN("Failed to start RakNet peer, RakNet already started.");
			break;
		case INVALID_SOCKET_DESCRIPTORS:
			LOGERR("Failed to start RakNet peer, invalid socket descriptors provided.");
			break;
		case INVALID_MAX_CONNECTIONS:
			LOGERR("Failed to start RakNet peer, invalid max. connection count provided.");
			break;
		case SOCKET_FAMILY_NOT_SUPPORTED:
			LOGERR("Failed to start RakNet peer, socket family not supported.");
			break;
		case SOCKET_PORT_ALREADY_IN_USE:
			LOGERR("Failed to start RakNet peer, port already in use.");
			break;
		case SOCKET_FAILED_TO_BIND:
			LOGERR("Failed to start RakNet peer, socket failed to bind.");
			break;
		case SOCKET_FAILED_TEST_SEND:
			LOGERR("Failed to start RakNet peer, socket failed to test send.");
			break;
		case PORT_CANNOT_BE_ZERO:
			LOGERR("Failed to start RakNet peer, port cannot be zero.");
			break;
		case FAILED_TO_CREATE_NETWORK_THREAD:
			LOGERR("Failed to start RakNet peer, failed to create the network thread.");
			break;
		case COULD_NOT_GENERATE_GUID:
			LOGERR("Failed to start RakNet peer, failed to generate GUID.");
			break;
		case STARTUP_OTHER_FAILURE:
			LOGERR("Failed to start RakNet peer, unknown failure.");
			break;
		default:
			break;
		}

		if(desc.maxNumIncomingConnections > 0)
			m->peer->SetMaximumIncomingConnections(desc.maxNumIncomingConnections);
	}

	NetworkPeer::~NetworkPeer()
	{
		RakPeerInterface::DestroyInstance(m->peer);

		bs_delete(m);
	}

	bool NetworkPeer::connect(const char* host, UINT16 port)
	{
		ConnectionAttemptResult result = m->peer->Connect(host, port, nullptr, 0);
		
		if(result != CONNECTION_ATTEMPT_STARTED)
		{
			switch(result)
			{
			case INVALID_PARAMETER:
				LOGERR_FMT("Unable to connect to {0}|{1}, invalid parameter.", host, port);
				break;
			case CANNOT_RESOLVE_DOMAIN_NAME:
				LOGERR_FMT("Unable to connect to {0}|{1}, domain name cannot be resolved.", host, port);
				break;
			case ALREADY_CONNECTED_TO_ENDPOINT:
				LOGWRN_FMT("Unable to connect to {0}|{1}, already connected.", host, port);
				break;
			case CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS:
				LOGWRN_FMT("Unable to connect to {0}|{1}, connection attempt already in progress.", host, port);
				break;
			case SECURITY_INITIALIZATION_FAILED:
				LOGERR_FMT("Unable to connect to {0}|{1}, security initialization failed.", host, port);
				break;
			default:
				break;
			}

			return false;
		}

		return true;
	}

	void NetworkPeer::disconnect(const NetworkAddress& address, bool silent)
	{
		m->peer->CloseConnection(networkToSystemAddress(address), !silent);
	}

	void NetworkPeer::disconnect(const NetworkId& id, bool silent)
	{
		const RakNetGUID* guid = m->getGUID(id);
		if(!guid)
		{
			LOGERR_FMT("Cannot disconnect from {0}, invalid network ID provided.", id.id);
			return;
		}

		m->peer->CloseConnection(*guid, !silent);
	}

	NetworkEvent* NetworkPeer::receive() const
	{
		Packet* packet = m->peer->Receive();
		if(!packet || packet->length == 0)
			return nullptr;

		return m->allocNetworkEvent(packet);
	}

	void NetworkPeer::free(NetworkEvent* event)
	{
		if(!event)
			return;

		m->freeNetworkEvent(event);
	}

	void NetworkPeer::send(const PacketData& data, const NetworkAddress& address, const PacketChannel& channel)
	{
		::PacketReliability reliability;
		::PacketPriority priority;

		mapChannelToRakNet(channel, reliability, priority);

		m->peer->Send((const char*)data.bytes, (INT32)data.length, priority, reliability, 0,
			networkToSystemAddress(address), false);
	}

	void NetworkPeer::send(const PacketData& data, const NetworkId& id, const PacketChannel& channel)
	{
		const RakNetGUID* guid = m->getGUID(id);
		if(!guid)
		{
			LOGERR_FMT("Cannot send to {0}, invalid network ID provided.", id.id);
			return;
		}

		::PacketReliability reliability;
		::PacketPriority priority;

		mapChannelToRakNet(channel, reliability, priority);

		m->peer->Send((const char*)data.bytes, (INT32)data.length, priority, reliability, 0,
			*guid, false);
	}

	void NetworkPeer::broadcast(const PacketData& data, const PacketChannel& channel)
	{
		::PacketReliability reliability;
		::PacketPriority priority;

		mapChannelToRakNet(channel, reliability, priority);

		m->peer->Send((const char*)data.bytes, (INT32)data.length, priority, reliability, 0,
			UNASSIGNED_RAKNET_GUID, true);
	}

}
