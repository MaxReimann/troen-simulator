//std
#include <stdio.h>
#include <string.h>
#include <initializer_list>

//qt
#include <QQueue>
//bullet
#include "LinearMath/btQuaternion.h"
//troen
#include "../constants.h"
#include "../troengame.h"
#include "../controller/bikecontroller.h"
#include "../model/bikemodel.h"
#include "../player.h"

#include "networkmanager.h"
#include <thread>
//raknet


using namespace troen::networking;
#define MAX_CLIENTS 10
#define SERVER_PORT 60000




NetworkManager::NetworkManager(troen::TroenGame *game)
{
	m_packet = new RakNet::Packet;
	peer = RakNet::RakPeerInterface::GetInstance();
	m_sendUpdateMessagesQueue = new QQueue<bikeUpdateMessage>();
	m_sendFenceUpdateMessagesQueue = new QQueue<fenceUpdateMessage>();
	m_sendStatusUpdateMessage = new QQueue<bikeStatusMessage>();
	m_players = std::vector<std::shared_ptr<NetworkPlayerInfo>>();
	m_sendBufferMutex = new QMutex();
	
	m_localBikeController = NULL;
	m_lastUpdateTime = 0;
	m_gameID = 0;
	m_troenGame = game;
	m_gameStarted = false;
	
}

void  NetworkManager::enqueueMessage(bikeUpdateMessage message)
{
	m_sendBufferMutex->lock();
	m_sendUpdateMessagesQueue->enqueue(message);
	m_sendBufferMutex->unlock();
}

void  NetworkManager::enqueueMessage(fenceUpdateMessage message)
{
	m_sendBufferMutex->lock();
	m_sendFenceUpdateMessagesQueue->enqueue(message);
	m_sendBufferMutex->unlock();
}

void  NetworkManager::enqueueMessage(bikeStatusMessage message)
{
	m_sendBufferMutex->lock();
	m_sendStatusUpdateMessage->enqueue(message);
	m_sendBufferMutex->unlock();
}





void NetworkManager::registerRemotePlayerInput(std::shared_ptr<troen::input::RemotePlayer> remotePlayer)
{
	int otherPlayer = 1 - m_gameID; //only works for 2 players
	//wait until other player is registered
	while (getPlayerWithID(otherPlayer) == NULL)
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	getPlayerWithID(otherPlayer)->m_remoteInputPlayer = remotePlayer;
}

void NetworkManager::registerLocalPlayer(troen::Player* player)
{
	m_localPlayer = std::shared_ptr<troen::Player>(player);
	m_localBikeController = player->bikeController();
	m_localBikeModel = m_localBikeController->getModel();
}


void NetworkManager::receiveBikeStatusMessage(bikeStatusMessage message)
{
	getPlayerWithID(message.bikeID)->status =  message.status;
}


void NetworkManager::update(long double g_gameTime)
{
	if (this->isValidSession())
	{
		btVector3 pos = m_localBikeController->getModel()->getPositionBt();
		btQuaternion quat = m_localBikeController->getModel()->getRotationQuat();
		btVector3 linearVelocity = m_localBikeController->getModel()->getLinearVelocity();
		bikeUpdateMessage message = {
			m_gameID,
			pos.x(), pos.y(), pos.z(),
			quat.x(), quat.y(), quat.z(), quat.w(),
			linearVelocity.x(), linearVelocity.y(), linearVelocity.z()
		};

		if ((message.linearVelX != lastSentMessage.linearVelX) || (message.linearVelY != lastSentMessage.linearVelY) || (message.linearVelZ != lastSentMessage.linearVelZ) ||
			(message.quat_x != lastSentMessage.quat_x) || (message.quat_y != lastSentMessage.quat_y) || (message.quat_z != lastSentMessage.quat_z) || (message.quat_w != lastSentMessage.quat_w) ||
			g_gameTime - m_lastUpdateTime > 20.0)
		{
			enqueueMessage(message);
			lastSentMessage = message;
		}

	}
}

void NetworkManager::updateFencePart(btTransform fencePart, int bikeID)
{
	fenceUpdateMessage message = { bikeID, fencePart };
	enqueueMessage(message);
}



//!! This runs in a seperate thread //
void NetworkManager::run()
{
	RakNet::Packet *packet;
	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
				case BIKE_POSITION_MESSSAGE:
				{
											   readMessage(packet, receivedUpdateMessage);
											   if (m_players.size() > 1)
												   getPlayerWithID(receivedUpdateMessage.bikeID)->m_remoteInputPlayer->update(receivedUpdateMessage);
				}
					break;

				case BIKE_STATUS_MESSAGE:
				{
											readMessage(packet, receivedBikeStatusMessage);
											receiveBikeStatusMessage(receivedBikeStatusMessage);
											printf("status_message");

				}
					break;

				case BIKE_FENCE_PART_MESSAGE:
				{

												readMessage(packet, receivedFenceMessage);
												if (m_players.size() > 1)
													getPlayerWithID(receivedFenceMessage.bikeID)->m_remoteInputPlayer->addNewFencePosition(receivedFenceMessage.fencePart);
				}

				case GAME_START_MESSAGE:
				{

										   //prevent game from calling start two times due to receviment of own packet
										   if (!m_gameStarted)
										   {   

											   emit remoteStartCall();
											   m_gameStarted = true;
										   }
				}
					break;
				
				case ADD_PLAYER:
				{
								   addPlayer(packet);
				}
					break;
				
				default:
				{
						   //handle all the client or server related stuff
						   handleSubClassMessages(packet);
				}
					break;



			}

		}
		sendData();
		this->msleep(10);
	}

	//cleanup
	RakNet::RakPeerInterface::DestroyInstance(peer);

}

