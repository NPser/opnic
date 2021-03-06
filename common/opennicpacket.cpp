/*
 *   This file is a part of OpenNIC Wizard
 *   Copyright (C) 2012-2015  Mike Sharkey <mike@8bitgeek.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "opennicpacket.h"

#include <QDataStream>
#include <QByteArray>

/** Keys */
const QString OpenNICPacket::tcp_listen_port		= "tcp_listen_port";		/* the tcp port to listen on */
const QString OpenNICPacket::refresh_timer_period	= "refresh_timer_period";	/* the service refresh timer period */
const QString OpenNICPacket::resolver_cache_size	= "resolver_cache_size";	/* the resolver cache size */
const QString OpenNICPacket::resolver_pool			= "resolver_pool";			/* the resolver pool */
const QString OpenNICPacket::resolver_cache			= "resolver_cache";			/* the resolver cache */
const QString OpenNICPacket::resolver_history		= "resolver_history";		/* the resolver history records */
const QString OpenNICPacket::bootstrap_t1_list		= "bootstrap_t1_list";		/* the bootstrap t1 liost */
const QString OpenNICPacket::bootstrap_domains		= "bootstrap_domains";		/* the bootstrap domains */
const QString OpenNICPacket::system_text			= "system_text";			/* the system text */
const QString OpenNICPacket::journal_text			= "journal_text";			/* the journal text */
const QString OpenNICPacket::async_message			= "async_message";			/* an async message */
const QString OpenNICPacket::score_rules			= "score_rules";			/* score rules */
const QString OpenNICPacket::score_internal			= "score_internal";			/* score internal */
const QString OpenNICPacket::update_dns				= "update_dns";				/* force update dns now */
const QString OpenNICPacket::poll_keys				= "poll_keys";				/* poll for key/values */


OpenNICPacket::OpenNICPacket(QObject *parent)
: QObject(parent)
, mRXState(0)
, mRXLength(0)
{
}

OpenNICPacket::~OpenNICPacket()
{
	clear();
}

/**
  * @brief set the name value pair of a data record in the packet
  */
void OpenNICPacket::set(const QString& key, QVariant value)
{
	mData.insert(key,value);
}

/**
  * @return the value from a key
  */
QVariant OpenNICPacket::get(const QString& key)
{
	QVariant rc;
	if (contains(key))
	{
		rc = mData[key];
	}
	return rc;
}

/**
  * @brief determine if the key is contained in teh packet
  */
bool OpenNICPacket::contains(const QString& key)
{
	return mData.contains(key);
}

/**
  * @brief clear the packet state
  */
void OpenNICPacket::clear()
{
	mData.clear();
	mRXState=0;
	mRXLength=0;
}

/**
  * @brief receive chunk of data from a socket to this packet
  */
void OpenNICPacket::recv(QTcpSocket* socket)
{
	QByteArray chunk;
	QDataStream stream(socket);
	switch(mRXState)
	{
	case 0:
		mRXData.clear();
		stream >> mRXLength;
		if (mRXLength == 0xFFFFFFFF)
		{
			return;
		}
		mRXState=1;
	case 1:
		chunk = stream.device()->readAll();
		mRXData.append(chunk);
		if (mRXData.length()<(int)mRXLength)
		{
			return;
		}
		mRXState=0;
		break;
	}
	QDataStream dataStream(&mRXData,QIODevice::ReadOnly);
	dataStream >> mData;
	emit dataReady();
}

/**
  * @brief send this packet out a socket.
  */
void OpenNICPacket::send(QTcpSocket* socket)
{
	if ( socket->isOpen() && socket->isValid() )
	{
		QByteArray bytes;
		QDataStream tcpStream(socket);
		QDataStream byteStream(&bytes,QIODevice::ReadWrite);
		byteStream << mData;
		tcpStream << bytes;
		socket->flush();
	}
}