inline std::shared_ptr<NetworkPlayerInfo> NetworkManager::getPlayerWithID(int bikeID)
{
	for (auto player : m_players)
	{
		if (player->networkID == bikeID)
			return player;
	}
	return NULL;
}

void NetworkManager::sendData()
{
	if (isValidSession())
	{
		sendMessages(m_sendUpdateMessagesQueue, messageToSend, UNRELIABLE_SEQUENCED, BIKE_POSITION_MESSSAGE);
		sendMessages(m_sendStatusUpdateMessage, statusMessageToSend, RELIABLE_SEQUENCED, BIKE_STATUS_MESSAGE);
		sendMessages(m_sendFenceUpdateMessagesQueue, fenceMessageToSend, RELIABLE_SEQUENCED, BIKE_FENCE_PART_MESSAGE);
		
	}
}

void NetworkManager::synchronizeGameStart(troen::GameConfig &config)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GAME_START_MESSAGE);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	m_gameStarted = true;
}


void NetworkManager::buildOwnPlayerInfo(const troen::GameConfig& config)
{
	if (getPlayerWithID(m_gameID) != NULL)
		return;
	int i;
	for (i = 0; i < 6; i++)
	if (config.ownView[i])
		break;

	m_ownPlayerInfo = std::make_shared<NetworkPlayerInfo>(config.playerNames[i], config.playerColors[i], m_gameID, false, m_startPosition);
	m_players.push_back(m_ownPlayerInfo);
}


bool NetworkManager::isValidSession()
{
	//sublcass responsibilty
	return false;
}

template <typename TQueue, typename TSendStruct>
void NetworkManager::sendMessages(QQueue<TQueue> *sendBufferQueue, TSendStruct &messageToSend, int order, int statusMessage)
{

	while (!sendBufferQueue->empty())
	{
		RakNet::BitStream bsOut;
		// Use a BitStream to write a custom user message
		//Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
		bsOut.Write((RakNet::MessageID)static_cast<GameMessages>(statusMessage));
		m_sendBufferMutex->lock();
		messageToSend = sendBufferQueue->dequeue();
		m_sendBufferMutex->unlock();
		bsOut.Write(messageToSend);

		peer->Send(&bsOut, HIGH_PRIORITY, static_cast<PacketReliability>(order), 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	}
}


template <typename T>
void NetworkManager::readMessage(RakNet::Packet *packet, T& readInto)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Read(readInto);
}

QColor NetworkManager::getPlayerColor(int playerID)
{
	//later, we might actually synchronize the chosen color
	return std::vector<QColor>{ QColor(255.0, 0.0, 0.0), QColor(0.0, 255.0, 0.0), QColor(0.0, 0.0, 255.0),
		QColor(255.0, 255.0, 0.0), QColor(0.0, 255.0, 255.0), QColor(255.0, 0.0, 255.0) }[playerID];
}

void troen::networking::NetworkManager::addPlayer(RakNet::Packet *packet)
{

	//instantiate the remote player
	std::shared_ptr<NetworkPlayerInfo> remote_player = std::make_shared<NetworkPlayerInfo>();
	remote_player->setParametersFromRemote(packet);
	
	if (getPlayerWithID(remote_player->networkID) != NULL) //player was added before
	{

		remote_player.reset();
		return;
	}

	m_players.push_back(remote_player); 
	std::cout << "got remote player: " << remote_player->name.toStdString() << std::endl;
}

void troen::networking::NetworkManager::waitOnAllPlayers()
{
	//block until every player has waiting for gamestart status (aka finished initiliazing)
	bool ready = false;
	while (!ready)
	{
		ready = true;
		for (auto player : m_players)
		{
			if (player->status != WAITING_FOR_GAMESTART)
				ready = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		
	}
}

void NetworkManager::setLocalGameReady()
{
	getPlayerWithID(m_gameID)->status = WAITING_FOR_GAMESTART;
	
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)BIKE_STATUS_MESSAGE);
	getPlayerWithID(m_gameID)->serializeStatus(&bsOut);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}



NetworkPlayerInfo::NetworkPlayerInfo(QString name, QColor color, int networkID, bool remote, btTransform position) :
name(name), color(color), networkID(networkID), remote(remote), position(position)
{
	m_remoteInputPlayer = NULL;
	status = INITIALIZING;
	score = 0;
}



void NetworkPlayerInfo::serializeStatus(RakNet::BitStream *bs)
{
	bikeStatusMessage statusMessageToSend = { networkID, status};
	bs->Write(statusMessageToSend);
}

void troen::networking::NetworkPlayerInfo::serialize(RakNet::BitStream *bs)
{
	bs->Write(networkID);

	char message[32];
	strncpy_s(message, name.toStdString().c_str(), 32);
	bs->Write(message);

	bs->WriteVector(color.red(), color.green(), color.blue());

	bs->Write(position);
}

void troen::networking::NetworkPlayerInfo::setParametersFromRemote(RakNet::Packet *packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);

	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Read(networkID);

	char message[32];
	bsIn.Read(message);
	name = QString(message);

	int red, green, blue;
	bsIn.ReadVector(red, green, blue);
	color.setRed(red); color.setGreen(green); color.setBlue(blue);

	bsIn.Read(position);

	remote = true;


}

